//
// Created by <fuga@studiofuga.com> on 03/11/15.
//

#include <iostream>

#include <core/dbus/bus.h>
#include <core/dbus/announcer.h>
#include <core/dbus/well_known_bus.h>
#include <core/dbus/asio/executor.h>
#include <core/dbus/resolver.h>

#include "Service.h"
#include "ClientStub.h"

namespace dbus = core::dbus;
using namespace std;

int main()
{
    cout << "Test the property with:" <<
        " dbus-send --session --print-reply --dest=com.studiofuga.test /com/studiofuga/test  org.freedesktop.DBus.Properties.Get string:\"com.studiofuga.test\" string:\"UUID\""
        <<endl;

    auto bus = std::make_shared<dbus::Bus>(dbus::WellKnownBus::session);
    bus->install_executor(core::dbus::asio::make_executor(bus));
    std::thread t1 { [&]() { bus->run(); } };

    auto benchmark_service = dbus::announce_service_on_bus<srv::IService, srv::Service>(bus);

    auto svc = dbus::resolve_service_on_bus<my::ClientInterface, my::ClientStub>(bus);
    auto r = svc->getCollections();
    for (auto x: r) {
        cout << x << endl;
    }

    if (t1.joinable())
        t1.join();
    return EXIT_SUCCESS;
}