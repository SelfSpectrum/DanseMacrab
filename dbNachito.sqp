-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- USE DATABASE mydb;

-- -----------------------------------------------------
-- Schema mydb
-- -----------------------------------------------------
SHOW WARNINGS;

-- -----------------------------------------------------
-- Table `paciente`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `paciente` ;

SHOW WARNINGS;
CREATE TABLE IF NOT EXISTS `paciente` (
  `id` INT NOT NULL,
  `nombre` VARCHAR(50) NOT NULL,
  `alergia` VARCHAR(200) NOT NULL,
  `fechaNacimiento` DATE NOT NULL,
  `dieta` VARCHAR(400) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE INDEX `id_UNIQUE` (`id` ASC) VISIBLE)
ENGINE = MariaDB;

SHOW WARNINGS;

-- -----------------------------------------------------
-- Table `doctor`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `doctor` ;

SHOW WARNINGS;
CREATE TABLE IF NOT EXISTS `doctor` (
  `idDoctor` INT NOT NULL,
  `cargo` VARCHAR(50) NOT NULL,
  `nombre` VARCHAR(50) NOT NULL,
  PRIMARY KEY (`idDoctor`))
ENGINE = MariaDB;

SHOW WARNINGS;

-- -----------------------------------------------------
-- Table `examen`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `examen` ;

