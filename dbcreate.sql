-- ****************** SqlDBM: MySQL ******************;
-- ***************************************************;

-- DROP TABLE `alert_data`;


-- DROP TABLE `daily_flow_data`;


-- DROP TABLE `hour_flow_data`;


-- DROP TABLE `minute_flow_data`;



-- ************************************** `alert_data`

CREATE TABLE `alert_data`
(
 `id`        INTEGER NOT NULL PRIMARY KEY ,
 `device`    TEXT NOT NULL ,
 `timestamp` TIMESTAMP NOT NULL ,
 `message`   TEXT NOT NULL
);





-- ************************************** `daily_flow_data`

CREATE TABLE `daily_flow_data`
(
 `id`     INTEGER NOT NULL PRIMARY KEY ,
 `device` TEXT NOT NULL ,
 `day`    TIMESTAMP NOT NULL ,
 `flow`   INTEGER NOT NULL
);





-- ************************************** `hour_flow_data`

CREATE TABLE `hour_flow_data`
(
 `id`     INTEGER NOT NULL PRIMARY KEY ,
 `device` TEXT NOT NULL ,
 `hour`   TIMESTAMP NOT NULL ,
 `flow`   INTEGER NOT NULL
);





-- ************************************** `minute_flow_data`

CREATE TABLE `minute_flow_data`
(
 `id`     INTEGER NOT NULL PRIMARY KEY ,
 `device` TEXT NOT NULL ,
 `minute` TIMESTAMP NOT NULL ,
 `flow`   INTEGER NOT NULL
);
