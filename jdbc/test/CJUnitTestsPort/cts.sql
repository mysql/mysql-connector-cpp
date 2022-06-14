
-- Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License, version 2.0, as
-- published by the Free Software Foundation.
--
-- This program is also distributed with certain software (including
-- but not limited to OpenSSL) that is licensed under separate terms,
-- as designated in a particular file or component or in included license
-- documentation.  The authors of MySQL hereby grant you an
-- additional permission to link the program and your derivative works
-- with the separately licensed software that they have included with
-- MySQL.
--
-- Without limiting anything contained in the foregoing, this file,
-- which is part of MySQL Connector/C++, is also subject to the
-- Universal FOSS Exception, version 1.0, a copy of which can be found at
-- http://oss.oracle.com/licenses/universal-foss-exception.
--
-- This program is distributed in the hope that it will be useful, but
-- WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
-- See the GNU General Public License, version 2.0, for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software Foundation, Inc.,
-- 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA



-- MySQL dump 9.07
--
-- Host: localhost    Database: cts
-- -------------------------------------------------------
-- Server version  4.0.8-gamma-max-nt-log

--
-- Table structure for table 'bb_tab'
--
DROP DATABASE IF EXISTS test;
CREATE DATABASE test;
USE test;


CREATE TABLE BB_tab (
  KEY_ID int(11) NOT NULL default '0',
  BRAND_NAME varchar(32) default NULL,
  PRICE float default NULL,
  PRIMARY KEY  (KEY_ID)
) ENGINE=InnoDB;

--
-- Dumping data for table 'bb_tab'
--


--
-- Table structure for table 'bigint_tab'
--

