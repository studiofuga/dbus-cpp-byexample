//
// Created by <fuga@studiofuga.com> on 03/11/15.
//

#include <iostream>

#include <core/dbus/bus.h>
#include <core/dbus/service.h>
#include <core/dbus/object.h>
#include <core/dbus/well_known_bus.h>
#include <core/dbus/asio/executor.h>

#define MYSERVICES "com.studiofuga.test"

using namespace std;

struct ServiceInterface {
    static string name() { return MYSERVICES; }

    struct Methods {
        struct DoSomething {
            inline static std::string name() { return "DoSomething"; }
            using Interface = ServiceInterface;
            inline static const std::chrono::milliseconds default_timeout() { return std::chrono::seconds(1); }
        };
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
// dbus-send --session --print-reply --dest=com.studiofuga.test /com/studiofuga/test com.studiofuga.test.DoSomething string:"abracarabra"


int main()
{
    auto bus = std::make_shared<dbus::Bus>(dbus::WellKnownBus::session);
    bus->install_executor(core::dbus::asio::make_executor(bus));

    auto service = core::dbus::Service::add_service(bus, ServiceInterface::name());
    auto object = service->add_object_for_path(core::dbus::types::ObjectPath{"/com/studiofuga/test"});

    object->install_method_handler<ServiceInterface::Methods::DoSomething>([&bus](const core::dbus::MessagePtr &msg) {
        std::cout << "Do Something Useful..." << std::endl;

        string s;
        dbus::MessagePtr reply = core::dbus::Message::make_method_return(msg);
        msg->reader() >> s;
        reply->writer() << s;

        bus->send(reply);
    });

    std::thread t1 { [&]() { bus->run(); } };

    if (t1.joinable())
        t1.join();

    return EXIT_SUCCESS;
}