/*
IDS projekt (2016/2017)
Autori:
- Marek Sipos (xsipos03)
- Patrik Sztefek (xsztef02)
*/

/************************************
           RUSENI TABULEK
************************************/

DROP TABLE Osoba CASCADE CONSTRAINTS;
DROP TABLE Zamestnanec CASCADE CONSTRAINTS;
DROP TABLE Pokoj CASCADE CONSTRAINTS;
DROP TABLE Sluzba CASCADE CONSTRAINTS;
DROP TABLE Obdobi CASCADE CONSTRAINTS;
DROP TABLE Pobyt CASCADE CONSTRAINTS;
DROP TABLE Sluzbypobyty CASCADE CONSTRAINTS;
DROP TABLE Platba CASCADE CONSTRAINTS;

/************************************
          RUSENI SEKVENCI
************************************/

DROP SEQUENCE SEQ_Zamestnanec;
DROP SEQUENCE SEQ_Pokoj;
DROP SEQUENCE SEQ_Sluzba;
DROP SEQUENCE SEQ_Obdobi;
DROP SEQUENCE SEQ_Pobyt;
DROP SEQUENCE SEQ_Sluzbypobyty;
DROP SEQUENCE SEQ_Platba;



/************************************
  TVORBA TABULEK A JEJICH PROPOJENI
************************************/

CREATE TABLE Osoba
(
rodne_cislo CHAR(11) CONSTRAINT oso_rcis_NN NOT NULL CONSTRAINT oso_rcis_RC CHECK(mod(rodne_cislo,11) = 0), -- Check na overeni delitelnosti 11
jmeno CHAR(50) CONSTRAINT oso_jmen_NN NOT NULL,
prijmeni CHAR(50) CONSTRAINT oso_prij_NN NOT NULL,
datum_narozeni DATE, -- Oracle vklada k DATE i cas
pohlavi CHAR(1) CONSTRAINT oso_pohl_IN CHECK( pohlavi IN ('M','Z') ), -- Check na overeni povolenych hodnot
CONSTRAINT PK_oso PRIMARY KEY (rodne_cislo)
);

CREATE TABLE Zamestnanec
(
ID INT CONSTRAINT zam_id_NN NOT NULL,
rodne_cislo CHAR(11) CONSTRAINT zam_rcis_NN NOT NULL CONSTRAINT zam_rcis_RC CHECK(mod(rodne_cislo,11) = 0), -- SPECIALIZACE osoba <- zamestnanec
pozice CHAR(20),
plat DECIMAL(19,4),
adresa CHAR(255),
aktivni NUMBER(1,0) DEFAULT 1 CONSTRAINT zam_akti_NN NOT NULL,
CONSTRAINT PK_zam PRIMARY KEY (ID),
CONSTRAINT FK_zam_osob FOREIGN KEY (rodne_cislo) REFERENCES Osoba
);

CREATE TABLE Pokoj
(
ID INT CONSTRAINT pok_id_NN NOT NULL,
nazev CHAR(30) CONSTRAINT pok_naze_NN NOT NULL,
popis CHAR(255),
cena DECIMAL(19,4) CONSTRAINT pok_cena_NN NOT NULL,
CONSTRAINT PK_pok PRIMARY KEY (ID)
);

CREATE TABLE Sluzba
(
ID INT CONSTRAINT slu_id_NN NOT NULL,
nazev CHAR(30) CONSTRAINT slu_naze_NN NOT NULL,
popis CHAR(255),
cena DECIMAL(19,4) CONSTRAINT slu_cena_NN NOT NULL,
CONSTRAINT PK_slu PRIMARY KEY (ID)
);



CREATE TABLE Obdobi
(
ID INT CONSTRAINT obd_id_NN NOT NULL,
od INT CONSTRAINT obd_od_NN NOT NULL,
do INT CONSTRAINT obd_do_NN NOT NULL,
koeficient DECIMAL(5,2) DEFAULT 1.00 CONSTRAINT obd_koef_NN NOT NULL,
CONSTRAINT PK_obd PRIMARY KEY (ID)
);

