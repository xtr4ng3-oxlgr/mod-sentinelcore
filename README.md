# mod-sentinelcore

<img width="1672" height="941" alt="sentinel" src="https://github.com/user-attachments/assets/d963c84a-f2aa-4e6a-8368-152e3f3c4546" />


**SentinelCore** is an observability and audit module for AzerothCore WotLK servers.

It helps administrators understand player sessions, GM activity, movement signals, economy thresholds and server metrics through structured logs, database tables and defensive reports.

Created by **xtr4ng3**.

---

## Purpose

A private server needs visibility. Not noise, not automatic punishment, not hidden control.

SentinelCore is designed as a defensive observability layer for AzerothCore-based servers. It records relevant operational events and makes them easier to review, export and document.

---

## Features

- player login/logout audit,
- GM chat command audit,
- movement delta observation,
- economy threshold observation,
- server metrics collection,
- structured database logging,
- optional file logging,
- configurable feature switches,
- SQL schema,
- Python report builder,
- static dashboard,
- documentation.

---

## Architecture

```text
AzerothCore hooks
  -> SentinelCore C++ module
  -> structured event model
  -> database / file logs
  -> report builder
  -> static dashboard
```

Languages:

- C++ for the AzerothCore module,
- SQL for storage,
- Python for reports,
- HTML/CSS/JS for dashboard.

---

## Installation

Clone into the AzerothCore modules directory:

```bash
cd /home/azeroth/azerothcore-wotlk/modules
git clone https://github.com/xtr4ng3-oxlgr/mod-sentinelcore.git
```

Build AzerothCore:

```bash
cd /home/azeroth/azerothcore-wotlk
./acore.sh compiler build
```

Copy config:

```bash
cp modules/mod-sentinelcore/conf/mod_sentinelcore.conf.dist env/dist/etc/modules/mod_sentinelcore.conf
```

Apply SQL to `acore_characters`:

```sql
SOURCE modules/mod-sentinelcore/data/sql/db-characters/sentinelcore_characters.sql;
```

Restart worldserver.

---

## Configuration

```ini
SentinelCore.Enable = 1
SentinelCore.LogToDatabase = 1
SentinelCore.LogToFile = 1

SentinelCore.TrackLogin = 1
SentinelCore.TrackMovement = 1
SentinelCore.TrackEconomy = 1
SentinelCore.TrackMetrics = 1
SentinelCore.TrackGM = 1
```

See:

```text
docs/CONFIGURATION.md
```

---

## Reports

Export your event tables to CSV and run:

```bash
python tools/report_builder.py --events sentinelcore_events.csv --metrics sentinelcore_metrics.csv --out reports
```

Open the generated HTML report or use:

```text
dashboard/index.html
```

to load a generated JSON report.

---

## Safety

SentinelCore does not ban players, delete data, give items, grant permissions or execute external programs from the game server.

It records observations for administrators to review.

---

## Status

This is a v1.0 foundation release.  
Hook signatures can vary between AzerothCore revisions. If your core has custom API differences, adjust the script hook signatures in `src/SentinelCore.cpp`.

---
## Licencia

<img width="384" height="384" alt="giphy (4)" src="https://github.com/user-attachments/assets/18d64b9e-fba8-493e-8462-f6722e0e64b7" />

**xtr4ng3**

MIT.
