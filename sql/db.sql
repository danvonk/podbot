-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Server version:               10.1.10-MariaDB-log - MariaDB Server
-- Server OS:                    Linux
-- HeidiSQL Version:             9.3.0.4999
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- Dumping structure for table dev.episodes
CREATE TABLE IF NOT EXISTS `episodes` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `podcast_id` int(11) NOT NULL,
  `title` text NOT NULL,
  `link` text NOT NULL,
  `pubdate` datetime DEFAULT NULL,
  `description` text,
  `itunes_subtitle` text,
  `itunes_summary` text,
  `itunes_author` text,
  `itunes_duration` bigint(20) unsigned DEFAULT NULL,
  `itunes_explicit` varchar(10) DEFAULT NULL,
  `enclosure_url` text,
  `enclosure_length` varchar(255) DEFAULT NULL,
  `enclosure_type` varchar(255) DEFAULT NULL,
  `guid` text,
  `itunes_image` text,
  `created_at` datetime NOT NULL,
  `updated_at` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `FK_episodes_podcasts` (`podcast_id`),
  CONSTRAINT `FK_episodes_podcasts` FOREIGN KEY (`podcast_id`) REFERENCES `podcasts` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Contains <item> information from every RSS feed.';

-- Data exporting was unselected.


-- Dumping structure for table dev.episodes_playlists
CREATE TABLE IF NOT EXISTS `episodes_playlists` (
  `playlist_id` int(11) DEFAULT NULL,
  `episode_id` int(11) DEFAULT NULL,
  KEY `index_episodes_playlists_on_playlist_id` (`playlist_id`),
  KEY `index_episodes_playlists_on_episode_id` (`episode_id`),
  CONSTRAINT `FK_episodes_playlists_episodes` FOREIGN KEY (`episode_id`) REFERENCES `episodes` (`id`),
  CONSTRAINT `FK_episodes_playlists_playlists` FOREIGN KEY (`playlist_id`) REFERENCES `playlists` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Data exporting was unselected.


-- Dumping structure for table dev.indexed_feeds
CREATE TABLE IF NOT EXISTS `indexed_feeds` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `podcast_id` int(11) NOT NULL,
  `feed_url` text NOT NULL,
  `created_at` datetime DEFAULT NULL,
  `updated_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `FK__podcasts` (`podcast_id`),
  CONSTRAINT `FK__podcasts` FOREIGN KEY (`podcast_id`) REFERENCES `podcasts` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Used to check if an already indexed feeds has any new episodes.';

-- Data exporting was unselected.


-- Dumping structure for table dev.playlists
CREATE TABLE IF NOT EXISTS `playlists` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` text NOT NULL,
  `description` text,
  `language` varchar(255) DEFAULT NULL,
  `category` text,
  `author` varchar(255) DEFAULT NULL,
  `explicit` tinyint(1) DEFAULT NULL,
  `copyright` varchar(255) DEFAULT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` datetime NOT NULL,
  `user_id` int(11) DEFAULT NULL,
  `image_file_name` varchar(255) DEFAULT NULL,
  `image_content_type` varchar(255) DEFAULT NULL,
  `image_file_size` int(11) DEFAULT NULL,
  `image_updated_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `index_playlists_on_user_id` (`user_id`),
  CONSTRAINT `fk_rails_e8888dd21e` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Data exporting was unselected.


-- Dumping structure for table dev.podcasts
CREATE TABLE IF NOT EXISTS `podcasts` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `title` text NOT NULL,
  `link` text NOT NULL,
  `description` text,
  `language` varchar(255) DEFAULT NULL,
  `category` varchar(255) DEFAULT NULL,
  `itunes_author` varchar(255) DEFAULT NULL,
  `itunes_explicit` varchar(255) DEFAULT NULL,
  `copyright` varchar(255) DEFAULT NULL,
  `itunes_image` text,
  `created_at` datetime DEFAULT NULL,
  `updated_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Contains RSS information about every podcast feed.';

-- Data exporting was unselected.


-- Dumping structure for table dev.schema_migrations
CREATE TABLE IF NOT EXISTS `schema_migrations` (
  `version` varchar(255) NOT NULL,
  UNIQUE KEY `unique_schema_migrations` (`version`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Data exporting was unselected.


-- Dumping structure for table dev.test
CREATE TABLE IF NOT EXISTS `test` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `times` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Data exporting was unselected.


-- Dumping structure for table dev.users
CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `email` varchar(255) NOT NULL DEFAULT '',
  `encrypted_password` varchar(255) NOT NULL DEFAULT '',
  `reset_password_token` varchar(255) DEFAULT NULL,
  `reset_password_sent_at` datetime DEFAULT NULL,
  `remember_created_at` datetime DEFAULT NULL,
  `sign_in_count` int(11) NOT NULL DEFAULT '0',
  `current_sign_in_at` datetime DEFAULT NULL,
  `last_sign_in_at` datetime DEFAULT NULL,
  `current_sign_in_ip` varchar(255) DEFAULT NULL,
  `last_sign_in_ip` varchar(255) DEFAULT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` datetime NOT NULL,
  `avatar_file_name` varchar(255) DEFAULT NULL,
  `avatar_content_type` varchar(255) DEFAULT NULL,
  `avatar_file_size` int(11) DEFAULT NULL,
  `avatar_updated_at` datetime DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `index_users_on_email` (`email`),
  UNIQUE KEY `index_users_on_reset_password_token` (`reset_password_token`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Data exporting was unselected.
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
