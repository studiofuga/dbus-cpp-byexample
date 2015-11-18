//
// Created by <fuga@studiofuga.com> on 03/11/15.
//

#include <iostream>

#include <core/dbus/bus.h>
#include <core/dbus/resolver.h>
#include <core/dbus/well_known_bus.h>
#include <core/dbus/asio/executor.h>

#define MYSERVICES "com.studiofuga.test"
#define MYOBJECTPATH "/com/studiofuga/test"

using namespace std;
using namespace core::dbus;

struct MySignal {
    static string name() { return MYSERVICES; }

    struct Signals {
        struct Sample {
            inline static std::string name() {
                return "Sample";
            }
            using Interface = MySignal;
            using ArgumentType = string;

            using SignalPtr = shared_ptr<Signal<Sample, ArgumentType>>;
        };
    };
};

namespace core {
    namespace dbus {
        namespace traits {
            template<>
            struct Service<MySignal> {
                inline static const std::string interface_name() {
                    return std::string{MySignal::name()};
                }
            };
        }
    }
}


namespace dbus = core::dbus;

int main()
{
    auto bus = std::make_shared<dbus::Bus>(dbus::WellKnownBus::session);
    bus->install_executor(core::dbus::asio::make_executor(bus));
    std::thread t1 { [&]() { bus->run(); } };

    auto service = Service::use_service(bus, MYSERVICES);
    auto object = service->object_for_path(types::ObjectPath{MYOBJECTPATH});
    auto sig = object->get_signal<MySignal::Signals::Sample>();
    sig->connect([](const MySignal::Signals::Sample::ArgumentType &arg) {
        cout << "Signal \"Sample\" received: " << arg << endl;
    });

    if (t1.joinable())
        t1.join();
    return EXIT_SUCCESS;
}