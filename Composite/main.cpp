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

class FirstObject : public ObjectManagerHelper {
public:
    FirstObject(const core::dbus::Service::Ptr &service)
            : object (service->add_object_for_path(types::ObjectPath{"/com/studiofuga/test/first"})) {

        propNum = object->get_property<Properties::Num>();
    }

    virtual sf::dbus::ObjectManagerSkeleton::ObjectDetails getManagedObjects() {
        sf::dbus::ObjectManagerSkeleton::ObjectDetails d;

        d.insert(std::make_pair(types::ObjectPath{"/com/studiofuga/test/first"}, getObjectDirectory()));

        return d;
    }

public:
    struct Methods {

    };

    struct Properties {
        struct Num {
            inline static string name()
            {
                return std::string {"Num"};
            };
            using Interface = ServiceInterface;
            using ValueType = uint32_t ;
            static const bool readable = true;
            static const bool writable = false;
        };
    };

protected:
    core::dbus::Object::Ptr object;

    Prop<Properties::Num> propNum;

    map<string,  map<string, types::Variant>> getObjectDirectory()
    {
        return { {ServiceInterface::name(), getAllProperties()} };
    }
    map<string, types::Variant> getAllProperties() {
        map<string, types::Variant> m;

        m.insert(getProperty(propNum));

        return m;
    };
};

class SecondObject {
public:
    SecondObject(const core::dbus::Service::Ptr &service)
            : object (service->add_object_for_path(types::ObjectPath{"/com/studiofuga/test/second"})) {
    }
    virtual sf::dbus::ObjectManagerSkeleton::ObjectDetails getManagedObjects() {
        sf::dbus::ObjectManagerSkeleton::ObjectDetails d;
        return d;
    }

protected:
    core::dbus::Object::Ptr object;

public:
    struct Properties {

    };

    struct Methods {

    };
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
        }
    }
}


namespace dbus = core::dbus;

// Test with
// dbus-send --session --print-reply --dest=com.studiofuga.test / org.freedesktop.DBus.ObjectManager.GetManagedObjects
// dbus-send --session --print-reply --dest=com.studiofuga.test /com/studiofuga/test/first org.freedesktop.DBus.Properties.Get string:"com.studiofuga.test" string:"Num"

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