CREATE TABLE Pobyt
(
ID INT CONSTRAINT pob_id_NN NOT NULL, -- NOT NULL u PK neni povinne, ale DB si tuhle explicitni informaci uchovava pro pripad, ze by sloupec prestal byt PK
zadano DATE DEFAULT CURRENT_TIMESTAMP, -- Vychozi hodnota zadani do systemu
trvani_od DATE CONSTRAINT pob_trod_NN NOT NULL,
trvani_do DATE CONSTRAINT pob_trdo_NN NOT NULL,
obdobi INT CONSTRAINT pob_obdo_NN NOT NULL, -- FOREIGN
prevzato DATE,
odevzdano DATE,
cena_celkem DECIMAL(19,4) CONSTRAINT pob_cena_NN NOT NULL, -- Presnost 19 cislic, z toho 4 desetinna
zadal INT CONSTRAINT pob_zada_NN NOT NULL, -- FOREIGN
pokoj INT CONSTRAINT pob_poko_NN NOT NULL, -- FOREIGN
rezervoval CHAR(11) CONSTRAINT pob_reze_RC CHECK(mod(rezervoval,11) = 0), -- FOREIGN
klient CHAR(11) CONSTRAINT pob_klie_NN NOT NULL CONSTRAINT pob_klie_RC CHECK(mod(klient,11) = 0), -- FOREIGN
CONSTRAINT PK_pob PRIMARY KEY (ID),
CONSTRAINT FK_pob_obdo FOREIGN KEY (obdobi) REFERENCES Obdobi,
CONSTRAINT FK_pob_zada FOREIGN KEY (zadal) REFERENCES Zamestnanec,
CONSTRAINT FK_pob_poko FOREIGN KEY (pokoj) REFERENCES Pokoj,
CONSTRAINT FK_pob_reze FOREIGN KEY (rezervoval) REFERENCES Osoba,
CONSTRAINT FK_pob_klie FOREIGN KEY (klient) REFERENCES Osoba
);



CREATE TABLE Sluzbypobyty -- Rozbiti vazby N:N u pobytu a sluzeb
(
ID INT CONSTRAINT slp_id_NN NOT NULL,
pobyt INT CONSTRAINT slp_poby_NN NOT NULL, -- FOREIGN
sluzba INT CONSTRAINT slp_sluz_NN NOT NULL, -- FOREIGN
typ CHAR(30) CONSTRAINT slp_typ_NN NOT NULL CONSTRAINT slp_typ_IN CHECK( typ IN ('zahrnuto','navic') ),
vyuzito DATE, -- NULL = nevyuzito zatim
pocet INT CONSTRAINT slp_poce_NN NOT NULL,
CONSTRAINT PK_slp PRIMARY KEY (ID),
CONSTRAINT FK_slp_poby FOREIGN KEY (pobyt) REFERENCES Pobyt,
CONSTRAINT FK_slp_sluz FOREIGN KEY (sluzba) REFERENCES Sluzba
);

CREATE TABLE Platba
(
ID INT CONSTRAINT pla_id_NN NOT NULL,
castka DECIMAL(19,4) CONSTRAINT pla_cast_NN NOT NULL,
zaplaceno DATE, -- Prazdne = zatim nezaplaceno
typ CHAR(8) CONSTRAINT pla_typ_NN NOT NULL CONSTRAINT pla_typ_IN CHECK( typ IN ('zaloha','doplatek','hlavni') ),
prevzal INT CONSTRAINT pla_prev_NN NOT NULL, -- FOREIGN
zaplatil CHAR(11) CONSTRAINT pla_zapl_NN NOT NULL CONSTRAINT pla_zapl_RC CHECK(mod(zaplatil,11) = 0), -- FOREIGN
pobyt INT CONSTRAINT pla_poby_NN NOT NULL, -- FOREIGN
CONSTRAINT PK_pla PRIMARY KEY (ID),
CONSTRAINT FK_pla_prev FOREIGN KEY (prevzal) REFERENCES Zamestnanec,
CONSTRAINT FK_pla_zapl FOREIGN KEY (zaplatil) REFERENCES Osoba,
CONSTRAINT FK_pla_pobyt FOREIGN KEY (pobyt) REFERENCES Pobyt
);

/************************************
          TVORBA SEKVENCI
************************************/

