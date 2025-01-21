# Smart Home Automation System
### Co-Authors are:
#### Kaushal Khadka - https://github.com/Sneaky721 
#### Harikant Sharma - https://github.com/Harikantsharmag

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Hardware and Communication Protocols](#hardware-and-communication-protocols)
- [Software Design](#software-design)
- [Setup](#setup)
- [Usage](#usage)
- [Testing and Performance](#testing-and-performance)
- [Challenges and Future Improvements](#challenges-and-future-improvements)
- [Technologies Used](#technologies-used)
- [License](#license)

---

## Overview
The **Smart Home Automation System** is an IoT-based project that integrates sensors, actuators, and communication protocols to create a smart, user-friendly home monitoring environment. The system uses **Zigbee**, **BLE**, and **Wi-Fi** to monitor and control parameters like temperature, humidity, oxygen levels, and occupancy across various rooms. Real-time data is stored in a **Firebase Realtime Database**, accessible via a web-based dashboard.

This project provides hands-on experience in IoT system design, protocol integration, and energy-efficient communication, making it a practical solution for modern smart home applications.

---

## Features
- **Real-Time Monitoring**: Track and visualize temperature, humidity, and oxygen levels.
- **Room-Specific Control**:
  - Bedroom: Manage lighting and temperature with occupancy detection.
  - Kitchen: Monitor oxygen levels and automate chimney ventilation.
  - Storage Room: Control ventilation based on environmental conditions.
- **Centralized Dashboard**: Web-based interface for real-time data visualization and device control.
- **Firebase Integration**: Cloud storage for sensor data and system commands.
- **Energy Efficiency**: Utilizes Zigbee and BLE for lightweight communication.
- **Remote Control**: Use the dashboard to toggle HVAC systems, lights, and garage doors.

---

## Architecture
The system architecture is based on a centralized **base station** that integrates:
- **Zigbee Network**: For lightweight, low-power communication between rooms.
- **BLE Network**: For high data transfer in HVAC systems and kitchen monitoring.
- **Wi-Fi**: To connect the base station to Firebase and the web interface.

The system uses a **star topology** for Zigbee communication and BLE slaves for room-specific control. Data is uploaded to Firebase and synchronized with the web dashboard.

---

## Hardware and Communication Protocols
### Hardware Components
1. **XBee Modules**:
   - XBee 1 (Coordinator): Manages Zigbee communication.
   - XBee 2: Temperature sensor and fan control.
   - XBee 3: Occupancy sensor and LED light control.
   - XBee 4: Ventilation control.
   - XBee 5: Front door occupancy sensor.
2. **BLE Devices**:
   - BLE Slave 1: HVAC system with temperature and humidity sensors.
   - BLE Slave 2: Oxygen level monitoring and chimney control.
3. **Base Station**:
   - An ESP32 device that integrates Zigbee and BLE networks and uploads data to Firebase.

### Communication Protocols
- **Zigbee**: Star topology for low-power and short-range communication.
- **BLE**: Two-way communication for high data transfer in specific rooms.
- **Wi-Fi**: For connecting the base station to the cloud.

---

## Software Design
The system uses a combination of **ESP32** devices, **Firebase**, and a web-based dashboard:
- **Base Station**: Collects data from BLE and Zigbee devices, processes it, and uploads it to Firebase.
- **Firebase Realtime Database**:
  - Stores sensor data and device states.
  - Synchronizes data between the dashboard and the base station.
- **Web Dashboard**:
  - Built with HTML, CSS, and JavaScript.
  - Features dynamic graphs (via Chart.js) and intuitive controls for managing devices.

---

## Setup
### Prerequisites
- ESP32 boards with BLE and Wi-Fi support.
- XBee S2C modules.
- Firebase account and project setup.
- Node.js environment for development.

### Installation
1. **Hardware Setup**:
   - Connect sensors and actuators to the respective XBee and ESP32 devices.
   - Configure Zigbee devices with appropriate PAN IDs and API settings.
2. **Software Setup**:
   - Clone the repository:
     ```bash
     git clone https://github.com/your-repo/smart-home-system.git
     cd smart-home-system
     ```
   - Replace `firebaseConfig` in `script.js` with your Firebase project credentials.
3. **Run the Web Dashboard**:
   - Open `index.html` in a browser or host it on a local server.

---

## Usage
1. Power on the devices and ensure all sensors and actuators are properly connected.
2. Open the web dashboard:
   - Home tab: Monitor overall home parameters.
   - Bedroom tab: Adjust lighting and monitor temperature.
   - Kitchen tab: Monitor oxygen levels and control the chimney.
3. Use control buttons to toggle devices like HVAC systems and garage doors.

---

## Testing and Performance
### Testing Summary
- **BLE Communication**: Achieved a latency of ~50ms for sensor updates.
- **Firebase Synchronization**: Commands executed with a delay of 2-3 seconds.
- **Dashboard**: Reliable real-time updates and control functionality.

### Limitations
- Memory constraints on the ESP32 prevented simultaneous operation of Zigbee and BLE modules.
- Zigbee communication was limited to one-way data flow in some configurations.

---

## Challenges and Future Improvements
### Challenges
- Memory limitations on the ESP32 base station.
- Reliability issues with Zigbee communication in the current star topology.

### Future Improvements
1. **Mesh Network for Zigbee**: Enhance scalability and fault tolerance.
2. **Expanded Sensor Array**: Add air quality, noise, and gas sensors.
3. **Advanced Analytics**: Integrate machine learning for predictive maintenance and energy optimization.
4. **Voice Control**: Add compatibility with Alexa or Google Assistant.
5. **Enhanced Security**: Implement robust encryption and authentication mechanisms.

---

## Technologies Used
- **Frontend**: HTML, CSS, JavaScript, Chart.js.
- **Backend**: Firebase Realtime Database.
- **Hardware**: ESP32, XBee S2C, TMP36 sensors.
- **Communication Protocols**: Zigbee, BLE, Wi-Fi.

---

## License
This project is licensed under the [MIT License](LICENSE).

Feel free to contribute by submitting pull requests or reporting issues!

