/*
 * mod-sentinelcore
 *
 * AzerothCore WotLK observability and audit module.
 *
 * Created by xtr4ng3.
 *
 * This module is designed as a defensive/admin observability layer.
 * It records structured server events and exports useful data for reports.
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldSession.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Config.h"
#include "GameTime.h"
#include "Define.h"
#include "ObjectGuid.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "../include/SentinelTypes.h"
#include "../include/SentinelConfig.h"
#include "../include/SentinelLogger.h"

namespace SentinelCore
{
    static ConfigState g_Config;
    static uint32 g_MetricsTimer = 0;

    struct PositionSnapshot
    {
        uint32 MapId = 0;
        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;
        uint32 Timer = 0;
        bool Ready = false;
    };

    static std::unordered_map<uint64, PositionSnapshot> g_Positions;

    static uint64 GuidToUInt64(ObjectGuid const& guid)
    {
        return guid.GetRawValue();
    }

    static std::string SafeString(std::string value)
    {
        for (char& c : value)
        {
            if (c == '\'' || c == '"' || c == '\n' || c == '\r' || c == '\t')
                c = ' ';
        }
        return value;
    }

    static std::string GetRemoteAddress(Player* player)
    {
        if (!player || !player->GetSession())
            return "";

        return player->GetSession()->GetRemoteAddress();
    }

    static AuditEvent MakePlayerEvent(Player* player, EventType type, Severity level, std::string const& category, std::string const& action, std::string const& detail)
    {
        AuditEvent event;
        event.Type = type;
        event.Level = level;
        event.AccountId = player && player->GetSession() ? player->GetSession()->GetAccountId() : 0;
        event.PlayerGuid = player ? GuidToUInt64(player->GetGUID()) : 0;
        event.PlayerName = player ? player->GetName() : "";
        event.Category = category;
        event.Action = action;
        event.Detail = detail;
        event.MapId = player ? player->GetMapId() : 0;
        event.X = player ? player->GetPositionX() : 0.0f;
        event.Y = player ? player->GetPositionY() : 0.0f;
        event.Z = player ? player->GetPositionZ() : 0.0f;
        event.RemoteAddress = GetRemoteAddress(player);
        return event;
    }

    char const* Logger::SeverityName(Severity level)
    {
        switch (level)
        {
            case Severity::Info: return "info";
            case Severity::Low: return "low";
            case Severity::Medium: return "medium";
            case Severity::High: return "high";
            case Severity::Critical: return "critical";
            default: return "unknown";
        }
    }

    char const* Logger::EventTypeName(EventType type)
    {
        switch (type)
        {
            case EventType::Login: return "login";
            case EventType::Logout: return "logout";
            case EventType::Movement: return "movement";
            case EventType::Economy: return "economy";
            case EventType::GM: return "gm";
            case EventType::Metrics: return "metrics";
            case EventType::Alert: return "alert";
            case EventType::System: return "system";
            default: return "unknown";
        }
    }

    void Logger::Write(AuditEvent const& event)
    {
        if (!g_Config.Enable)
            return;

        std::string category = SafeString(event.Category);
        std::string action = SafeString(event.Action);
        std::string detail = SafeString(event.Detail);
        std::string player = SafeString(event.PlayerName);
        std::string remote = SafeString(event.RemoteAddress);

        if (g_Config.LogToFile)
        {
            std::ofstream file("sentinelcore.log", std::ios::app);
            if (file.is_open())
            {
                file
                    << "[" << Logger::SeverityName(event.Level) << "]"
                    << " type=" << Logger::EventTypeName(event.Type)
                    << " account=" << event.AccountId
                    << " guid=" << event.PlayerGuid
                    << " player=\"" << player << "\""
                    << " category=\"" << category << "\""
                    << " action=\"" << action << "\""
                    << " map=" << event.MapId
                    << " pos=" << event.X << "," << event.Y << "," << event.Z
                    << " ip=\"" << remote << "\""
                    << " detail=\"" << detail << "\""
                    << std::endl;
            }
        }

        if (g_Config.LogToDatabase)
        {
            CharacterDatabase.Execute(
                "INSERT INTO sentinelcore_events "
                "(event_type, severity, account_id, player_guid, player_name, category, action, detail, map_id, position_x, position_y, position_z, remote_address, created_at) "
                "VALUES ('{}', '{}', {}, {}, '{}', '{}', '{}', '{}', {}, {}, {}, {}, '{}', NOW())",
                Logger::EventTypeName(event.Type),
                Logger::SeverityName(event.Level),
                event.AccountId,
                event.PlayerGuid,
                player,
                category,
                action,
                detail,
                event.MapId,
                event.X,
                event.Y,
                event.Z,
                remote
            );
        }

        LOG_INFO("module", "[SentinelCore] {} {} {} {}", Logger::SeverityName(event.Level), Logger::EventTypeName(event.Type), category, action);
    }

    static float Distance2D(float x1, float y1, float x2, float y2)
    {
        float dx = x1 - x2;
        float dy = y1 - y2;
        return std::sqrt(dx * dx + dy * dy);
    }

    static void TrackMovement(Player* player, uint32 diff)
    {
        if (!player || !g_Config.TrackMovement)
            return;

        if (player->IsGameMaster())
            return;

        uint64 guid = GuidToUInt64(player->GetGUID());
        PositionSnapshot& snap = g_Positions[guid];

        if (snap.Timer > diff)
        {
            snap.Timer -= diff;
            return;
        }

        snap.Timer = g_Config.MovementCheckIntervalMs;

        if (!snap.Ready)
        {
            snap.MapId = player->GetMapId();
            snap.X = player->GetPositionX();
            snap.Y = player->GetPositionY();
            snap.Z = player->GetPositionZ();
            snap.Ready = true;
            return;
        }

        uint32 currentMap = player->GetMapId();
        float x = player->GetPositionX();
        float y = player->GetPositionY();
        float z = player->GetPositionZ();

        if (currentMap == snap.MapId)
        {
            float dist = Distance2D(x, y, snap.X, snap.Y);
            if (dist >= g_Config.MovementDistanceThreshold && !player->IsBeingTeleported())
            {
                std::ostringstream ss;
                ss << "large movement delta=" << dist
                   << " from=" << snap.X << "," << snap.Y << "," << snap.Z
                   << " to=" << x << "," << y << "," << z;

                Logger::Write(MakePlayerEvent(player, EventType::Movement, Severity::Medium, "movement", "large_delta", ss.str()));
            }
        }

        snap.MapId = currentMap;
        snap.X = x;
        snap.Y = y;
        snap.Z = z;
    }

    static void WriteMetric(std::string const& name, uint64 value, std::string const& detail)
    {
        if (!g_Config.Enable || !g_Config.TrackMetrics)
            return;

        if (g_Config.LogToDatabase)
        {
            CharacterDatabase.Execute(
                "INSERT INTO sentinelcore_metrics (metric_name, metric_value, detail, created_at) VALUES ('{}', {}, '{}', NOW())",
                SafeString(name),
                value,
                SafeString(detail)
            );
        }

        if (g_Config.LogToFile)
        {
            std::ofstream file("sentinelcore_metrics.log", std::ios::app);
            if (file.is_open())
                file << "metric=" << name << " value=" << value << " detail=\"" << detail << "\"" << std::endl;
        }
    }

    static void CollectMetrics()
    {
        uint32 online = sWorld->GetPlayerCount();
        uint32 maxOnline = sWorld->GetMaxPlayerCount();

        WriteMetric("players_online", online, "current online players");
        WriteMetric("players_max", maxOnline, "max online players");
        WriteMetric("uptime", sWorld->GetUptime(), "world uptime seconds");
    }
}

class SentinelCoreWorldScript : public WorldScript
{
public:
    SentinelCoreWorldScript() : WorldScript("SentinelCoreWorldScript") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        SentinelCore::g_Config = SentinelCore::ConfigState::Load();

        if (SentinelCore::g_Config.Enable)
            LOG_INFO("module", "[SentinelCore] configuration loaded");
    }

    void OnUpdate(uint32 diff) override
    {
        if (!SentinelCore::g_Config.Enable || !SentinelCore::g_Config.TrackMetrics)
            return;

        if (SentinelCore::g_MetricsTimer > diff)
        {
            SentinelCore::g_MetricsTimer -= diff;
            return;
        }

        SentinelCore::g_MetricsTimer = SentinelCore::g_Config.MetricsIntervalSeconds * 1000;
        SentinelCore::CollectMetrics();
    }
};

class SentinelCorePlayerScript : public PlayerScript
{
public:
    SentinelCorePlayerScript() : PlayerScript("SentinelCorePlayerScript") { }

    void OnLogin(Player* player) override
    {
        if (!SentinelCore::g_Config.Enable || !player)
            return;

        if (SentinelCore::g_Config.Announce && player->GetSession())
            ChatHandler(player->GetSession()).SendSysMessage("|cff7ef9ff[SentinelCore]|r observability module active.");

        if (SentinelCore::g_Config.TrackLogin)
            SentinelCore::Logger::Write(SentinelCore::MakePlayerEvent(player, SentinelCore::EventType::Login, SentinelCore::Severity::Info, "session", "login", "player login"));
    }

    void OnLogout(Player* player) override
    {
        if (!SentinelCore::g_Config.Enable || !player)
            return;

        if (SentinelCore::g_Config.TrackLogin)
            SentinelCore::Logger::Write(SentinelCore::MakePlayerEvent(player, SentinelCore::EventType::Logout, SentinelCore::Severity::Info, "session", "logout", "player logout"));

        SentinelCore::g_Positions.erase(SentinelCore::GuidToUInt64(player->GetGUID()));
    }

    void OnUpdate(Player* player, uint32 diff) override
    {
        if (!SentinelCore::g_Config.Enable)
            return;

        SentinelCore::TrackMovement(player, diff);
    }

    void OnMoneyChanged(Player* player, int32& amount) override
    {
        if (!SentinelCore::g_Config.Enable || !SentinelCore::g_Config.TrackEconomy || !player)
            return;

        uint32 absAmount = amount < 0 ? uint32(-amount) : uint32(amount);
        if (absAmount < SentinelCore::g_Config.EconomyGoldThresholdCopper)
            return;

        std::ostringstream ss;
        ss << "money_changed amount=" << amount << " copper";

        SentinelCore::Logger::Write(SentinelCore::MakePlayerEvent(player, SentinelCore::EventType::Economy, SentinelCore::Severity::Medium, "economy", "money_threshold", ss.str()));
    }

    void OnChat(Player* player, uint32 /*type*/, uint32 /*lang*/, std::string& msg) override
    {
        if (!SentinelCore::g_Config.Enable || !SentinelCore::g_Config.TrackGM || !player)
            return;

        if (!player->IsGameMaster())
            return;

        if (msg.empty() || msg[0] != '.')
            return;

        std::string command = msg.substr(0, 96);
        SentinelCore::Logger::Write(SentinelCore::MakePlayerEvent(player, SentinelCore::EventType::GM, SentinelCore::Severity::Info, "gm", "command_chat", command));
    }
};

void AddSC_mod_sentinelcore()
{
    new SentinelCoreWorldScript();
    new SentinelCorePlayerScript();
}