CREATE SEQUENCE SEQ_Zamestnanec INCREMENT BY 1 START WITH 1 NOMAXVALUE MINVALUE 0;
CREATE SEQUENCE SEQ_Pokoj INCREMENT BY 1 START WITH 1 NOMAXVALUE MINVALUE 0;
CREATE SEQUENCE SEQ_Sluzba INCREMENT BY 1 START WITH 1 NOMAXVALUE MINVALUE 0;
CREATE SEQUENCE SEQ_Obdobi INCREMENT BY 1 START WITH 1 NOMAXVALUE MINVALUE 0;
CREATE SEQUENCE SEQ_Pobyt INCREMENT BY 1 START WITH 1 NOMAXVALUE MINVALUE 0;
CREATE SEQUENCE SEQ_Sluzbypobyty INCREMENT BY 1 START WITH 1 NOMAXVALUE MINVALUE 0;
CREATE SEQUENCE SEQ_Platba INCREMENT BY 1 START WITH 1 NOMAXVALUE MINVALUE 0;

/************************************
          TVORBA TRIGGERU
************************************/

-- Trigger na automaticky vypocet celkove ceny pobytu a prirazeni obdobi
create or replace TRIGGER vypocet_ceny BEFORE INSERT 
  ON Pobyt
  FOR each row
DECLARE 
  p_cena INT;
  p_dnu INT;
  coef DECIMAL(5,2);
  obd INT;
BEGIN
  SELECT id, koeficient into obd, coef FROM obdobi WHERE do >= to_number(to_char(:NEW.trvani_od,'mm')) AND od <= to_number(to_char(:NEW.trvani_od,'mm'));
  SELECT cena into p_cena FROM pokoj WHERE id = :NEW.pokoj;
  p_dnu := :NEW.trvani_do - :NEW.trvani_od;
  :NEW.cena_celkem := p_dnu * p_cena * coef;
  :NEW.obdobi := obd;
END vypocet_ceny;
/
-- Auto increment triggery
CREATE OR REPLACE TRIGGER TR_Zamestnanec BEFORE INSERT ON Zamestnanec FOR EACH ROW
begin
  if :NEW.ID is null then 
    SELECT SEQ_Zamestnanec.nextval INTO :NEW.ID FROM DUAL; 
  end if; 
end TR_Zamestnanec;
/
CREATE OR REPLACE TRIGGER TR_Pokoj BEFORE INSERT ON Pokoj FOR EACH ROW
begin
  if :NEW.ID is null then 
    SELECT SEQ_Pokoj.nextval INTO :NEW.ID FROM DUAL; 
  end if; 
end TR_Pokoj;
/
CREATE OR REPLACE TRIGGER TR_Sluzba BEFORE INSERT ON Sluzba FOR EACH ROW
begin
  if :NEW.ID is null then 
    SELECT SEQ_Sluzba.nextval INTO :NEW.ID FROM DUAL; 
  end if; 
end TR_Sluzba;
/
CREATE OR REPLACE TRIGGER TR_Obdobi BEFORE INSERT ON Obdobi FOR EACH ROW
begin
  if :NEW.ID is null then 
    SELECT SEQ_Obdobi.nextval INTO :NEW.ID FROM DUAL; 
  end if; 
end TR_Obdobi;
/
CREATE OR REPLACE TRIGGER TR_Pobyt BEFORE INSERT ON Pobyt FOR EACH ROW
begin
  if :NEW.ID is null then 
    SELECT SEQ_Pobyt.nextval INTO :NEW.ID FROM DUAL; 
  end if; 
end TR_Pobyt;
/
CREATE OR REPLACE TRIGGER TR_Sluzbypobyty BEFORE INSERT ON Sluzbypobyty FOR EACH ROW
begin
  if :NEW.ID is null then 
    SELECT SEQ_Sluzbypobyty.nextval INTO :NEW.ID FROM DUAL; 
  end if; 
end TR_Sluzbypobyty;
/
CREATE OR REPLACE TRIGGER TR_Platba BEFORE INSERT ON Platba FOR EACH ROW
begin
  if :NEW.ID is null then 
    SELECT SEQ_Platba.nextval INTO :NEW.ID FROM DUAL; 
  end if; 
end TR_Platba;
/

/************************************
          TVORBA PROCEDUR
************************************/

-- Vypise vykonnost zamestnancu
create or replace PROCEDURE staff_stats
IS
  all_count NUMBER;
