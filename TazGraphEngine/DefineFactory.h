#pragma once
#include <cstring>

// Macro to extract just the enum name
#define ENUM_NAME(name) name,

// Macro to extract the string representation
#define ENUM_STRING(name) #name,

// Macro to create switch case for enum to string
#define ENUM_TO_STRING_CASE(name) case name: return #name;

// Macro to create string comparison for string to enum
#define STRING_TO_ENUM_COMPARE(name) if (strcmp(str, #name) == 0) return name;

/// Declare enum and conversion functions in header
#define DECLARE_ENUM(EnumType, ENUM_LIST) \
    enum EnumType { \
        ENUM_LIST(ENUM_NAME) \
    }; \
    const char* EnumType##_ToString(EnumType value); \
    EnumType EnumType##_FromString(const char* str);

/// Define conversion functions in source file
#define DEFINE_ENUM(EnumType, ENUM_LIST) \
    const char* EnumType##_ToString(EnumType value) { \
        switch(value) { \
            ENUM_LIST(ENUM_TO_STRING_CASE) \
            default: return "UNKNOWN"; \
        } \
    } \
    EnumType EnumType##_FromString(const char* str) { \
        if (str == nullptr) return (EnumType)0; \
        ENUM_LIST(STRING_TO_ENUM_COMPARE) \
        return (EnumType)0; \
    }

// Example usage:
#define NODE_PORTS(XX) \
    XX(TOP) \
    XX(RIGHT) \
    XX(BOTTOM) \
    XX(LEFT)

#define LINK_CHILDREN(XX) \
    XX(NONE) \
    XX(ARROWHEAD)

// In header file (.h):
DECLARE_ENUM(NodePorts, NODE_PORTS)
DECLARE_ENUM(LinkChildren, LINK_CHILDREN)