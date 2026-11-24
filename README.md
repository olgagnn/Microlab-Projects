# Microcomputers Laboratory – NTUA

This repository contains laboratory assignments and embedded systems implementations developed as part of the **Microcomputers Laboratory** course (7th Semester, Track Y – Computer Systems) at the **School of Electrical and Computer Engineering, National Technical University of Athens (NTUA)**.

The laboratory focused on hands-on **Embedded Systems and Microcontroller Programming**, combining low-level software development, hardware interfacing, peripheral communication and IoT applications.

## Overview

The assignments involved programming the **ATmega328PB microcontroller** using **C and AVR Assembly**, working directly with hardware registers, configuring peripherals, handling interrupts and developing applications on the ntuAboard development platform using MicroLab IDE.

Practical work included interfacing with temperature and pressure sensors, controlling LED displays, implementing serial communication protocols and developing embedded applications.

## Main Topics & Implementations

- **Microcontroller Programming:** Embedded C and AVR Assembly programming.
- **Timers & Interrupts:** Timer configuration, overflow interrupts, external interrupts and time-based operations.
- **Serial Communication:** UART/USART configuration and asynchronous serial data transmission and reception.
- **Hardware Interfacing:** Interfacing microcontrollers with external components and peripherals.
- **Sensor Integration:** Working with temperature and pressure sensors and processing sensor data.
- **GPIO & Peripheral Control:** Digital input/output operations, switches, LEDs and LED displays.
- **Embedded & IoT Applications:** Developing connected embedded systems integrating sensors, wireless communication and remote monitoring.

## Final Project – Hospital Patient Monitoring IoT System

The final laboratory project involved developing an IoT-based hospital patient monitoring application, simulating a connected healthcare environment.

The system followed a distributed architecture in which each hospital bed represented an independent IoT node responsible for collecting patient-related sensor measurements and transmitting them to a central Gateway.

### System Architecture

- **IoT Node (ntuAboard / ATmega328PB):** Programmed embedded device responsible for sensor data acquisition and communication.
- **Sensors:** DS18B20 digital temperature sensor and a potentiometer simulating a Central Venous Pressure (CVP) sensor (0–20 cm H₂O).
- **Wi-Fi Communication (ESP8266):** Wireless connectivity using UART-based serial commands to transmit sensor measurements.
- **Gateway:** Central component responsible for receiving and processing data from IoT nodes, as well as sending responses and commands back to devices.
- **Monitoring Interface:** Centralized dashboard displaying patient monitoring data for hospital medical and nursing staff.

The project provided practical experience in integrating embedded hardware, sensor data acquisition, serial communication and wireless IoT connectivity into a distributed monitoring application.

## Hardware & Development Tools

| Component | Description |
|---|---|
| Microcontroller | ATmega328PB |
| Development Board | ntuAboard |
| IDE | MicroLab IDE |
| Programming Languages | C, AVR Assembly |
| Wi-Fi Module | ESP8266 |
| Sensors | DS18B20, Potentiometer (CVP simulation) |
| Peripherals | LEDs, LED Displays, Switches |
| Communication | UART / USART, Wi-Fi |

## Skills & Concepts

- Embedded C & AVR Assembly Programming
- AVR Microcontroller Architecture
- Hardware Register-Level Programming
- Interrupt Service Routines (ISRs)
- Timer/Counter Configuration
- UART Serial Communication
- GPIO & Hardware Interfacing
- Sensor Integration & Data Acquisition
- ESP8266 Wireless Communication
- IoT System Architecture
- Embedded Systems Development & Debugging

## Repository Structure

The repository is organized by laboratory assignments and includes the corresponding source code implementations and laboratory reports.

---

**Course:** Microcomputers Laboratory  
**Institution:** National Technical University of Athens (NTUA)  
**Department:** Electrical and Computer Engineering  
**Semester:** 7th  
**Track:** Y – Computer Systems
