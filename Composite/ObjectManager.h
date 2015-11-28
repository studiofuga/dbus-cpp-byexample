//
// Created by <fuga@studiofuga.com> on 03/11/15.
//

#ifndef DBTEST_OBJECTMANAGER_H
#define DBTEST_OBJECTMANAGER_H


#include <core/dbus/skeleton.h>
#include <core/dbus/stub.h>
#include <core/dbus/object.h>
#include <core/dbus/property.h>
#include <core/dbus/types/variant.h>
#include <core/dbus/types/object_path.h>

#include <core/dbus/types/stl/vector.h>
#include <core/dbus/types/stl/map.h>

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
                inline static std::string name() { return "GetManagedObjects"; }
                using Interface = ObjectManagerInterface;
                inline static const std::chrono::milliseconds default_timeout() { return std::chrono::seconds(1); }
            };
            struct Signals {
                struct InterfacesAdded {
                    inline static std::string name()
                    {
                        return "InterfacesAdded";
                    };
                    using Interface = ObjectManagerInterface;
                    using ArgumentType = tuple<dbus::types::ObjectPath, std::map<std::string, std::map<std::string, dbus::types::Variant>>>;

                    using Signal = std::shared_ptr<dbus::Signal<InterfacesAdded, ArgumentType >>;
                };
            };
        public:
            virtual ~ObjectManagerInterface() noexcept = default;
        };

#if 0
        //
        class ObjectManagerStub : public dbus::Stub<ObjectManagerInterface> {
        public:
            using Ptr = shared_ptr<ObjectManagerStub>;

            ObjectManagerStub(const dbus::Bus::Ptr &bus)
            : dbus::Stub<ObjectManagerInterface>(bus)
            {

            }

            dbus::Service::Ptr getService() const { return access_service(); }
        };
#endif

        class ObjectManagerSkeleton {
        public:

            ObjectManagerSkeleton(const dbus::Bus::Ptr &bus, const dbus::Service::Ptr &service)
                    : mBus(bus), mService(service)
            {
                mObject = mService->add_object_for_path(dbus::types::ObjectPath{"/"});
                mObject->install_method_handler<ObjectManagerInterface::GetManagedObjects>(
                        std::bind(&ObjectManagerSkeleton::handleGetManagedObjects, this, std::placeholders::_1));
            }

            /*
            void init( const dbus::Object::Ptr &object) {
                mObject = object;
                mObject->install_method_handler<ObjectManagerInterface::GetManagedObjects>(
                        std::bind(&ObjectManagerSkeleton::handleGetManagedObjects, this, std::placeholders::_1));
            }*/

            ~ObjectManagerSkeleton() noexcept = default;

            // DICT<OBJPATH,DICT<STRING,DICT<STRING,VARIANT>>>
            // DICT<OBJPATH,DICT<STRING,VariantDict>>   VariantDict => DICT<STRING,VARIANT>
            // DICT<OBJPATH,ObjectDict>   ObjectDict => DICT<STRING,VariantDict>
            using Variant = types::Variant;
            using VariantDict = map<string, types::Variant>;
            using ObjectDict = map<string, VariantDict>;
            using ObjectDetails = map<types::ObjectPath, ObjectDict>;

            using ObjectManaged = std::function<ObjectDetails()>;

            void addObjectManaged(ObjectManaged man) {
                mObjects.emplace_back(man);
            }
        protected:
            dbus::Bus::Ptr mBus;
            dbus::Service::Ptr mService;
            dbus::Object::Ptr mObject;

            void handleGetManagedObjects(const dbus::Message::Ptr &msg) {
                auto reply = dbus::Message::make_method_return(msg);
                reply->writer() << getManagedObjects();
                mBus->send(reply);
            };

            std::list<ObjectManaged> mObjects;

            virtual ObjectDetails getManagedObjects() {
                ObjectDetails d;
                for (auto func : mObjects) {
                    ObjectDetails dt = func();
                    for (auto dtl : dt)
                        d.insert(dtl);
                }

                return d;
            }
        public:
        };

    }
}

class ObjectManagerHelper {
public:

protected:
    template <typename X>
    using Prop = std::shared_ptr<core::dbus::Property<X>>;

    template <typename X>
    std::pair<std::string, core::dbus::types::Variant> getProperty(Prop<X> prop) const {
        return make_pair(X::name(), core::dbus::types::Variant::encode(prop->get()));
    };
};

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
