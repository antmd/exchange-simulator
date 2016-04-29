DROP DATABASE IF EXISTS ExchangeSimulator;
CREATE DATABASE ExchangeSimulator;

CREATE TABLE `ExchangeSimulator`.`OrderBook` (
    `id` INT NOT NULL AUTO_INCREMENT,
    `symbol` TEXT NOT NULL ,
    `client` TEXT NOT NULL ,
    `client_order_id` TEXT NOT NULL,
    `side` CHAR(1) NOT NULL ,
    `price` INT NOT NULL ,
    `quantity` INT NOT NULL ,
    `time` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (`id`)
    ) ENGINE = InnoDB;

CREATE TABLE `ExchangeSimulator`.`SecurityStatus` (
    `symbol` TEXT NOT NULL ,
    `price` INT NOT NULL ,
    `prev_close` INT NOT NULL ,
    `mkt_cap` BIGINT NOT NULL,
    `lock` BOOLEAN NOT NULL DEFAULT FALSE,
    PRIMARY KEY (`symbol`(8))
    ) ENGINE = InnoDB;


SOURCE procedures.sql