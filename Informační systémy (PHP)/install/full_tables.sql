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

--
-- Vypisuji data pro tabulku `account`
--

INSERT INTO `account` (`id`, `inserted`, `state`, `type`, `number`, `funds`, `owner`, `verified_by`, `verified_when`, `verified_comment`) VALUES
(1, '2018-12-03 16:20:11', 'REJECTED', 3, NULL, '0.00', 3, 2, '2018-12-03 16:21:03', 'Studentský účet je určen pouze pro studenty.<br />\r\nK dispozici jsou ovšem naše další typy účtů.'),
(2, '2018-12-03 16:21:26', 'ACTIVE', 1, '8643325568', '7500.00', 3, 2, '2018-12-03 16:23:21', NULL),
(3, '2018-12-03 17:06:10', 'ACTIVE', 2, '6273542432', '1000.00', 4, 2, '2018-12-03 17:06:22', '');

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

--
-- Vypisuji data pro tabulku `account_type`
--

INSERT INTO `account_type` (`id`, `name`, `offered`, `monthly_fee`, `interest`) VALUES
(1, 'Běžný účet', 1, '30.00', '0.15'),
(2, 'Spořicí účet', 1, '50.00', '0.90'),
(3, 'Studentský účet', 1, '0.00', '0.05');

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

--
-- Vypisuji data pro tabulku `card`
--

INSERT INTO `card` (`id`, `inserted`, `state`, `number`, `pin`, `disponent`, `expires`, `verified_by1`, `verified_when1`, `verified_comment1`, `verified_by2`, `verified_when2`, `verified_comment2`) VALUES
(1, '2018-12-03 17:00:24', 'ACTIVE', '1684532568954126', '$2y$10$F/c4.e3BnHHrCWbPkJtHZe4yKVqxpVXtxdelR76x8Wk.e/H0oIkp2', 1, '2025-05-05 00:00:00', 2, '2018-12-03 17:02:35', '', 2, '2018-12-03 17:03:05', ''),
(2, '2018-12-03 17:04:05', 'ACTIVE', '8413648563452165', '$2y$10$jHJisE/SXPIOmXIGQPxWR.VzBZAfR8xRoilQRKMK/XPrZUXIbVFb.', 2, '2020-12-12 00:00:00', 1, '2018-12-03 17:04:17', '', 1, '2018-12-03 17:04:31', ''),
(3, '2018-12-03 17:07:30', 'REJECTED', NULL, NULL, 3, NULL, 1, '2018-12-03 17:07:53', '', 1, '2018-12-03 17:08:03', 'Došlo k problémům, kartu sežral pes'),
(4, '2018-12-03 17:08:32', 'ACTIVE', '8543569852142052', '$2y$10$puV8MHrU9bLy.lNWskzSn.XcMY2T8vch246boxZn5Z9sHi7BPp/3G', 3, '2025-06-28 00:00:00', 2, '2018-12-03 17:08:43', '', 2, '2018-12-03 17:09:01', '');

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

--
-- Vypisuji data pro tabulku `disponent`
--

INSERT INTO `disponent` (`id`, `inserted`, `state`, `account`, `ib_account`, `custom_name`, `verified_by`, `verified_when`, `verified_comment`) VALUES
(1, '2018-12-03 16:23:21', 'ACTIVE', 2, 3, 'Můj první účet', 2, '2018-12-03 16:23:21', '(vytvořeno automaticky - majitel účtu)'),
(2, '2018-12-03 17:00:22', 'ACTIVE', 2, 4, 'Pepův účet', 2, '2018-12-03 17:02:28', ''),
(3, '2018-12-03 17:06:22', 'ACTIVE', 3, 4, NULL, 2, '2018-12-03 17:06:22', '(vytvořeno automaticky - majitel účtu)');

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

--
-- Vypisuji data pro tabulku `employee`
--

INSERT INTO `employee` (`id`, `person`, `workplace`, `ib_account`, `start`, `end`) VALUES
(1, 1, 1, 1, '2018-01-31 00:00:00', NULL),
(2, 2, 2, 2, '2018-10-30 00:00:00', NULL);

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

