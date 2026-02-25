#  Interface Fingerprint System (Arduino)

A fingerprint-based authentication system built using Arduino.  
This project allows secure access control using a fingerprint sensor module.

---

##  Project Description

This project uses a fingerprint sensor to:
- Enroll new fingerprints
- Store fingerprint data
- Match fingerprints for authentication
- Grant or deny access based on verification

It can be used for:
- Attendance systems
- Door lock security
- Access control systems
- IoT-based authentication projects

---

##  Hardware Requirements

- Arduino Board (UNO / ESP32 / etc.)
- Fingerprint Sensor Module (e.g., R305 / R307)
- Jumper Wires
- Breadboard
- Power Supply

---

##  Connections

| Fingerprint Sensor | Arduino |
|--------------------|----------|
| VCC                | 5V / 3.3V |
| GND                | GND |
| TX                 | RX |
| RX                 | TX |

> ⚠ Make sure RX-TX are cross connected.

---

##  Software Requirements

- Arduino IDE
- Required fingerprint library (e.g., Adafruit Fingerprint Library)

---
##  How to Run

1. Open the `.ino` file in Arduino IDE.
2. Select correct Board & Port.
3. Upload the code to Arduino.
4. Oopen Serial Monitor.
5. Fllow on-screen instructions for enrolling and verifying fingerprints.

---

