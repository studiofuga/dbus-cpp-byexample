//
// Created by <fuga@studiofuga.com> on 04/11/15.
//

#ifndef DBTEST_CLIENTSTUB_H
#define DBTEST_CLIENTSTUB_H

#include <Helpers.h>

#include <core/dbus/stub.h>
#include <core/dbus/property.h>
#include <core/dbus/types/stl/vector.h>

namespace my {

    namespace dbus = core::dbus;
    using namespace std;

    struct SecretService {
        SERVICE_INTERFACE("org.freedesktop.Secret.Service", "/org/freedesktop/secrets");
    };

    struct ClientInterface {
        SERVICE_INTERFACE("org.gnome.keyring", "/org/freedesktop/secrets");

        struct Properties {
            struct Collections {
                inline static std::string name()
                {
                    return "Collections";
                };
                using Interface = SecretService;
                using ValueType = vector<dbus::types::ObjectPath>;
                static const bool readable = true;
                static const bool writable = false;
            };
        };

    public:
        virtual vector<dbus::types::ObjectPath> getCollections() = 0;
    };

    struct ClientStub : public dbus::Stub<ClientInterface> {
    public:
        using Ptr = std::shared_ptr<ClientStub>;

        ClientStub(const dbus::Bus::Ptr &bus)
                : dbus::Stub<ClientInterface>(bus) ,
                  object(access_service()->object_for_path(ClientInterface::ObjectPath()))
        {
        }

        virtual vector<dbus::types::ObjectPath> getCollections() override {
            return object->get_property<ClientInterface::Properties::Collections>()->get();
        }

    protected:
        dbus::Object::Ptr object;
    };
}

namespace core {
    namespace dbus {
        namespace traits {
            template<>
            struct Service<my::ClientInterface> {
                inline static const std::string interface_name() {
                    return std::string{"org.gnome.keyring"};
                }
            };
            template <>
            struct Service<my::SecretService> {
                inline static const string interface_name() { return string{"org.freedesktop.Secret.Service"};}
            };
        }
    }
}

#endif //DBTEST_CLIENTSTUB_H
