# CAN-Based Motor Control System using STM32, ESP32 & MCP2515

## Project Overview

This project implements a real-time CAN (Controller Area Network) based motor control system using STM32 and ESP32 microcontrollers interfaced with the MCP2515 CAN controller via SPI communication.

The ESP32 acts as the CAN transmitter node and continuously reads analog data from a potentiometer. The STM32 acts as the receiver node and controls DC motor speed using PWM generated from TIM1 based on the received CAN data.

The project demonstrates:
- CAN bus communication
- SPI interfacing with MCP2515
- Bare-metal embedded programming
- PWM motor speed control
- Real-time embedded system design

---

## Hardware Components

- STM32F401RE
- ESP32 Dev Module
- MCP2515 CAN Controller Module
- DC Motor
- Potentiometer
- Breadboard
- Jumper Wires
- Power Supply

---

## Software Tools

- STM32CubeIDE
- Arduino IDE
- Embedded C
- FreeRTOS
- MCP2515 CAN Library

---

## Features

- Real-time CAN communication
- CAN bitrate configured at 125 kbps
- Bare-metal SPI driver implementation
- PWM motor control using TIM1
- ADC-based speed control
- UART debugging support
- Reliable CAN frame transmission
- Low-level register programming

---

## System Architecture

ESP32 (ADC Input)
        |
        | CAN Bus via MCP2515
        |
STM32 (PWM Motor Controller)
        |
        ---> DC Motor

---

## Working Principle

1. ESP32 reads analog value from potentiometer.
2. ADC value is converted into CAN data frame.
3. MCP2515 transmits CAN message over CAN bus.
4. STM32 receives CAN frame using MCP2515.
5. STM32 extracts ADC value from CAN frame.
6. PWM duty cycle is updated according to received value.
7. Motor speed changes dynamically.

---

## CAN Configuration

| Parameter | Value |
|----------|-------|
| CAN ID | 0x123 |
| Bitrate | 125 kbps |
| Data Length | 2 Bytes |
| Communication Type | Standard CAN Frame |

---

## PWM Speed Mapping

| ADC Range | PWM Duty Cycle |
|-----------|----------------|
| 0 - 1023 | 25% |
| 1024 - 2047 | 50% |
| 2048 - 3071 | 75% |
| 3072 - 4095 | 100% |

---

## Embedded Concepts Used

- CAN Protocol
- SPI Communication
- UART Communication
- PWM Generation
- Bare-metal Programming
- Register-Level Programming
- Embedded Drivers
- Real-Time Communication

---

## Project Output

- Successful CAN communication established between ESP32 and STM32
- Real-time motor speed control achieved
- Stable CAN data transfer with minimal errors
- PWM output dynamically updated from CAN data



