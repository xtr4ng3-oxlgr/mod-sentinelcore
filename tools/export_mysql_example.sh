#!/usr/bin/env bash
set -e

# Example only. Edit credentials/database before use.

MYSQL_USER="acore"
MYSQL_PASS="password"
MYSQL_DB="acore_characters"

mysql -u"$MYSQL_USER" -p"$MYSQL_PASS" "$MYSQL_DB" -e "
SELECT * FROM sentinelcore_events
ORDER BY id DESC
LIMIT 5000;
" --batch --raw > sentinelcore_events.tsv

mysql -u"$MYSQL_USER" -p"$MYSQL_PASS" "$MYSQL_DB" -e "
SELECT * FROM sentinelcore_metrics
ORDER BY id DESC
LIMIT 5000;
" --batch --raw > sentinelcore_metrics.tsv

echo "Export completed."
