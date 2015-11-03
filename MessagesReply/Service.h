//
// Created by <fuga@studiofuga.com> on 02/11/15.
//

#ifndef DBTEST_SERVICE_H
#define DBTEST_SERVICE_H
#include <string>
#include <array>
using namespace std;

#include <Helpers.h>


#include <core/dbus/service.h>

namespace bluez {
    struct Introspection {
        SERVICE_INTERFACE("org.freedesktop.DBus.Introspectable", "/com/studiofuga/test/dbus");
    };

    struct Service {
        SERVICE_INTERFACE("com.studiofuga.test.dbus", "/com/studiofuga/test/dbus");

        struct TestVec {
            METHOD_SIGNATURE("TestVec", Service);
        };
        struct TestDict {
            METHOD_SIGNATURE("TestDic", Service);
        };
        struct TestDV {
            METHOD_SIGNATURE("TestDV", Service);
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
                using Interface = Service;
                using ValueType = string;
                static const bool readable = true;
                static const bool writable = false;
            };
            struct Primary {
                inline static string name()
                {
                    return "Primary";
                };
                using Interface = Service;
                using ValueType = bool;
                static const bool readable = true;
                static const bool writable = false;
            };
            struct Characteristics {
                inline static string name()
                {
                    return "Characteristics";
                };
                using Interface = Service;
                using ValueType = vector<string>;
                static const bool readable = true;
                static const bool writable = false;
            };
        };
    };

}

namespace core {
    namespace dbus {
        namespace traits {
            template<>
            struct Service<bluez::Service> {
                inline static const std::string interface_name() {
                    return std::string{"com.studiofuga.test.dbus"};
                }
            };

            template<>
            struct Service<bluez::Introspection>
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

#endif //DBTEST_SERVICE_H
