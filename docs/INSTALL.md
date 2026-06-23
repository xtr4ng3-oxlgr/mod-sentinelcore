# Installation

Clone into the AzerothCore modules directory:

```bash
cd /home/azeroth/azerothcore-wotlk/modules
git clone https://github.com/YOUR-USER/mod-sentinelcore.git
```

Rebuild:

```bash
cd /home/azeroth/azerothcore-wotlk
./acore.sh compiler build
```

Copy config:

```bash
cp modules/mod-sentinelcore/conf/mod_sentinelcore.conf.dist env/dist/etc/modules/mod_sentinelcore.conf
```

Apply SQL to characters database:

```sql
SOURCE modules/mod-sentinelcore/data/sql/db-characters/sentinelcore_characters.sql;
```

Restart worldserver.
