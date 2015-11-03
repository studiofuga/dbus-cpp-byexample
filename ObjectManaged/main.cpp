//
// Created by <fuga@studiofuga.com> on 03/11/15.
//

#include <iostream>

#include <core/dbus/bus.h>
#include <core/dbus/announcer.h>
#include <core/dbus/well_known_bus.h>
#include <core/dbus/asio/executor.h>

#include "SimpleServiceWithObjectManager.h"

namespace dbus = core::dbus;

int main()
{
    auto bus = std::make_shared<dbus::Bus>(dbus::WellKnownBus::session);
    bus->install_executor(core::dbus::asio::make_executor(bus));
    std::thread t1 { [&]() { bus->run(); } };

    auto service = dbus::announce_service_on_bus<srv::IService, srv::Service>(bus);

    if (t1.joinable())
        t1.join();
    return EXIT_SUCCESS;
}

