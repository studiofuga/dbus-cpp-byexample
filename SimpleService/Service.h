//
// Created by <fuga@studiofuga.com> on 03/11/15.
//

#ifndef DBTEST_SERVICE_H
#define DBTEST_SERVICE_H

#include <Helpers.h>

#include <string>
#include <memory>

#include <core/dbus/service.h>
#include <core/dbus/stub.h>
#include <core/dbus/skeleton.h>
#include <core/dbus/bus.h>
#include <core/dbus/object.h>

#include <core/dbus/message_streaming_operators.h>

namespace dbus = core::dbus;
using namespace std;

namespace srv {
    struct Introspection {
        SERVICE_INTERFACE("org.freedesktop.DBus.Introspectable", "/com/studiofuga/test");
    };

    class IService {
    protected:
        SERVICE_INTERFACE("com.studiofuga.test", "/com/studiofuga/test");

        struct GetListOfStrings {

            METHOD_SIGNATURE("GetListOfStrings", IService);

            /*typedef IService Interface;

            inline static const std::string& name() {
                static const std::string s { "GetListOfStrings" };
                return s;
            }

            inline static const std::chrono::milliseconds default_timeout()
            {
                return std::chrono::seconds{1};
            }*/
        };

        struct Introspect
        {
            using Interface = Introspection;
            inline static std::string name()
            {
                return "Introspect";
            }
            static const bool call_synchronously = true;
            inline static const std::chrono::milliseconds default_timeout()
            {
                return std::chrono::seconds{1};
            }
        };

        struct Properties {
            struct UUID {
                inline static string name()
                {
                    return std::string {"UUID"};
                };
                using Interface = IService;
                using ValueType = string;
                static const bool readable = true;
                static const bool writable = false;
            };
            struct Primary {
                inline static string name()
                {
                    return "Primary";
                };
                using Interface = IService;
                using ValueType = bool;
                static const bool readable = true;
                static const bool writable = false;
            };
            struct Characteristics {
                inline static string name()
                {
                    return "Characteristics";
                };
                using Interface = IService;
                using ValueType = vector<string>;
                static const bool readable = true;
                static const bool writable = false;
            };
        };
    public:
        virtual ~IService() = default;

        // methods to be implemented
        virtual std::vector<std::string> getListOfStrings(int32_t num) = 0;
    };
}

namespace core {
    namespace dbus {
        namespace traits {
            template<>
            struct Service<srv::IService> {
                inline static const std::string interface_name() {
                    return std::string{"com.studiofuga.test"};
                }
            };

            template<>
            struct Service<srv::Introspection>
            {
                inline static const std::string& interface_name()
                {
                    static const std::string s{ "org.freedesktop.DBus.Introspectable" };
                    return s;
                }
            };

        }
    }
}


namespace srv {

    /** The service Stub is a stub class to be used by the Client. It implements a proxy for the remote object. */
    class ServiceStub : public core::dbus::Stub<IService> {
    public:
        using Ptr = std::shared_ptr<ServiceStub>;

        ServiceStub(const dbus::Bus::Ptr& bus)
                : dbus::Stub<IService>(bus),
                  object(access_service()->object_for_path(dbus::types::ObjectPath("/com/studiofuga/test")))
        {
        }

        ~ServiceStub() noexcept = default;

        std::vector<std::string> getListOfStrings(int32_t num)
        {
            auto result = object->invoke_method_synchronously<IService::GetListOfStrings, std::vector<std::string>>(num);
            if (result.is_error())
                throw std::runtime_error(result.error().print());

            return result.value();
        }

    protected:
        dbus::Object::Ptr object;
    };


    class ServiceSkeleton : public core::dbus::Skeleton<IService> {
    public:
        ServiceSkeleton(const dbus::Bus::Ptr& bus)
                : dbus::Skeleton<IService>(bus),
                  object(access_service()->add_object_for_path(dbus::types::ObjectPath(IService::ObjectPath())))
        {
            object->install_method_handler<IService::GetListOfStrings>(
                    std::bind(&ServiceSkeleton::handleGetListOfStrings, this, std::placeholders::_1));

//            object->install_method_handler<IService::GetListOfStrings>(
//                    [&](int32_t num) { handleGetListOfStrings(num); });
        }

        ~ServiceSkeleton() noexcept = default;

    private:
        void handleGetListOfStrings(const dbus::Message::Ptr& msg)
        {
            int32_t num;
            msg->reader() >> num;
            auto out = getListOfStrings(num);
            auto reply = dbus::Message::make_method_return(msg);
            reply->writer() << out;
            access_bus()->send(reply);
        }

        dbus::Object::Ptr object;
    };

    class Service: public ServiceSkeleton {
    public:
        typedef std::shared_ptr<Service> Ptr;

        Service(const dbus::Bus::Ptr& bus) : ServiceSkeleton(bus)
        {
        }

        ~Service() noexcept = default;

        std::vector<std::string> getListOfStrings(int32_t num)
        {
            std::vector<std::string> v;
            for (int i = 0; i < num; ++i)
                v.emplace_back(std::string{"Sample"});
            return v;
        }
    };
}

#endif //DBTEST_SERVICE_H
