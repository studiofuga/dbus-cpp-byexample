//
// Created by <fuga@studiofuga.com> on 03/11/15.
//

#include <iostream>

#include <core/dbus/bus.h>
#include <core/dbus/resolver.h>
#include <core/dbus/well_known_bus.h>
#include <core/dbus/asio/executor.h>
#include <unistd.h>
#include <core/dbus/property.h>

#define MYSERVICES "com.studiofuga.test"
#define MYOBJECTPATH "/com/studiofuga/test"

using namespace std;
using namespace core::dbus;

struct MyObject {
    static string name() { return MYSERVICES; }

    struct Properties {
        struct Num {
            inline static string name()
            {
                return std::string {"Num"};
            };
            using Interface = MyObject;
            using ValueType = uint32_t ;
            static const bool readable = true;
            static const bool writable = false;
        };
    };
};

namespace core {
    namespace dbus {
        namespace traits {
            template<>
            struct Service<MyObject> {
                inline static const std::string interface_name() {
                    return std::string{MyObject::name()};
                }
            };
        }
    }
}


namespace dbus = core::dbus;

int main()
{
    auto bus = std::make_shared<dbus::Bus>(dbus::WellKnownBus::session);

    std::promise<void> promise;
    auto future = promise.get_future();
    std::promise<void> endpromise;
    auto endfuture = endpromise.get_future();

    auto server = [&bus, &promise, &endpromise] () {
        bus->install_executor(core::dbus::asio::make_executor(bus));
        std::thread t1 { [&]() { bus->run(); } };

        auto service = Service::add_service(bus, MYSERVICES);
        auto object = service->add_object_for_path(types::ObjectPath(MYOBJECTPATH));

        std::cout << "Server starting" << std::endl;

        promise.set_value();

        auto property = object->get_property<MyObject::Properties::Num>();
        property->set(0);

        int n = 0;
        while (n < 100) {
            ::usleep(1000000);
            ++n;

            property->set(n);

/* This doesn't work as well ...*/
#if 1
            auto changed_signal = object->get_signal<core::dbus::interfaces::Properties::Signals::PropertiesChanged>();
            core::dbus::interfaces::Properties::Signals::PropertiesChanged::ArgumentType
                    args(MYSERVICES,
                         {{MyObject::Properties::Num::name(),
                                  core::dbus::types::TypedVariant<MyObject::Properties::Num::ValueType>(n)}},
                         {});
            object->emit_signal<core::dbus::interfaces::Properties::Signals::PropertiesChanged, core::dbus::interfaces::Properties::Signals::PropertiesChanged::ArgumentType>(args);
            // send only once.
            //changed_signal->emit(args);
#endif

            std::cout << "Update: " << n << std::endl;
        }

        endpromise.set_value();
        std::cout << "Server exit " << std::endl;

        bus->stop();
        if (t1.joinable())
            t1.join();
    };

    auto client = [&bus, &future, &endfuture]() {
        bus->install_executor(core::dbus::asio::make_executor(bus));
        std::thread t1 { [&]() { bus->run(); } };

        // ensure the client starts after the server
        future.get();

        std::cout << "Client starting" << std::endl;

        auto service = Service::use_service<MyObject>(bus);
        auto object = service->object_for_path(types::ObjectPath{MYOBJECTPATH});

        auto property = object->get_property<MyObject::Properties::Num>();
        property->changed().connect([](uint32_t n) {
            std::cout << "Changed: " << n << std::endl;
        });

        endfuture.get();
        std::cout << "Client Exit" << std::endl;

        bus->stop();
        if (t1.joinable())
            t1.join();
    };

    std::thread sthread(server);
    std::thread cthread(client);

    sthread.join();
    cthread.join();

    return EXIT_SUCCESS;
}