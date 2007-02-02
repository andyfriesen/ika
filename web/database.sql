-- MySQL dump 8.23
--
-- Host: mysql4-i    Database: i38780_ika
---------------------------------------------------------
-- Server version	4.1.21-standard-log

--
-- Table structure for table `articles`
--

CREATE TABLE articles (
  title varchar(80) default NULL,
  author varchar(80) default NULL,
  description text,
  `date` date default NULL,
  queued tinyint(1) default '1',
  `text` text,
  id int(11) NOT NULL auto_increment,
  PRIMARY KEY  (id)
) ENGINE=MyISAM AUTO_INCREMENT=18 DEFAULT CHARSET=latin1 PACK_KEYS=1;

--
-- Table structure for table `board`
--

CREATE TABLE board (
  id int(11) NOT NULL auto_increment,
  parentid int(11) default '0',
  `subject` varchar(80) default NULL,
  name varchar(80) default NULL,
  `text` text,
  `date` date default NULL,
  `time` time default NULL,
  ip varchar(80) default NULL,
  deleted tinyint(1) NOT NULL default '0',
  locked tinyint(1) default '0',
  PRIMARY KEY  (id),
  INDEX(parentid, deleted)
) ENGINE=MyISAM AUTO_INCREMENT=3808 DEFAULT CHARSET=latin1 PACK_KEYS=1;

--
-- Table structure for table `files`
--

CREATE TABLE files (
  id int(11) NOT NULL auto_increment,
  filename varchar(80) default NULL,
  name varchar(80) default NULL,
  author varchar(80) default NULL,
  description text,
  size int(11) default '0',
  category int(11) default '0',
  queued tinyint(1) default '1',
  `date` date default '2001-01-01',
  PRIMARY KEY  (id)
) ENGINE=MyISAM AUTO_INCREMENT=303 DEFAULT CHARSET=latin1 PACK_KEYS=1;

--
-- Table structure for table `gallery`
--

CREATE TABLE gallery (
  id int(11) NOT NULL auto_increment,
  title varchar(80) default NULL,
  name varchar(80) default NULL,
  `date` date NOT NULL default '0000-00-00',
  filename varchar(80) default NULL,
  description text,
  PRIMARY KEY  (id)
) ENGINE=MyISAM AUTO_INCREMENT=58 DEFAULT CHARSET=latin1;

--
-- Table structure for table `links`
--

CREATE TABLE links (
  id int(11) NOT NULL auto_increment,
  url varchar(80) default NULL,
  name varchar(80) default NULL,
  description text,
  queued tinyint(1) default '1',
  category int(11) default '0',
  PRIMARY KEY  (id)
) ENGINE=MyISAM AUTO_INCREMENT=39 DEFAULT CHARSET=latin1 PACK_KEYS=1;

--
-- Table structure for table `news`
--

CREATE TABLE news (
  id int(11) NOT NULL auto_increment,
  `subject` varchar(80) default NULL,
  name varchar(80) default NULL,
  `text` text,
  `date` date default NULL,
  `time` time default NULL,
  PRIMARY KEY  (id)
) ENGINE=MyISAM AUTO_INCREMENT=141 DEFAULT CHARSET=latin1 PACK_KEYS=1;

--
-- Table structure for table `users`
--

CREATE TABLE users (
  id int(11) NOT NULL auto_increment,
  name varchar(80) default NULL,
  email varchar(80) default NULL,
  passwd varchar(40) NOT NULL default '',
  signature text,
  admin tinyint(1) default '0',
  layout varchar(80) default 'default',
  PRIMARY KEY  (id)
) ENGINE=MyISAM AUTO_INCREMENT=280 DEFAULT CHARSET=latin1 PACK_KEYS=1;

