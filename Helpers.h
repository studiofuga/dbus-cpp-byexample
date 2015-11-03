//
// Created by <fuga@studiofuga.com> on 02/11/15.
//

#ifndef DBTEST_HELPERS_H
#define DBTEST_HELPERS_H

#include <string>
#include <core/dbus/types/object_path.h>
#include <chrono>

#define SERVICE_INTERFACE(serviceName, objectPath) \
    static inline std::string ServiceName() { return serviceName; } \
    static inline const core::dbus::types::ObjectPath ObjectPath() { return core::dbus::types::ObjectPath{objectPath}; }

#define METHOD_SIGNATURE(methodName, InterfaceClass) \
    inline static std::string name() { return methodName; } \
    using Interface = InterfaceClass; \
    inline static const std::chrono::milliseconds default_timeout() { return std::chrono::seconds(1); }

#define PROPERTY_RO(propertyName, InterfaceClass, _ValueType) \
    inline static string name() { return propertyName; } \
    using Interface = InterfaceClass; \
    using ValueType = _ValueType; \
    static const bool readable = true; static const bool writable = false;

#define PROPERTY_RW(propertyName, InterfaceClass, _ValueType) \
    struct propertyName { \
        inline static string name() { return "##propertyName"; } \
        using Interface = InterfaceClass; \
        using ValueType = _ValueType; \
        static const bool readable = true; static const bool writable = true; \
    }


#endif //DBTEST_HELPERS_H