CREATE TABLE Bigint_Tab (
  MAX_VAL bigint(20) default NULL,
  MIN_VAL bigint(20) default NULL,
  NULL_VAL bigint(20) default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'bigint_Tab'
--


--
-- Table structure for table 'binary_Tab'
--

CREATE TABLE Binary_Tab (
  BINARY_VAL blob
) ENGINE=InnoDB;

--
-- Dumping data for table 'binary_Tab'
--


--
-- Table structure for table 'bit_Tab'
--

CREATE TABLE Bit_Tab (
  MAX_VAL tinyint(1) default NULL,
  MIN_VAL tinyint(1) default NULL,
  NULL_VAL tinyint(1) default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'bit_Tab'
--


--
-- Table structure for table 'char_Tab'
--

CREATE TABLE Char_Tab (
  COFFEE_NAME char(30) default NULL,
  NULL_VAL char(30) default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'char_Tab'
--


--
-- Table structure for table 'connector_Tab'
--

CREATE TABLE Connector_Tab (
  KEY_ID int(11) default NULL,
  PRODUCT_NAME varchar(32) default NULL,
  PRICE float default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'connector_Tab'
--


--
-- Table structure for table 'ctstable1'
--

CREATE TABLE ctstable1 (
  TYPE_ID int(11) NOT NULL default '0',
  TYPE_DESC varchar(32) default NULL,
  PRIMARY KEY  (TYPE_ID)
) ENGINE=InnoDB;

--
-- Dumping data for table 'ctstable1'
--

INSERT INTO ctstable1 VALUES (1,NULL);
INSERT INTO ctstable1 VALUES (2,NULL);
INSERT INTO ctstable1 VALUES (3,NULL);
INSERT INTO ctstable1 VALUES (4,NULL);
INSERT INTO ctstable1 VALUES (5,NULL);
INSERT INTO ctstable1 VALUES (6,NULL);
INSERT INTO ctstable1 VALUES (7,NULL);
INSERT INTO ctstable1 VALUES (8,NULL);
INSERT INTO ctstable1 VALUES (9,NULL);
INSERT INTO ctstable1 VALUES (10,NULL);

--
-- Table structure for table 'ctstable2'
--

CREATE TABLE ctstable2 (
  KEY_ID int(11) NOT NULL default '0',
  COF_NAME varchar(32) default NULL,
  PRICE float default NULL,
  TYPE_ID int(11) default NULL,
  PRIMARY KEY  (KEY_ID),
  KEY TYPE_ID (TYPE_ID),
  FOREIGN KEY (`TYPE_ID`) REFERENCES `ctstable1` (`TYPE_ID`)
) ENGINE=InnoDB;

--
-- Dumping data for table 'ctstable2'
--

INSERT INTO ctstable2 VALUES (2,'Continue-2',2,2);
INSERT INTO ctstable2 VALUES (3,'COFFEE-3',3,2);
INSERT INTO ctstable2 VALUES (4,'COFFEE-4',4,3);
INSERT INTO ctstable2 VALUES (5,'COFFEE-5',5,3);
INSERT INTO ctstable2 VALUES (6,'COFFEE-6',6,3);
INSERT INTO ctstable2 VALUES (7,'COFFEE-7',7,4);
INSERT INTO ctstable2 VALUES (8,'COFFEE-8',8,4);
INSERT INTO ctstable2 VALUES (9,'COFFEE-9',9,4);

--
-- Table structure for table 'ctstable3'
--

CREATE TABLE ctstable3 (
  STRING1 varchar(20) default NULL,
  STRING2 varchar(20) default NULL,
  STRING3 varchar(20) default NULL,
  NUMCOL decimal(10,0) default NULL,
  FLOATCOL float default NULL,
  DATECOL date default NULL,
  TIMECOL time default NULL,
  TSCOL1 datetime default NULL,
  TSCOL2 datetime default NULL
) ENGINE=MyISAM;

--
-- Dumping data for table 'ctstable3'
--


--
-- Table structure for table 'ctstable4'
--

CREATE TABLE ctstable4 (
  STRING4 varchar(20) default NULL,
  NUMCOL decimal(10,0) default NULL
) ENGINE=MyISAM;

--
-- Dumping data for table 'ctstable4'
--


--
-- Table structure for table 'date_Tab'
--

CREATE TABLE Date_Tab (
  MFG_DATE date default NULL,
  NULL_VAL date default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'date_Tab'
--


--
-- Table structure for table 'decimal_Tab'
--

CREATE TABLE Decimal_Tab (
  MAX_VAL decimal(30,15) default NULL,
  MIN_VAL decimal(30,15) default NULL,
  NULL_VAL decimal(30,15) default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'decimal_Tab'
--


--
-- Table structure for table 'deploy_Tab1'
--

CREATE TABLE Deploy_Tab1 (
  KEY_ID int(11) NOT NULL default '0',
  BRAND_NAME varchar(32) default NULL,
  PRICE float default NULL,
  PRIMARY KEY  (KEY_ID)
) ENGINE=InnoDB;

--
-- Dumping data for table 'deploy_Tab1'
--


--
-- Table structure for table 'deploy_Tab2'
--

CREATE TABLE Deploy_Tab2 (
  KEY_ID varchar(100) NOT NULL default '',
  BRAND_NAME varchar(32) default NULL,
  PRICE float default NULL,
  PRIMARY KEY  (KEY_ID)
) ENGINE=InnoDB;

--
-- Dumping data for table 'deploy_Tab2'
--


--
-- Table structure for table 'deploy_Tab3'
--

CREATE TABLE Deploy_Tab3 (
  KEY_ID bigint(20) NOT NULL default '0',
  BRAND_NAME varchar(32) default NULL,
  PRICE float default NULL,
  PRIMARY KEY  (KEY_ID)
) ENGINE=InnoDB;

--
-- Dumping data for table 'deploy_Tab3'
--


--
-- Table structure for table 'deploy_Tab4'
--

CREATE TABLE Deploy_Tab4 (
  KEY_ID float NOT NULL default '0',
  BRAND_NAME varchar(32) default NULL,
  PRICE float default NULL,
  PRIMARY KEY  (KEY_ID)
) ENGINE=InnoDB;

--
-- Dumping data for table 'deploy_Tab4'
--


--
-- Table structure for table 'deploy_Tab5'
--

CREATE TABLE Deploy_Tab5 (
  KEY_ID1 int(11) NOT NULL default '0',
  KEY_ID2 varchar(100) NOT NULL default '',
  KEY_ID3 float NOT NULL default '0',
  BRAND_NAME varchar(32) default NULL,
  PRICE float default NULL,
  PRIMARY KEY  (KEY_ID1,KEY_ID2,KEY_ID3)
) ENGINE=InnoDB;

--
-- Dumping data for table 'deploy_Tab5'
--


--
-- Table structure for table 'double_Tab'
--

CREATE TABLE Double_Tab (
  MAX_VAL double default NULL,
  MIN_VAL double default NULL,
  NULL_VAL double default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'double_Tab'
--


--
-- Table structure for table 'float_Tab'
--

CREATE TABLE Float_Tab (
  MAX_VAL float default NULL,
  MIN_VAL float default NULL,
  NULL_VAL float default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'float_Tab'
--


--
-- Table structure for table 'integer_Tab'
--

CREATE TABLE Integer_Tab (
  MAX_VAL int(11) default NULL,
  MIN_VAL int(11) default NULL,
  NULL_VAL int(11) default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'integer_Tab'
--


--
-- Table structure for table 'integration_sec_Tab'
--

CREATE TABLE Integration_sec_Tab (
  LOG_NO int(11) default NULL,
  LINE_NO int(11) default NULL,
  MESSAGE varchar(255) default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'integration_sec_Tab'
--


--
-- Table structure for table 'integration_Tab'
--

CREATE TABLE Integration_Tab (
  ACCOUNT int(11) NOT NULL default '0',
  BALANCE float default NULL,
  PRIMARY KEY  (ACCOUNT)
) ENGINE=InnoDB;

--
-- Dumping data for table 'integration_Tab'
--


--
-- Table structure for table 'jta_Tab1'
--

CREATE TABLE Jta_Tab1 (
  KEY_ID int(11) default NULL,
  COF_NAME varchar(32) default NULL,
  PRICE float default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'jta_Tab1'
--


--
-- Table structure for table 'jta_Tab2'
--

CREATE TABLE Jta_Tab2 (
  KEY_ID int(11) default NULL,
  CHOC_NAME varchar(32) default NULL,
  PRICE float default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'jta_Tab2'
--


--
-- Table structure for table 'longvarbinary_Tab'
--

CREATE TABLE Longvarbinary_Tab (
  LONGVARBINARY_VAL longblob
) ENGINE=InnoDB;

--
-- Dumping data for table 'longvarbinary_Tab'
--


--
-- Table structure for table 'longvarchar_Tab'
--

CREATE TABLE Longvarchar_Tab (
  COFFEE_NAME mediumtext
) ENGINE=InnoDB;

--
-- Dumping data for table 'longvarchar_Tab'
--

INSERT INTO Longvarchar_Tab VALUES ('1999-12-31 12:59:59');

--
-- Table structure for table 'longvarcharnull_Tab'
--

CREATE TABLE Longvarcharnull_Tab (
  NULL_VAL mediumtext
) ENGINE=InnoDB;

--
-- Dumping data for table 'longvarcharnull_Tab'
--



--
-- Table structure for table 'numeric_Tab'
--

CREATE TABLE Numeric_Tab (
  MAX_VAL decimal(30,15) default NULL,
  MIN_VAL decimal(30,15) default NULL,
  NULL_VAL decimal(30,15) default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'numeric_Tab'
--


--
-- Table structure for table 'real_Tab'
--

CREATE TABLE Real_Tab (
  MAX_VAL double default NULL,
  MIN_VAL double default NULL,
  NULL_VAL double default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'real_Tab'
--


--
-- Table structure for table 'sec_Tab1'
--

CREATE TABLE Sec_Tab1 (
  KEY_ID int(11) NOT NULL default '0',
  PRICE float default NULL,
  BRAND varchar(32) default NULL,
  PRIMARY KEY  (KEY_ID)
) ENGINE=InnoDB;

--
-- Dumping data for table 'sec_Tab1'
--


--
-- Table structure for table 'smallint_Tab'
--

CREATE TABLE Smallint_Tab (
  MAX_VAL smallint(6) default NULL,
  MIN_VAL smallint(6) default NULL,
  NULL_VAL smallint(6) default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'smallint_Tab'
--


--
-- Table structure for table 'time_Tab'
--

CREATE TABLE Time_Tab (
  BRK_TIME time default NULL,
  NULL_VAL time default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'time_Tab'
--


--
-- Table structure for table 'timestamp_Tab'
--

CREATE TABLE Timestamp_Tab (
  IN_TIME datetime default NULL,
  NULL_VAL datetime default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'timestamp_Tab'
--


--
-- Table structure for table 'tinyint_Tab'
--

CREATE TABLE Tinyint_Tab (
  MAX_VAL tinyint(4) default NULL,
  MIN_VAL tinyint(4) default NULL,
  NULL_VAL tinyint(4) default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'tinyint_Tab'
--


--
-- Table structure for table 'txbean_Tab1'
--

CREATE TABLE TXBean_Tab1 (
  KEY_ID int(11) default NULL,
  TABONE_NAME varchar(32) default NULL,
  PRICE float default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'txbean_Tab1'
--


--
-- Table structure for table 'txbean_Tab2'
--

CREATE TABLE TXBean_Tab2 (
  KEY_ID int(11) default NULL,
  TABTWO_NAME varchar(32) default NULL,
  PRICE float default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'txbean_Tab2'
--


--
-- Table structure for table 'txebean_Tab'
--

CREATE TABLE TXEBean_Tab (
  KEY_ID int(11) NOT NULL default '0',
  BRAND_NAME varchar(32) default NULL,
  PRICE float default NULL,
  PRIMARY KEY  (KEY_ID)
) ENGINE=InnoDB;

--
-- Dumping data for table 'txebean_Tab'
--


--
-- Table structure for table 'varbinary_Tab'
--

CREATE TABLE Varbinary_Tab (
  VARBINARY_VAL blob
) ENGINE=InnoDB;

--
-- Dumping data for table 'varbinary_Tab'
--


--
-- Table structure for table 'varchar_Tab'
--

CREATE TABLE Varchar_Tab (
  COFFEE_NAME varchar(30) default NULL,
  NULL_VAL varchar(30) default NULL
) ENGINE=InnoDB;

--
-- Dumping data for table 'varchar_Tab'
--


--
-- Table structure for table 'xa_Tab1'
--

CREATE TABLE XA_Tab1 (
  col1 int(11) NOT NULL default '0',
  col2 varchar(32) default NULL,
  col3 varchar(32) default NULL,
  PRIMARY KEY  (col1)
) ENGINE=InnoDB;

--
-- Dumping data for table 'xa_Tab1'
--


--
-- Table structure for table 'xa_Tab2'
--

CREATE TABLE XA_Tab2 (
  col1 int(11) NOT NULL default '0',
  col2 varchar(32) default NULL,
  col3 varchar(32) default NULL,
  PRIMARY KEY  (col1)
) ENGINE=InnoDB;

--
-- Dumping data for table 'xa_Tab2'
--


