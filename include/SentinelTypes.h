#ifndef MOD_SENTINELCORE_TYPES_H
#define MOD_SENTINELCORE_TYPES_H

#include "Define.h"
#include <string>

namespace SentinelCore
{
    enum class EventType : uint8
    {
        Login       = 1,
        Logout      = 2,
        Movement    = 3,
        Economy     = 4,
        GM          = 5,
        Metrics     = 6,
        Alert       = 7,
        System      = 8
    };

    enum class Severity : uint8
    {
        Info     = 1,
        Low      = 2,
        Medium   = 3,
        High     = 4,
        Critical = 5
    };

    struct AuditEvent
    {
        EventType Type;
        Severity Level;
        uint32 AccountId;
        uint64 PlayerGuid;
        std::string PlayerName;
        std::string Category;
        std::string Action;
        std::string Detail;
        uint32 MapId;
        float X;
        float Y;
        float Z;
        std::string RemoteAddress;
    };
}

#endif // MOD_SENTINELCORE_TYPES_H
