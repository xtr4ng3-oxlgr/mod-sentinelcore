#ifndef MOD_SENTINELCORE_CONFIG_H
#define MOD_SENTINELCORE_CONFIG_H

#include "Config.h"
#include "Define.h"

namespace SentinelCore
{
    struct ConfigState
    {
        bool Enable;
        bool Announce;
        bool LogToDatabase;
        bool LogToFile;

        bool TrackLogin;
        bool TrackMovement;
        bool TrackEconomy;
        bool TrackMetrics;
        bool TrackGM;

        uint32 MovementCheckIntervalMs;
        float MovementDistanceThreshold;
        uint32 MetricsIntervalSeconds;
        uint32 EconomyGoldThresholdCopper;

        static ConfigState Load()
        {
            ConfigState cfg;

            cfg.Enable = sConfigMgr->GetOption<bool>("SentinelCore.Enable", true);
            cfg.Announce = sConfigMgr->GetOption<bool>("SentinelCore.Announce", true);
            cfg.LogToDatabase = sConfigMgr->GetOption<bool>("SentinelCore.LogToDatabase", true);
            cfg.LogToFile = sConfigMgr->GetOption<bool>("SentinelCore.LogToFile", true);

            cfg.TrackLogin = sConfigMgr->GetOption<bool>("SentinelCore.TrackLogin", true);
            cfg.TrackMovement = sConfigMgr->GetOption<bool>("SentinelCore.TrackMovement", true);
            cfg.TrackEconomy = sConfigMgr->GetOption<bool>("SentinelCore.TrackEconomy", true);
            cfg.TrackMetrics = sConfigMgr->GetOption<bool>("SentinelCore.TrackMetrics", true);
            cfg.TrackGM = sConfigMgr->GetOption<bool>("SentinelCore.TrackGM", true);

            cfg.MovementCheckIntervalMs = sConfigMgr->GetOption<uint32>("SentinelCore.MovementCheckIntervalMs", 5000);
            cfg.MovementDistanceThreshold = sConfigMgr->GetOption<float>("SentinelCore.MovementDistanceThreshold", 180.0f);
            cfg.MetricsIntervalSeconds = sConfigMgr->GetOption<uint32>("SentinelCore.MetricsIntervalSeconds", 300);
            cfg.EconomyGoldThresholdCopper = sConfigMgr->GetOption<uint32>("SentinelCore.EconomyGoldThresholdCopper", 10000000);

            return cfg;
        }
    };
}

#endif // MOD_SENTINELCORE_CONFIG_H