--
-- Vypisuji data pro tabulku `ib_account`
--

INSERT INTO `ib_account` (`id`, `inserted`, `state`, `role`, `activation_code`, `reset_pw_code`, `name`, `password`, `person`, `email`) VALUES
(1, '2018-12-03 14:51:59', 'ACTIVE', 'ADMIN', NULL, NULL, 'admin', '$2y$10$DJqGA8HnuFHha666DwCWcus9ffB40.AKqvTeLXUJ7q4wqSSxXplau', 1, 'admin@iwitrag.cz'),
(2, '2018-12-03 14:54:34', 'ACTIVE', 'EMPLOYEE', NULL, NULL, 'employee', '$2y$10$.7HB9Rz.chzRXYd67qdDMOei9ocR2IbarOd7ErlibC5U4zWZyR4nq', 2, 'z.zamecnicka@iwitrag.cz'),
(3, '2018-12-03 14:43:57', 'ACTIVE', 'CLIENT', NULL, NULL, 'pepa', '$2y$10$L5mrXDhKq0SW/M3W1KFto.wE/GTAoPfWmtX7.GQJK1W9pckpsPoju', 3, 'pepek.namornik@nejakaadresa.cz'),
(4, '2018-12-03 16:56:59', 'ACTIVE', 'CLIENT', NULL, NULL, 'zamecnicka', '$2y$10$.7HB9Rz.chzRXYd67qdDMOei9ocR2IbarOd7ErlibC5U4zWZyR4nq', 2, 'zuzkazam@nejakaadresa.cz');

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

--
-- Vypisuji data pro tabulku `notification`
--