BEGIN
  
  SELECT count(*) into all_count FROM Pobyt;

  for row1 in (SELECT O.JMENO, O.PRIJMENI, count(P.id) AS CNT FROM Osoba O, Pobyt P, Zamestnanec Z WHERE Z.rodne_cislo = O.rodne_cislo AND Z.ID = P.ZADAL GROUP BY O.JMENO,O.PRIJMENI)
  loop
    DBMS_OUTPUT.PUT_LINE(  ROUND( (row1.CNT/all_count*100),2 )    ||'% Pobytu pro: ' || row1.JMENO || row1.PRIJMENI);
  end loop;

  EXCEPTION
        WHEN ZERO_DIVIDE THEN
            Raise_Application_Error (-20005, 'ERROR: ZERO DIVIDE in staff_stats!');
        
        WHEN OTHERS THEN
            Raise_Application_Error (-20999, 'ERROR: Error in staff_stats!');
END;
/
/************************************
  NAPLNENI TABULEK UKAZKOVYMI DATY
************************************/

INSERT INTO Osoba (rodne_cislo, jmeno, prijmeni, datum_narozeni, pohlavi) VALUES ('9712101366', 'Martin', 'Testovací', TO_DATE('01-12-1997', 'dd-mm-yyyy'), 'M');
INSERT INTO Osoba (rodne_cislo, jmeno, prijmeni, datum_narozeni, pohlavi) VALUES ('9955083479', 'Franta', 'Rychlý', TO_DATE('08-05-1999', 'dd-mm-yyyy'), 'M');
INSERT INTO Osoba (rodne_cislo, jmeno, prijmeni, datum_narozeni, pohlavi) VALUES ('7853146059', 'Aleš', 'Modřín', TO_DATE('14-03-1978', 'dd-mm-yyyy'), 'M');
INSERT INTO Osoba (rodne_cislo, jmeno, prijmeni, datum_narozeni, pohlavi) VALUES ('9259190149', 'Klára', 'Jirková', TO_DATE('19-09-1992', 'dd-mm-yyyy'), 'Z');
INSERT INTO Osoba (rodne_cislo, jmeno, prijmeni, datum_narozeni, pohlavi) VALUES ('7457117008', 'Muhammad', 'Ali Pa', TO_DATE('11-07-1974', 'dd-mm-yyyy'), 'M');
INSERT INTO Osoba (rodne_cislo, jmeno, prijmeni, datum_narozeni, pohlavi) VALUES ('8510039967', 'Evžen', 'Sidržporský', TO_DATE('03-10-1985', 'dd-mm-yyyy'), 'M');

INSERT INTO Zamestnanec (rodne_cislo, pozice, plat, adresa, aktivni) VALUES ('7853146059', 'Údržba', 24600, 'Adršpachy 13, 79397 Amalín', 1);
INSERT INTO Zamestnanec (rodne_cislo, pozice, plat, adresa, aktivni) VALUES ('9259190149', 'Recepční', 21400, 'Huskvoucí 14/5, 79351 Albrechtice u Rýmařova', 1);

INSERT INTO Pokoj ( nazev, popis, cena) VALUES ('Běžný 1', 'Krásný pokoj s balkónem', 1600);
INSERT INTO Pokoj ( nazev, popis, cena) VALUES ('Běžný 2', 'Krásný pokoj s balkónem', 1600);
INSERT INTO Pokoj ( nazev, popis, cena) VALUES ('Běžný 3 (moře)', 'Krásný pokoj s balkónem s výhledem na moře', 1900);
INSERT INTO Pokoj ( nazev, popis, cena) VALUES ('Běžný 4', 'Krásný pokoj s balkónem', 1600);
INSERT INTO Pokoj ( nazev, popis, cena) VALUES ('Apartmán 1', 'Luxusní pokoj jako pro krále', 4242);
INSERT INTO Pokoj ( nazev, popis, cena) VALUES ('Apartmán 2', 'Luxusní pokoj jako pro krále', 4242);

INSERT INTO Sluzba (nazev, popis, cena) VALUES ('Šampus na pokoj', 'Bublinky pro klidné večery nebo bouřlivé oslavy', 240);
INSERT INTO Sluzba (nazev, popis, cena) VALUES ('Úklid extra', 'Potřebujete uklidit mimo běžnou dobu? Není problém!', 500);

INSERT INTO Obdobi (od, do, koeficient) VALUES (1,1, 1.20);
INSERT INTO Obdobi (od, do, koeficient) VALUES (2,6, 1.00);
INSERT INTO Obdobi (od, do, koeficient) VALUES (7,8, 1.50);
INSERT INTO Obdobi (od, do, koeficient) VALUES (9,11, 1.00);
INSERT INTO Obdobi (od, do, koeficient) VALUES (12,12, 1.20);

