SET FOREIGN_KEY_CHECKS=0;

--
-- Struktura tabulky `account`
--

CREATE TABLE `account` (
  `id` int(10) UNSIGNED NOT NULL,
  `inserted` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `state` enum('PENDING','REJECTED','ACTIVE','DEACTIVATED') COLLATE utf8_czech_ci NOT NULL DEFAULT 'PENDING',
  `type` int(10) UNSIGNED NOT NULL,
  `number` varchar(17) COLLATE utf8_czech_ci DEFAULT NULL,
  `funds` decimal(15,2) NOT NULL DEFAULT '0.00',
  `owner` int(10) UNSIGNED NOT NULL COMMENT 'ib_account',
  `verified_by` int(10) UNSIGNED DEFAULT NULL,
  `verified_when` datetime DEFAULT NULL,
  `verified_comment` text COLLATE utf8_czech_ci
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

-- --------------------------------------------------------

--
-- Struktura tabulky `account_type`
--

CREATE TABLE `account_type` (
  `id` int(10) UNSIGNED NOT NULL,
  `name` varchar(50) COLLATE utf8_czech_ci NOT NULL,
  `offered` tinyint(1) NOT NULL DEFAULT '1',
  `monthly_fee` decimal(15,2) NOT NULL DEFAULT '0.00',
  `interest` decimal(15,2) NOT NULL DEFAULT '0.00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

-- --------------------------------------------------------

--
-- Struktura tabulky `card`
--

CREATE TABLE `card` (
  `id` int(10) UNSIGNED NOT NULL,
  `inserted` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `state` enum('PENDING','REJECTED','MAKING','ACTIVE','DEACTIVATED') COLLATE utf8_czech_ci DEFAULT 'PENDING',
  `number` varchar(19) COLLATE utf8_czech_ci DEFAULT NULL,
  `pin` varchar(255) COLLATE utf8_czech_ci DEFAULT NULL COMMENT 'password_hash',
  `disponent` int(10) UNSIGNED NOT NULL,
  `expires` datetime DEFAULT NULL,
  `verified_by1` int(10) UNSIGNED DEFAULT NULL,
  `verified_when1` datetime DEFAULT NULL,
  `verified_comment1` text COLLATE utf8_czech_ci,
  `verified_by2` int(10) UNSIGNED DEFAULT NULL,
  `verified_when2` datetime DEFAULT NULL,
  `verified_comment2` text COLLATE utf8_czech_ci
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

-- --------------------------------------------------------

--
-- Struktura tabulky `disponent`
--

CREATE TABLE `disponent` (
  `id` int(10) UNSIGNED NOT NULL,
  `inserted` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `state` enum('PENDING','AGREED','REJECTED','ACTIVE','DEACTIVATED') COLLATE utf8_czech_ci NOT NULL DEFAULT 'PENDING',
  `account` int(10) UNSIGNED NOT NULL,
  `ib_account` int(10) UNSIGNED NOT NULL,
  `custom_name` varchar(50) COLLATE utf8_czech_ci DEFAULT NULL,
  `verified_by` int(10) UNSIGNED DEFAULT NULL,
  `verified_when` datetime DEFAULT NULL,
  `verified_comment` text COLLATE utf8_czech_ci
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

-- --------------------------------------------------------

--
-- Struktura tabulky `employee`
--

CREATE TABLE `employee` (
  `id` int(10) UNSIGNED NOT NULL,
  `person` int(10) UNSIGNED NOT NULL,
  `workplace` int(10) UNSIGNED NOT NULL,
  `ib_account` int(10) UNSIGNED NOT NULL,
  `start` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `end` datetime DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

-- --------------------------------------------------------

--
-- Struktura tabulky `ib_account`
--

CREATE TABLE `ib_account` (
  `id` int(10) UNSIGNED NOT NULL,
  `inserted` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `state` enum('PENDING','ACTIVE','DEACTIVATED') COLLATE utf8_czech_ci NOT NULL DEFAULT 'PENDING',
  `role` enum('CLIENT','EMPLOYEE','ADMIN') COLLATE utf8_czech_ci NOT NULL DEFAULT 'CLIENT',
  `activation_code` varchar(16) COLLATE utf8_czech_ci DEFAULT NULL,
  `reset_pw_code` varchar(16) COLLATE utf8_czech_ci DEFAULT NULL,
  `name` varchar(50) COLLATE utf8_czech_ci NOT NULL,
  `password` varchar(255) COLLATE utf8_czech_ci NOT NULL COMMENT 'password_hash',
  `person` int(10) UNSIGNED NOT NULL,
  `email` varchar(255) COLLATE utf8_czech_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

-- --------------------------------------------------------

--
-- Struktura tabulky `notification`
--

CREATE TABLE `notification` (
  `id` int(10) UNSIGNED NOT NULL,
  `account` int(10) UNSIGNED NOT NULL,
  `text` text COLLATE utf8_czech_ci,
  `seen` tinyint(1) NOT NULL DEFAULT '0',
  `created` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

-- --------------------------------------------------------

--
-- Struktura tabulky `payment`
--

CREATE TABLE `payment` (
  `id` int(10) UNSIGNED NOT NULL,
  `from_acc` varchar(17) COLLATE utf8_czech_ci NOT NULL,
  `from_bank_code` varchar(4) COLLATE utf8_czech_ci NOT NULL,
  `to_acc` varchar(17) COLLATE utf8_czech_ci NOT NULL,
  `to_bank_code` varchar(4) COLLATE utf8_czech_ci NOT NULL,
  `amount` decimal(15,2) NOT NULL,
  `created` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `creator_disponent_id` int(10) UNSIGNED DEFAULT NULL,
  `processed` datetime DEFAULT NULL,
  `variable_symbol` varchar(10) COLLATE utf8_czech_ci DEFAULT NULL,
  `constant_symbol` varchar(4) COLLATE utf8_czech_ci DEFAULT NULL,
  `specific_symbol` varchar(10) COLLATE utf8_czech_ci DEFAULT NULL,
  `message` text COLLATE utf8_czech_ci
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

-- --------------------------------------------------------

--
-- Struktura tabulky `person`
--

CREATE TABLE `person` (
  `id` int(10) UNSIGNED NOT NULL,
  `inserted` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `state` enum('PENDING','VERIFIED','INVALID') COLLATE utf8_czech_ci NOT NULL DEFAULT 'PENDING',
  `first_name` varchar(255) COLLATE utf8_czech_ci NOT NULL,
  `second_name` varchar(255) COLLATE utf8_czech_ci NOT NULL,
  `identification` varchar(255) COLLATE utf8_czech_ci NOT NULL COMMENT 'always prefix with ID_',
  `country_code` varchar(4) COLLATE utf8_czech_ci NOT NULL,
  `address` varchar(1024) COLLATE utf8_czech_ci NOT NULL,
  `birthdate` date NOT NULL,
  `verified_by` int(10) UNSIGNED DEFAULT NULL,
  `verified_when` datetime DEFAULT NULL,
  `verified_comment` text COLLATE utf8_czech_ci
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

-- --------------------------------------------------------

--
-- Struktura tabulky `place`
--

CREATE TABLE `place` (
  `id` int(10) UNSIGNED NOT NULL,
  `name` varchar(255) COLLATE utf8_czech_ci NOT NULL,
  `address` varchar(1024) COLLATE utf8_czech_ci NOT NULL,
  `opened` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `closed` datetime DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_czech_ci;

-- --------------------------------------------------------

--
-- Klíče pro tabulku `account`
--
ALTER TABLE `account`
  ADD PRIMARY KEY (`id`),
  ADD KEY `type` (`type`),
  ADD KEY `account_ibfk_1` (`owner`),
  ADD KEY `account_ibfk_3` (`verified_by`);

--
-- Klíče pro tabulku `account_type`
--
ALTER TABLE `account_type`
  ADD PRIMARY KEY (`id`);

--
-- Klíče pro tabulku `card`
--
ALTER TABLE `card`
  ADD PRIMARY KEY (`id`),
  ADD KEY `disponent` (`disponent`),
  ADD KEY `card_ibfk_3` (`verified_by2`),
  ADD KEY `card_ibfk_2` (`verified_by1`);

--
-- Klíče pro tabulku `disponent`
--
ALTER TABLE `disponent`
  ADD PRIMARY KEY (`id`),
  ADD KEY `account` (`account`),
  ADD KEY `disponent_ibfk_2` (`ib_account`),
  ADD KEY `disponent_ibfk_3` (`verified_by`);

--
-- Klíče pro tabulku `employee`
--
ALTER TABLE `employee`
  ADD PRIMARY KEY (`id`),
  ADD KEY `person` (`person`),
  ADD KEY `workplace` (`workplace`),
  ADD KEY `ib_account` (`ib_account`);

--
-- Klíče pro tabulku `ib_account`
--
ALTER TABLE `ib_account`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `name` (`name`),
  ADD UNIQUE KEY `email` (`email`),
  ADD KEY `person` (`person`);

--
-- Klíče pro tabulku `notification`
--
ALTER TABLE `notification`
  ADD PRIMARY KEY (`id`),
  ADD KEY `account` (`account`);

--
-- Klíče pro tabulku `payment`
--
ALTER TABLE `payment`
  ADD PRIMARY KEY (`id`),
  ADD KEY `creator_disponent_id` (`creator_disponent_id`);

--
-- Klíče pro tabulku `person`
--
ALTER TABLE `person`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `identification` (`identification`),
  ADD KEY `person_ibfk_1` (`verified_by`);

--
-- Klíče pro tabulku `place`
--
ALTER TABLE `place`
  ADD PRIMARY KEY (`id`);

-- --------------------------------------------------------

--
-- AUTO_INCREMENT pro tabulku `account`
--
ALTER TABLE `account`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=70;

--
-- AUTO_INCREMENT pro tabulku `account_type`
--
ALTER TABLE `account_type`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT pro tabulku `card`
--
ALTER TABLE `card`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT pro tabulku `disponent`
--
ALTER TABLE `disponent`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT pro tabulku `employee`
--
ALTER TABLE `employee`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT pro tabulku `ib_account`
--
ALTER TABLE `ib_account`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=19;

--
-- AUTO_INCREMENT pro tabulku `notification`
--
ALTER TABLE `notification`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=60;

--
-- AUTO_INCREMENT pro tabulku `payment`
--
ALTER TABLE `payment`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT pro tabulku `person`
--
ALTER TABLE `person`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=19;

--
-- AUTO_INCREMENT pro tabulku `place`
--
ALTER TABLE `place`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

-- --------------------------------------------------------

--
-- Omezení pro tabulku `account`
--
ALTER TABLE `account`
  ADD CONSTRAINT `account_ibfk_1` FOREIGN KEY (`owner`) REFERENCES `ib_account` (`id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `account_ibfk_2` FOREIGN KEY (`type`) REFERENCES `account_type` (`id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `account_ibfk_3` FOREIGN KEY (`verified_by`) REFERENCES `employee` (`id`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Omezení pro tabulku `card`
--
ALTER TABLE `card`
  ADD CONSTRAINT `card_ibfk_1` FOREIGN KEY (`disponent`) REFERENCES `disponent` (`id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `card_ibfk_2` FOREIGN KEY (`verified_by1`) REFERENCES `employee` (`id`) ON DELETE SET NULL ON UPDATE CASCADE,
  ADD CONSTRAINT `card_ibfk_3` FOREIGN KEY (`verified_by2`) REFERENCES `employee` (`id`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Omezení pro tabulku `disponent`
--
ALTER TABLE `disponent`
  ADD CONSTRAINT `disponent_ibfk_1` FOREIGN KEY (`account`) REFERENCES `account` (`id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `disponent_ibfk_2` FOREIGN KEY (`ib_account`) REFERENCES `ib_account` (`id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `disponent_ibfk_3` FOREIGN KEY (`verified_by`) REFERENCES `employee` (`id`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Omezení pro tabulku `employee`
--
ALTER TABLE `employee`
  ADD CONSTRAINT `employee_ibfk_1` FOREIGN KEY (`person`) REFERENCES `person` (`id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `employee_ibfk_2` FOREIGN KEY (`workplace`) REFERENCES `place` (`id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `employee_ibfk_3` FOREIGN KEY (`ib_account`) REFERENCES `ib_account` (`id`) ON UPDATE CASCADE;

--
-- Omezení pro tabulku `ib_account`
--
ALTER TABLE `ib_account`
  ADD CONSTRAINT `ib_account_ibfk_1` FOREIGN KEY (`person`) REFERENCES `person` (`id`) ON UPDATE CASCADE;

--
-- Omezení pro tabulku `notification`
--
ALTER TABLE `notification`
  ADD CONSTRAINT `notification_ibfk_1` FOREIGN KEY (`account`) REFERENCES `ib_account` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Omezení pro tabulku `payment`
--
ALTER TABLE `payment`
  ADD CONSTRAINT `payment_ibfk_1` FOREIGN KEY (`creator_disponent_id`) REFERENCES `disponent` (`id`) ON UPDATE CASCADE;

--
-- Omezení pro tabulku `person`
--
ALTER TABLE `person`
  ADD CONSTRAINT `person_ibfk_1` FOREIGN KEY (`verified_by`) REFERENCES `employee` (`id`) ON DELETE SET NULL ON UPDATE CASCADE;

SET FOREIGN_KEY_CHECKS=1;