INSERT INTO `notification` (`id`, `account`, `text`, `seen`, `created`) VALUES
(3, 3, 'Dobrý den,<br />\r\n<br />\r\nvaše e-mailová adresa byla ověřena. Děkujeme!<br />\r\nPokud budete mít jakékoliv dotazy, neváhejte a obraťte se na naši infolinku.<br />\r\n<br />\r\n<em>Vaše Simple Bank</em>', 1, '2018-12-03 16:15:22'),
(4, 3, 'Vaše osobní údaje byly úspěšně ověřeny zaměstnancem!<br />V případě jakýchkoliv dotazů kontaktujte naši infolinku!<br /><br />Zaměstnanec přidal tento komentář:<br />Ověřeno na pobočce', 1, '2018-12-03 16:16:44'),
(5, 3, 'Vaše žádost o otevření účtu (Studentský účet) ze dne 3.12.2018 byla zamítnuta zaměstnancem.<br />V případě zájmu si můžete zažádat o účet znovu.<br /><br />Zaměstnanec přidal tento komentář:<br />Studentský účet je určen pouze pro studenty.<br />\r\nK dispozici jsou ovšem naše další typy účtů.', 1, '2018-12-03 16:21:03'),
(6, 3, 'Vaše žádost o otevření účtu (Běžný účet) ze dne 3.12.2018 byla schválena!<br />Účet byl otevřen.', 1, '2018-12-03 16:23:21'),
(7, 3, 'Dobrý den,<br />\r\n<br />\r\nv rámci zkvalitňování služeb zákazníkům jsme vám zaslali 10.000 Kč.<br />\r\nDoufáme, že máte radost!<br />\r\n<br />\r\nVaše Simple Bank', 1, '2018-12-03 16:39:23'),
(8, 1, 'Ráda bych byla zároveň klientkou, můžete mi založit prosím účet?<br />\r\n<br />\r\nZuzana Zámečnická', 1, '2018-12-03 16:53:29'),
(9, 2, 'Ano, budu potřebovat vaši osobní e-mailovou adresu.<br />\r\n<br />\r\nAdmin', 1, '2018-12-03 16:55:38'),
(10, 1, 'zuzkazam@nejakaadresa.cz<br />\r\n<br />\r\nDěkuji, Zuzana Zámečnická', 1, '2018-12-03 16:56:16'),
(11, 2, 'IB účet založen.<br />\r\nPřihl. jméno: zamecnicka<br />\r\nHeslo: stejné jako heslo k vašemu účtu employee<br />\r\n<br />\r\nadmin', 1, '2018-12-03 16:58:16'),
(12, 1, 'Děkuji<br />\r\n<br />\r\nZuzana Zámečnická', 1, '2018-12-03 16:58:49'),
(13, 3, 'Ahoj Pepo, přidáš mě tedy jako disponentku jak jsme se domlouvali osobně?<br />\r\n<br />\r\nÚčet je zamecnicka<br />\r\n<br />\r\nZuzka', 1, '2018-12-03 17:00:09'),
(14, 4, 'Dobrý den,<br />uživatel Josef Jandáč (494156741) vás chce přidat jako disponenta k účtu 8643325568. Pokud s tím souhlasíte, klikněte prosím <a href=\"/disponent-agree/2/agree\">ZDE</a>, jinak klikněte <a href=\"/disponent-agree/2/disagree\">ZDE</a>.', 1, '2018-12-03 17:00:22'),
(15, 3, 'Vaše žádost o přidání disponenta Zuzana Zámečnická (123123123) k účtu 8643325568 byla disponentem potvrzena!<br />Disponent bude přidán po ověření pracovníkem.', 1, '2018-12-03 17:02:16'),
(16, 3, 'Vaše žádost o přidání disponenta Zuzana Zámečnická (123123123) k účtu 8643325568 (Můj první účet) ze dne 3.12.2018 byla zpracována zaměstnancem!<br />Disponent byl přidán k účtu.', 1, '2018-12-03 17:02:28'),
(17, 4, 'Bylo vám uděleno dispoziční právo k účtu 8643325568 !', 1, '2018-12-03 17:02:28'),
(18, 3, 'Vaše žádost o přidání karty k účtu 8643325568 (Můj první účet) ze dne 3.12.2018 je nyní zpracovávána zaměstnancem!<br />Kartu vám brzy zašleme na vaši adresu.', 1, '2018-12-03 17:02:35'),
(19, 3, 'Vaše žádost o přidání karty k účtu 8643325568 (Můj první účet) ze dne 3.12.2018 byla vyřízena zaměstnancem!<br />Karta byla zaslána na vaši adresu.', 1, '2018-12-03 17:03:05'),
(20, 4, 'Vaše žádost o přidání karty k účtu 8643325568 (Běžný účet) ze dne 3.12.2018 je nyní zpracovávána zaměstnancem!<br />Kartu vám brzy zašleme na vaši adresu.', 1, '2018-12-03 17:04:17'),
(21, 4, 'Vaše žádost o přidání karty k účtu 8643325568 (Běžný účet) ze dne 3.12.2018 byla vyřízena zaměstnancem!<br />Karta byla zaslána na vaši adresu.', 1, '2018-12-03 17:04:31'),
(22, 2, 'Vaše osobní údaje byly úspěšně ověřeny zaměstnancem!<br />V případě jakýchkoliv dotazů kontaktujte naši infolinku!', 1, '2018-12-03 17:05:34'),
(23, 4, 'Vaše osobní údaje byly úspěšně ověřeny zaměstnancem!<br />V případě jakýchkoliv dotazů kontaktujte naši infolinku!', 1, '2018-12-03 17:05:34'),
(24, 4, 'Vaše žádost o otevření účtu (Spořicí účet) ze dne 3.12.2018 byla schválena!<br />Účet byl otevřen.', 1, '2018-12-03 17:06:22'),
(25, 4, 'Vaše žádost o přidání karty k účtu 6273542432 (Spořicí účet) ze dne 3.12.2018 je nyní zpracovávána zaměstnancem!<br />Kartu vám brzy zašleme na vaši adresu.', 1, '2018-12-03 17:07:53'),
(26, 4, 'Vaše žádost o přidání karty k účtu 6273542432 (Spořicí účet) ze dne 3.12.2018 byla stornována zaměstnancem!<br />Můžete si vytvořit žádost novou.<br /><br />Zaměstnanec přidal tento komentář:<br />Došlo k problémům, kartu sežral pes', 1, '2018-12-03 17:08:03'),
(27, 4, 'Vaše žádost o přidání karty k účtu 6273542432 (Spořicí účet) ze dne 3.12.2018 je nyní zpracovávána zaměstnancem!<br />Kartu vám brzy zašleme na vaši adresu.', 1, '2018-12-03 17:08:43'),
(28, 4, 'Vaše žádost o přidání karty k účtu 6273542432 (Spořicí účet) ze dne 3.12.2018 byla vyřízena zaměstnancem!<br />Karta byla zaslána na vaši adresu.', 1, '2018-12-03 17:09:01'),
(29, 4, 'Vaše žádost o vytvoření karty k účtu  (Spořicí účet) ze dne 3.12.2018 byla zrušena zaměstnancem.', 1, '2018-12-03 17:14:49');

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

