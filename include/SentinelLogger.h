#ifndef MOD_SENTINELCORE_LOGGER_H
#define MOD_SENTINELCORE_LOGGER_H

#include "SentinelTypes.h"
#include "SentinelConfig.h"

namespace SentinelCore
{
    class Logger
    {
    public:
        static void Write(AuditEvent const& event);
        static char const* SeverityName(Severity level);
        static char const* EventTypeName(EventType type);
    };
}

#endif // MOD_SENTINELCORE_LOGGER_H
