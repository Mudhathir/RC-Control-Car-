# RC Car with IoT Integration

RC Car with IoT Integration is an IoT-enabled remote-controlled car project that provides real-time telemetry and remote navigation via a custom Swift-based iOS app. This project integrates both hardware and software components to allow users to control the RC car wirelessly using Bluetooth, while also monitoring sensor data and performance in real time.

## Overview

This project comprises two main components:

1. **Hardware Component:**  
   - An RC car modified with embedded electronics.
   - Sensor integration using Analog-to-Digital Conversion (ADC) for telemetry.
   - Precise motor control implemented via Pulse-Width Modulation (PWM).
   - A Bluetooth module for enabling wireless communication.

2. **Software Component (iOS App):**  
   - An iOS application built in Swift/SwiftUI that connects with the RC car over Bluetooth Low Energy (BLE).
   - A user interface that allows remote control (directions, speed adjustments, etc.) and displays real-time telemetry.
   - BLE handling using CoreBluetooth to discover, connect, and communicate with the RC car.

## Features

- **Remote Navigation:**  
  Control the RC car's movement using an intuitive touch interface.
  
- **Bluetooth Connectivity:**  
  Utilize BLE for secure and low-latency communication between the RC car and the iOS app.
  
- **Real-Time Telemetry:**  
  Monitor live sensor data captured from the RC car’s onboard ADC.
  
- **Precise Motor Control:**  
  Utilize PWM to adjust motor speeds and ensure smooth control of the vehicle.
  
- **IoT Integration:**  
  Bridge the gap between embedded systems and mobile development to create a connected, interactive vehicle.

## Technologies Used

- **Embedded Systems / IoT:**  
  - C for low-level firmware controlling ADC and PWM.
  - Custom hardware circuit design and sensor integration.

- **Bluetooth Low Energy (BLE):**  
  - For wireless communication between the RC car and the iOS app.

- **iOS Development:**  
  - Swift and SwiftUI for building the user interface.
  - CoreBluetooth framework for managing BLE connectivity.

## Setup and Installation

### Hardware Setup

1. **RC Car Modification:**  
   - Modify your RC car to integrate sensor modules for telemetry and motor controllers using ADC and PWM respectively.
   - Install a compatible Bluetooth module configured for BLE communication.
   - Ensure that the power supply and wiring meet safety and performance requirements.

2. **Firmware Programming:**  
   - Use your preferred embedded development environment to program the microcontroller.
   - Load the firmware that configures sensor data acquisition and precise motor control via PWM.
   - Test the hardware independently before full integration with the iOS app.