--
-- Vypisuji data pro tabulku `payment`
--

INSERT INTO `payment` (`id`, `from_acc`, `from_bank_code`, `to_acc`, `to_bank_code`, `amount`, `created`, `creator_disponent_id`, `processed`, `variable_symbol`, `constant_symbol`, `specific_symbol`, `message`) VALUES
(1, '8643325568', '1234', '123456789', '5555', '1500.00', '2018-12-03 16:44:22', 1, NULL, NULL, NULL, NULL, 'Alimenty'),
(2, '8643325568', '1234', '6273542432', '1234', '1000.00', '2018-12-03 17:07:05', 2, NULL, NULL, NULL, NULL, 'Přesouvám si peníze na můj účet, promiň Pepo, já ty nové boty potřebuju');

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

--
-- Vypisuji data pro tabulku `person`
--

INSERT INTO `person` (`id`, `inserted`, `state`, `first_name`, `second_name`, `identification`, `country_code`, `address`, `birthdate`, `verified_by`, `verified_when`, `verified_comment`) VALUES
(1, '2018-01-31 00:00:00', 'VERIFIED', 'Adrian', 'Adršpach', '1_555666777', 'CZE', 'Nejvyšší digitální sféra', '1950-04-04', 1, '2018-01-31 00:00:00', NULL),
(2, '2018-10-30 00:00:00', 'VERIFIED', 'Zuzana', 'Zámečnická', '2_123123123', 'CZE', 'Moravské náměstí 13, Brno', '1985-05-05', 2, '2018-12-03 17:05:34', ''),
(3, '2018-12-03 16:10:47', 'VERIFIED', 'Josef', 'Jandáč', '3_494156741', 'CZE', 'Malá Lhota 1', '1969-06-15', 2, '2018-12-03 16:16:44', 'Ověřeno na pobočce');

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

--
-- Vypisuji data pro tabulku `place`
--

INSERT INTO `place` (`id`, `name`, `address`, `opened`, `closed`) VALUES
(1, 'Centrála', 'Rudé náměstí 16, Moskva', '2018-01-31 00:00:00', NULL),
(2, 'Pobočka v Brně', 'Brno Street 17', '2018-10-30 00:00:00', NULL);

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
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT pro tabulku `account_type`
--
ALTER TABLE `account_type`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT pro tabulku `card`
--
ALTER TABLE `card`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT pro tabulku `disponent`
--
ALTER TABLE `disponent`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT pro tabulku `employee`
--
ALTER TABLE `employee`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT pro tabulku `ib_account`
--
ALTER TABLE `ib_account`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT pro tabulku `notification`
--
ALTER TABLE `notification`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=30;

--
-- AUTO_INCREMENT pro tabulku `payment`
--
ALTER TABLE `payment`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

--
-- AUTO_INCREMENT pro tabulku `person`
--
ALTER TABLE `person`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;

--
-- AUTO_INCREMENT pro tabulku `place`
--
ALTER TABLE `place`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=3;

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