SHOW WARNINGS;
CREATE TABLE IF NOT EXISTS `examen` (
  `idExamen` INT NOT NULL,
  `tipo` VARCHAR(45) NOT NULL,
  `fecha` DATE NOT NULL,
  `resultado` VARCHAR(100) NULL,
  `diagnostico` VARCHAR(100) NULL,
  `paciente_id` INT NOT NULL,
  `doctor_idDoctor` INT NOT NULL,
  PRIMARY KEY (`idExamen`),
  INDEX `fk_examen_paciente_idx` (`paciente_id` ASC) VISIBLE,
  INDEX `fk_examen_doctor1_idx` (`doctor_idDoctor` ASC) VISIBLE,
  CONSTRAINT `fk_examen_paciente`
    FOREIGN KEY (`paciente_id`)
    REFERENCES `paciente` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_examen_doctor1`
    FOREIGN KEY (`doctor_idDoctor`)
    REFERENCES `doctor` (`idDoctor`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MariaDB;

SHOW WARNINGS;

-- -----------------------------------------------------
-- Table `dispositivo`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `dispositivo` ;

SHOW WARNINGS;
CREATE TABLE IF NOT EXISTS `dispositivo` (
  `codigo` INT NOT NULL,
  `modelo` VARCHAR(45) NOT NULL,
  `marca` VARCHAR(100) NOT NULL,
  `tipo` VARCHAR(100) NOT NULL,
  `doctor_idDoctor` INT NOT NULL,
  PRIMARY KEY (`codigo`),
  INDEX `fk_dispositivo_doctor1_idx` (`doctor_idDoctor` ASC) VISIBLE,
  CONSTRAINT `fk_dispositivo_doctor1`
    FOREIGN KEY (`doctor_idDoctor`)
    REFERENCES `doctor` (`idDoctor`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MariaDB;

SHOW WARNINGS;

-- -----------------------------------------------------
-- Table `organizacion`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `organizacion` ;

SHOW WARNINGS;
CREATE TABLE IF NOT EXISTS `organizacion` (
  `idOrganizacion` INT NOT NULL,
  `nombre` VARCHAR(50) NOT NULL,
  `ubicacion` VARCHAR(100) NOT NULL,
  `tipo` VARCHAR(60) NOT NULL,
  `doctor_idDoctor` INT NOT NULL,
  PRIMARY KEY (`idOrganizacion`),
  INDEX `fk_organizacion_doctor1_idx` (`doctor_idDoctor` ASC) VISIBLE,
  CONSTRAINT `fk_organizacion_doctor1`
    FOREIGN KEY (`doctor_idDoctor`)
    REFERENCES `doctor` (`idDoctor`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MariaDB;

SHOW WARNINGS;

-- -----------------------------------------------------
-- Table `aplicacion`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `aplicacion` ;

SHOW WARNINGS;
CREATE TABLE IF NOT EXISTS `aplicacion` (
  `idAplicacion` INT NOT NULL,
  `nombre` VARCHAR(50) NOT NULL,
  `version` VARCHAR(100) NOT NULL,
  `funcion` VARCHAR(50) NOT NULL,
  PRIMARY KEY (`idAplicacion`))
ENGINE = MariaDB;

SHOW WARNINGS;

-- -----------------------------------------------------
-- Table `datosMedicos`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `datosMedicos` ;

SHOW WARNINGS;
CREATE TABLE IF NOT EXISTS `datosMedicos` (
  `idDatomedico` INT NOT NULL,
  `tipo` VARCHAR(45) NOT NULL,
  `fecha` VARCHAR(45) NOT NULL,
  `info` VARCHAR(45) NOT NULL,
  `datosMedicoscol` VARCHAR(60) NOT NULL,
  PRIMARY KEY (`idDatomedico`))
ENGINE = MariaDB;

SHOW WARNINGS;

-- -----------------------------------------------------
-- Table `organizacion`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `organizacion` ;

SHOW WARNINGS;
CREATE TABLE IF NOT EXISTS `organizacion` (
  `idOrganizacion` INT NOT NULL,
  `nombre` VARCHAR(50) NOT NULL,
  `ubicacion` VARCHAR(100) NOT NULL,
  `tipo` VARCHAR(60) NOT NULL,
  `doctor_idDoctor` INT NOT NULL,
  PRIMARY KEY (`idOrganizacion`),
  INDEX `fk_organizacion_doctor1_idx` (`doctor_idDoctor` ASC) VISIBLE,
  CONSTRAINT `fk_organizacion_doctor1`
    FOREIGN KEY (`doctor_idDoctor`)
    REFERENCES `doctor` (`idDoctor`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MariaDB;

SHOW WARNINGS;

-- -----------------------------------------------------
-- Table `gestion`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `gestion` ;

SHOW WARNINGS;
CREATE TABLE IF NOT EXISTS `gestion` (
  `dispositivo_codigo` INT NOT NULL,
  `aplicacion_idAplicacion` INT NOT NULL,
  PRIMARY KEY (`dispositivo_codigo`, `aplicacion_idAplicacion`),
  INDEX `fk_dispositivo_has_aplicacion_aplicacion1_idx` (`aplicacion_idAplicacion` ASC) VISIBLE,
  INDEX `fk_dispositivo_has_aplicacion_dispositivo1_idx` (`dispositivo_codigo` ASC) VISIBLE,
  CONSTRAINT `fk_dispositivo_has_aplicacion_dispositivo1`
    FOREIGN KEY (`dispositivo_codigo`)
    REFERENCES `dispositivo` (`codigo`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_dispositivo_has_aplicacion_aplicacion1`
    FOREIGN KEY (`aplicacion_idAplicacion`)
    REFERENCES `aplicacion` (`idAplicacion`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MariaDB;

SHOW WARNINGS;

-- -----------------------------------------------------
-- Table `manipulacion`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `manipulacion` ;

SHOW WARNINGS;
CREATE TABLE IF NOT EXISTS `manipulacion` (
  `aplicacion_idAplicacion` INT NOT NULL,
  `datosMedicos_idDatomedico` INT NOT NULL,
  PRIMARY KEY (`aplicacion_idAplicacion`, `datosMedicos_idDatomedico`),
  INDEX `fk_aplicacion_has_datosMedicos_datosMedicos1_idx` (`datosMedicos_idDatomedico` ASC) VISIBLE,
  INDEX `fk_aplicacion_has_datosMedicos_aplicacion1_idx` (`aplicacion_idAplicacion` ASC) VISIBLE,
  CONSTRAINT `fk_aplicacion_has_datosMedicos_aplicacion1`
    FOREIGN KEY (`aplicacion_idAplicacion`)
    REFERENCES `aplicacion` (`idAplicacion`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_aplicacion_has_datosMedicos_datosMedicos1`
    FOREIGN KEY (`datosMedicos_idDatomedico`)
    REFERENCES `datosMedicos` (`idDatomedico`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = MariaDB;

SHOW WARNINGS;

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
