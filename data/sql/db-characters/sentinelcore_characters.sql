-- mod-sentinelcore
-- Apply to acore_characters.

CREATE TABLE IF NOT EXISTS `sentinelcore_events` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `event_type` VARCHAR(32) NOT NULL,
  `severity` VARCHAR(32) NOT NULL,
  `account_id` INT UNSIGNED NOT NULL DEFAULT 0,
  `player_guid` BIGINT UNSIGNED NOT NULL DEFAULT 0,
  `player_name` VARCHAR(64) NOT NULL DEFAULT '',
  `category` VARCHAR(64) NOT NULL DEFAULT '',
  `action` VARCHAR(64) NOT NULL DEFAULT '',
  `detail` TEXT NULL,
  `map_id` INT UNSIGNED NOT NULL DEFAULT 0,
  `position_x` FLOAT NOT NULL DEFAULT 0,
  `position_y` FLOAT NOT NULL DEFAULT 0,
  `position_z` FLOAT NOT NULL DEFAULT 0,
  `remote_address` VARCHAR(64) NOT NULL DEFAULT '',
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idx_sentinelcore_created` (`created_at`),
  KEY `idx_sentinelcore_type` (`event_type`),
  KEY `idx_sentinelcore_severity` (`severity`),
  KEY `idx_sentinelcore_account` (`account_id`),
  KEY `idx_sentinelcore_player` (`player_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `sentinelcore_metrics` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `metric_name` VARCHAR(64) NOT NULL,
  `metric_value` BIGINT NOT NULL DEFAULT 0,
  `detail` VARCHAR(255) NOT NULL DEFAULT '',
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idx_sentinelcore_metric_name` (`metric_name`),
  KEY `idx_sentinelcore_metric_created` (`created_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