INSERT INTO Pobyt (zadano, trvani_od, trvani_do,  prevzato, zadal, pokoj, klient) 
  VALUES (to_date('21-01-2017', 'dd-mm-yyyy'), to_date('23-03-2017', 'dd-mm-yyyy'), to_date('15-04-2017', 'dd-mm-yyyy'), to_date('28-03-2017', 'dd-mm-yyyy'), 2, 2,'9259190149');

INSERT INTO Pobyt (zadano, trvani_od, trvani_do,  prevzato, zadal, pokoj, rezervoval, klient) 
  VALUES (to_date('21-01-2017', 'dd-mm-yyyy'), to_date('23-03-2017', 'dd-mm-yyyy'), to_date('27-03-2017', 'dd-mm-yyyy'), to_date('23-03-2017', 'dd-mm-yyyy'), 1, 1, '9712101366 ', '9712101366');

INSERT INTO Pobyt (zadano, trvani_od, trvani_do,  prevzato, zadal, pokoj, klient) 
  VALUES (to_date('28-04-2017', 'dd-mm-yyyy'), to_date('28-04-2017', 'dd-mm-yyyy'), to_date('03-05-2017', 'dd-mm-yyyy'), to_date('28-04-2017', 'dd-mm-yyyy'), 1, 1, '9712101366');

INSERT INTO Pobyt (zadano, trvani_od, trvani_do,  prevzato, zadal, pokoj, klient) 
  VALUES (to_date('21-03-2017', 'dd-mm-yyyy'), to_date('28-03-2017', 'dd-mm-yyyy'), to_date('10-04-2017', 'dd-mm-yyyy'), to_date('28-03-2017', 'dd-mm-yyyy'), 2, 1, '9955083479');

INSERT INTO Platba (castka, zaplaceno, typ, prevzal, zaplatil, pobyt) VALUES ( 50000, TO_DATE('23-03-2017', 'dd-mm-yyyy'), 'zaloha', 2, '7457117008', 1);
INSERT INTO Platba (castka, zaplaceno, typ, prevzal, zaplatil, pobyt) VALUES ( 56000, TO_DATE('23-03-2017', 'dd-mm-yyyy'), 'zaloha', 2, '7457117008', 2);

/************************************
       VYBEROVE SELECT DOTAZY
************************************/

-- Seznam nevracenych pokoju / spojeni 2 tabulek
SELECT * 
  FROM POKOJ P, POBYT PO
  WHERE PO.POKOJ = P.ID AND PO.ODEVZDANO IS NULL;
  
-- Seznam zakazniku, kteri provedli rezervaci / spojeni 2 tabulek
SELECT DISTINCT O.JMENO, O.PRIJMENI
  FROM OSOBA O, POBYT P
  WHERE P.REZERVOVAL = O.RODNE_CISLO;

-- Seznam zakazniku, ktere odbavil zamestranec / spojeni 3 tabulek
SELECT DISTINCT O.JMENO, O.PRIJMENI
  FROM OSOBA O,ZAMESTNANEC Z, POBYT P
  WHERE O.RODNE_CISLO = P.KLIENT AND Z.ID = P.ZADAL AND Z.ID = 2;

-- Prumerna cena pobytu za ruzna obdobi / GROUP BY + AGREGACNI FCE
SELECT AVG(P.CENA_CELKEM), O.OD, O.DO
  FROM POBYT P, OBDOBI O
  WHERE P.OBDOBI = O.ID
  GROUP BY O.OD,O.DO;

-- Pocet odbavenych pobytu pro kazdeho zamestnance / GROUP BY + AGREGACNI FCE
SELECT COUNT(*), Z.ID
  FROM ZAMESTNANEC Z, POBYT P
  WHERE P.ZADAL = Z.ID
  GROUP BY Z.ID;

-- Seznam nezaplacenych pobytu / EXIST
SELECT *
  FROM POBYT P
  WHERE NOT EXISTS (
    SELECT * 
      FROM PLATBA PL 
      WHERE PL.POBYT = P.ID
  );

-- Jmena klientu ubytovanych na pokoji 2 / IN + vnoreny SELECT
SELECT DISTINCT O.JMENO, O.PRIJMENI 
  FROM OSOBA O 
  WHERE O.RODNE_CISLO IN (
    SELECT P.KLIENT 
    FROM POBYT P 
    WHERE P.POKOJ = 2
  );

