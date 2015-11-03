//
// Created by <fuga@studiofuga.com> on 03/11/15.
//

#ifndef DBTEST_OBJECTMANAGER_H
#define DBTEST_OBJECTMANAGER_H


#include <core/dbus/skeleton.h>
#include <core/dbus/object.h>
#include <core/dbus/types/variant.h>
#include <core/dbus/types/object_path.h>
#include <Helpers.h>

#include <core/dbus/message_streaming_operators.h>

namespace sf {
    namespace dbus {
        namespace dbus = core::dbus;
        namespace types = core::dbus::types;
        using namespace std;

        // org.freedesktop.DBus.ObjectManager.GetManagedObjects (out DICT<OBJPATH,DICT<STRING,DICT<STRING,VARIANT>>> objpath_interfaces_and_properties);
        class ObjectManagerInterface {
        public:
            struct GetManagedObjects {
                METHOD_SIGNATURE("GetManagedObjects", ObjectManagerInterface);
            };
        public:
            virtual ~ObjectManagerInterface() noexcept = default;
        };

        //

        class ObjectManagerSkeleton {
        public:

            ObjectManagerSkeleton(const dbus::Bus::Ptr &bus, const dbus::Service::Ptr &service)
                    : mBus(bus), mService(service)
            {
            }

            void init( const dbus::Object::Ptr &object) {
                mObject = object;
                mObject->install_method_handler<ObjectManagerInterface::GetManagedObjects>(
                        std::bind(&ObjectManagerSkeleton::handleGetManagedObjects, this, std::placeholders::_1));
            }

            ~ObjectManagerSkeleton() noexcept = default;
        private:
            dbus::Bus::Ptr mBus;
            dbus::Service::Ptr mService;
            dbus::Object::Ptr mObject;

            void handleGetManagedObjects(const dbus::Message::Ptr &msg) {
                auto reply = dbus::Message::make_method_return(msg);
                reply->writer() << getManagedObjects();
                mBus->send(reply);
            };

        public:
            using VariantDict = map<string, types::Variant>;
            using ObjectDict = map<string, VariantDict>;
            using ObjectDetails = map<types::ObjectPath, ObjectDict>;

            virtual ObjectDetails getManagedObjects() {
                return ObjectDetails {};
            };
        };

    }
}

namespace core {
    namespace dbus {
        namespace traits {
            template<>
            struct Service<sf::dbus::ObjectManagerInterface> {
                inline static const std::string interface_name() {
                    return std::string{"org.freedesktop.DBus.ObjectManager"};
                }
            };
        }
    }
}


#endif //DBTEST_OBJECTMANAGER_H
