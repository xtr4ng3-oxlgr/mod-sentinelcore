SELECT
  id,
  event_type,
  severity,
  account_id,
  player_guid,
  player_name,
  category,
  action,
  detail,
  map_id,
  position_x,
  position_y,
  position_z,
  remote_address,
  created_at
FROM sentinelcore_events
ORDER BY id DESC
LIMIT 5000;
