#include <iostream>
#include <memory>
using namespace std;

#include "Service.h"
using namespace my;

#include <core/dbus/bus.h>
#include <core/dbus/service.h>
#include <core/dbus/object.h>
#include <core/dbus/asio/executor.h>

#include <core/dbus/types/stl/vector.h>
#include <core/dbus/message_streaming_operators.h>
namespace dbus = core::dbus;

template<typename T>
using Ptr = std::shared_ptr<T>;

Ptr<dbus::Bus> mBus;

const char *const DBUS_INTROSPECTION_XML =
"<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\"\n"
        "        \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\n"
        "<node>\n"
        "    <interface name=\"org.freedesktop.DBus.Introspectable\">\n"
        "        <method name=\"Introspect\">\n"
        "            <arg name=\"data\" direction=\"out\" type=\"s\"/>\n"
        "        </method>\n"
        "    </interface>\n"
        "    <interface name=\"com.studiofuga.test.dbus\">\n"
        "        <method name=\"TestVec\" />\n"
        "        <method name=\"TestDic\" />\n"
        "        <method name=\"TestDV\" />\n"
        "    </interface>\n"
        "</node>";

void handle_introspection(const dbus::Message::Ptr &msg)
{
    auto reply = dbus::Message::make_method_return(msg);
    reply->writer().push_stringn(DBUS_INTROSPECTION_XML, strlen(DBUS_INTROSPECTION_XML));
    mBus->send(reply);
}

void handle_testvec(const dbus::Message::Ptr &msg)
{
    std::vector<std::string> d { "first" , "second", "third"};

    cout << "Received Vec" << endl;
    auto reply = dbus::Message::make_method_return(msg);

    reply->writer() << d;
    mBus->send(reply);
}

void handle_testdic(const dbus::Message::Ptr &msg)
{
    std::map<std::string, std::vector<std::string>> d { {"first" , {"one", "two", "three"}},
                                                        {"second", {"a", "b", "c"}},
                                                        {"third", {"1", "2", "111"}}};

    cout << "Received TestDic" << endl;
    auto reply = dbus::Message::make_method_return(msg);

    reply->writer() << d;
    mBus->send(reply);
}

void handle_testdv(const dbus::Message::Ptr &msg)
{
    using V = dbus::types::Variant;
    std::map<std::string, V> d;

    d.insert(make_pair("string", V::encode(std::string{"uno"})));
    d.insert(make_pair("int", V::encode(2)));
    d.insert(make_pair("map", V::encode(map<string,string>{{"uno","uno"},{"due","due"}})));

    cout << "Received TestDv: " << msg->destination() << endl;
    auto reply = dbus::Message::make_method_return(msg);

    reply->writer() << d;
    mBus->send(reply);
}

namespace
{
    dbus::Bus::Ptr the_session_bus()
    {
        static dbus::Bus::Ptr system_bus = std::make_shared<dbus::Bus>(dbus::WellKnownBus::session);
        return system_bus;
    }
}

int main()
{

    std::thread mThread;
    Ptr<dbus::Service> mService;
    Ptr<dbus::Object> mObject;

    mBus = the_session_bus();

    mBus->install_executor(core::dbus::asio::make_executor(mBus));
    mThread = std::thread {std::bind(&dbus::Bus::run, mBus)};

    mService = dbus::Service::add_service(mBus, Service::ServiceName());
    mObject = mService->add_object_for_path(Service::ObjectPath());
    mObject->install_method_handler<my::Service::Introspect>(
            std::bind(&handle_introspection, std::placeholders::_1));
    mObject->install_method_handler<my::Service::TestVec>(
            std::bind(&handle_testvec, std::placeholders::_1));
    mObject->install_method_handler<my::Service::TestDict>(
            std::bind(&handle_testdic, std::placeholders::_1));
    mObject->install_method_handler<my::Service::TestDV>(
            std::bind(&handle_testdv, std::placeholders::_1));


    mThread.join();

    return 0;
}