/************************************
         SPUSTENI PROCEDUR
************************************/

exec staff_stats;

/************************************
        INDEX, EXPLAIN PLAN
************************************/

DROP INDEX performanceIndex;

EXPLAIN PLAN SET STATEMENT_ID = 'performanceAnalysisBefore' FOR
  SELECT Pokoj.ID, Pokoj.Nazev, COUNT(Pobyt.pokoj) AS pocet_ubytovani
  FROM Pokoj LEFT JOIN Pobyt ON (Pokoj.ID = Pobyt.pokoj)
  GROUP BY Pokoj.ID, Pokoj.Nazev ORDER BY Pokoj.ID;
SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.display('plan_table', 'performanceAnalysisBefore', 'typical'));

CREATE INDEX performanceIndex ON Pobyt (Pokoj);

EXPLAIN PLAN SET STATEMENT_ID = 'performanceAnalysisAfter' FOR
  SELECT Pokoj.ID, Pokoj.Nazev, COUNT(Pobyt.pokoj) AS pocet_ubytovani
  FROM Pokoj LEFT JOIN Pobyt ON (Pokoj.ID = Pobyt.pokoj)
  GROUP BY Pokoj.ID, Pokoj.Nazev ORDER BY Pokoj.ID;
SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.display('plan_table', 'performanceAnalysisAfter', 'typical'));

/************************************
    PRISTUPOVA OPRAVNENI XSZTEF02
************************************/

-- Pristupy k tabulkam
GRANT ALL PRIVILEGES ON Osoba TO xsztef02;
GRANT SELECT ON Zamestnanec TO xsztef02;
GRANT ALL PRIVILEGES ON Pokoj TO xsztef02;
GRANT ALL PRIVILEGES ON Sluzba TO xsztef02;
GRANT ALL PRIVILEGES ON Pobyt TO xsztef02;
GRANT ALL PRIVILEGES ON Sluzbypobyty TO xsztef02;
GRANT ALL PRIVILEGES ON Platba TO xsztef02;

-- Pristupy k proceduram
-- GRANT EXECUTE ON staff_stats TO xsztef02; -- zamestnanci na to nemaji pravo

/************************************
      MATERIALIZOVANY POHLED
************************************/

DROP MATERIALIZED VIEW obl_pokoje;
DROP MATERIALIZED VIEW LOG ON POKOJ;
DROP MATERIALIZED VIEW LOG ON POBYT;

CREATE MATERIALIZED VIEW LOG ON POKOJ WITH PRIMARY KEY, ROWID(nazev)
INCLUDING NEW VALUES;

CREATE MATERIALIZED VIEW LOG ON POBYT WITH PRIMARY KEY, ROWID(pokoj)
INCLUDING NEW VALUES;

CREATE MATERIALIZED VIEW obl_pokoje
BUILD IMMEDIATE        
REFRESH FAST ON COMMIT 
ENABLE QUERY REWRITE
AS SELECT  POKOJ.NAZEV, COUNT(POBYT.POKOJ) as POCET_POBYTU
FROM POKOJ, POBYT
WHERE POKOJ.ID = POBYT.POKOJ
GROUP BY POKOJ.NAZEV ORDER BY POCET_POBYTU DESC;

SELECT * FROM obl_pokoje;

INSERT INTO Pobyt (zadano, trvani_od, trvani_do,  prevzato, zadal, pokoj, rezervoval, klient) 
  VALUES (to_date('21-01-2017', 'dd-mm-yyyy'), to_date('23-03-2017', 'dd-mm-yyyy'), to_date('10-05-2017', 'dd-mm-yyyy'), to_date('23-03-2017', 'dd-mm-yyyy'), 1, 5, '9712101366 ', '9712101366');

INSERT INTO Pobyt (zadano, trvani_od, trvani_do,  prevzato, zadal, pokoj, klient) 
  VALUES (to_date('21-03-2017', 'dd-mm-yyyy'), to_date('28-03-2017', 'dd-mm-yyyy'), to_date('10-04-2017', 'dd-mm-yyyy'), to_date('28-03-2017', 'dd-mm-yyyy'), 1, 4, '9955083479');
COMMIT;

SELECT * FROM obl_pokoje;




