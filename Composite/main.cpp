//
// Created by <fuga@studiofuga.com> on 03/11/15.
//

#include <iostream>

#include <core/dbus/bus.h>
#include <core/dbus/service.h>
#include <core/dbus/object.h>
#include <core/dbus/resolver.h>
#include <core/dbus/well_known_bus.h>
#include <core/dbus/asio/executor.h>
#include <unistd.h>
#include <core/dbus/property.h>
#include "ObjectManager.h"

#define MYSERVICES "com.studiofuga.test"

using namespace std;
using namespace core::dbus;

struct ServiceInterface {
    static string name() { return MYSERVICES; }
};

struct OtherInterface {
    static string name() { return "com.studiofuga.test.alt"; }
};

class FirstObject : public sf::dbus::ObjectManagerHelper {
public:
    FirstObject(const core::dbus::Service::Ptr &service)
            : sf::dbus::ObjectManagerHelper(core::dbus::types::ObjectPath{"/com/studiofuga/test/first"}),
              object (service->add_object_for_path(getObjectPath())) {

        propNum = makeProperty<Properties::Num>(object);
    }

public:
    struct Properties {
        struct Num {
            inline static string name()
            {
                return std::string {"Num"};
            };
            using Interface = ServiceInterface;
            using ValueType = uint32_t ;
            static const bool readable = true;
            static const bool writable = true;
        };
    };

protected:
    core::dbus::Object::Ptr object;
    Prop<Properties::Num> propNum;
};

class SecondObject : public sf::dbus::ObjectManagerHelper {
public:
    SecondObject(const core::dbus::Service::Ptr &service)
            : ObjectManagerHelper(core::dbus::types::ObjectPath{"/com/studiofuga/test/second"}),
              object (service->add_object_for_path(getObjectPath())) {
        propName = makeProperty<Properties::Name>(object);
        propComments = makeProperty<Properties::Comments>(object);
        propValue = makeProperty<Properties::Value>(object);
    }

public:
    struct Properties {
        struct Name {
            inline static string name()
            {
                return std::string {"Name"};
            };
            using Interface = OtherInterface;
            using ValueType = std::string;
            static const bool readable = true;
            static const bool writable = false;
        };
        struct Comments {
            inline static string name()
            {
                return std::string {"Comments"};
            };
            using Interface = ServiceInterface;
            using ValueType = std::string;
            static const bool readable = true;
            static const bool writable = true;
            inline static const std::chrono::milliseconds default_timeout() { return std::chrono::seconds(1); }
        };
        struct Value {
            inline static string name()
            {
                return std::string {"Value"};
            };
            using Interface = ServiceInterface;
            using ValueType = uint32_t;
            static const bool readable = true;
            static const bool writable = true;
            inline static const std::chrono::milliseconds default_timeout() { return std::chrono::seconds(1); }
        };
    };

protected:
    core::dbus::Object::Ptr object;

    Prop<Properties::Name> propName;
    Prop<Properties::Comments> propComments;
    Prop<Properties::Value> propValue;
};

namespace core {
    namespace dbus {
        namespace traits {
            template<>
            struct Service<ServiceInterface> {
                inline static const std::string interface_name() {
                    return std::string{ServiceInterface::name()};
                }
            };
            template<>
            struct Service<OtherInterface> {
                inline static const std::string interface_name() {
                    return std::string{OtherInterface::name()};
                }
            };
        }
    }
}


namespace dbus = core::dbus;

// Test with
// dbus-send --session --print-reply --dest=com.studiofuga.test / org.freedesktop.DBus.ObjectManager.GetManagedObjects
// dbus-send --session --print-reply --dest=com.studiofuga.test /com/studiofuga/test/first org.freedesktop.DBus.Properties.Get string:"com.studiofuga.test" string:"Num"
// dbus-send --session --print-reply --dest=com.studiofuga.test /com/studiofuga/test/second org.freedesktop.DBus.Properties.Get string:"com.studiofuga.test" string:"Name"
// dbus-send --session --print-reply --dest=com.studiofuga.test /com/studiofuga/test/first org.freedesktop.DBus.Properties.Set string:"com.studiofuga.test" string:"Num" variant:uint32:32

int main()
{
    auto bus = std::make_shared<dbus::Bus>(dbus::WellKnownBus::session);
    bus->install_executor(core::dbus::asio::make_executor(bus));

    auto service = Service::add_service(bus, ServiceInterface::name());
    auto object1 = std::make_shared<FirstObject>(service);
    auto object2 = std::make_shared<SecondObject>(service);
    auto objectManager = std::make_shared<sf::dbus::ObjectManagerSkeleton>(bus, service);

    objectManager->addObjectManaged([&object1]() {
        return object1->getManagedObjects();
    });
    objectManager->addObjectManaged([&object2]() {
        return object2->getManagedObjects();
    });

    std::thread t1 { [&]() { bus->run(); } };

    if (t1.joinable())
        t1.join();

    return EXIT_SUCCESS;
}