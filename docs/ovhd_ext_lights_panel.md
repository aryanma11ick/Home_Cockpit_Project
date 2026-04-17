# ✈️ Arduino → ToLiss A20Neo Overhead Light Panel

A custom X-Plane plugin that connects a physical Arduino-based overhead panel to the ToLiss A333 aircraft using serial communication.

This project enables real-world cockpit switches to control aircraft lighting systems with accurate, state-driven behavior.

---

## 🔗 Project Links

### 💻 Plugin Source Code

* https://github.com/aryanma11ick/Home_Cockpit_Project/blob/main/software/sim-interface/ovhd_ext_lights_panel/plugin.cpp

### 🔌 Arduino Firmware

* https://github.com/aryanma11ick/Home_Cockpit_Project/blob/main/firmware/arduino/ovhd_ext_lights_panel/ovhd_ext_lights.ino

### 🖼️ Panel Images

* https://github.com/aryanma11ick/Home_Cockpit_Project/tree/main/docs/images/ovhd_ext_lights_panel

---

## 📸 Hardware Preview

> *(Add images below once uploaded in repo)*

```md
![Panel](docs/images/ovhd_ext_lights_panel/image_1.jpg)
![Wiring](docs/images/ovhd_ext_lights_panel/image_2.jpg)
![Panel](docs/images/ovhd_ext_lights_panel/image_3.jpg)
![Wiring](docs/images/ovhd_ext_lights_panel/image_4.jpg)
```

---

## 🚀 Features

### 🔌 Hardware Integration

* Arduino-based input system
* Multi-switch overhead panel support
* Serial communication @ 115200 baud

---

### 💡 Supported Systems

#### 🔺 3-Position Switches

* Strobe Lights
* Nav/Logo Lights
* Landing Lights (Left & Right independent)
* Nose Light (Taxi / Takeoff / Off)

#### 🔻 2-Position Switches

* Beacon Light
* Wing Light
* RWY Turn-Off Light

---

### 🔁 Closed-Loop Control System

* Uses ToLiss animation datarefs for feedback
* Compares hardware vs aircraft state
* Automatically steps switches using Up/Down commands
* Eliminates desynchronization

---

### ⚡ Performance Optimized

* Non-blocking serial communication
* Buffered input parsing
* Low CPU usage
* No FPS drops / sim lag

---

## 🧠 Architecture

```text
Arduino → Serial → Plugin → Commands → Aircraft
                                ↑
                          DataRefs (feedback)
```

---

## 📡 Serial Protocol

| Code | System        | Values    |
| ---- | ------------- | --------- |
| S    | Strobe        | 0 / 1 / 2 |
| N    | Nav/Logo      | 0 / 1 / 2 |
| LL   | Left Landing  | 0 / 1 / 2 |
| RL   | Right Landing | 0 / 1 / 2 |
| O    | Nose Light    | 0 / 1 / 2 |
| B    | Beacon        | 0 / 1     |
| W    | Wing          | 0 / 1     |
| R    | RWY Turn Off  | 0 / 1     |

Example:

```
S:2
N:1
LL:2
RL:0
O:2
B:1
W:0
R:1
```

---

## 🧩 X-Plane Integration

### ✈️ Commands (Control)

```
toliss_airbus/lightcommands/*
```

### 📊 DataRefs (State Feedback)

```
ckpt/oh/.../anim
```

---

## 🛠️ Setup

### 1. Build Plugin

* Open project in Visual Studio
* Build as **64-bit**
* Rename output:

```
win.xpl
```

---

### 2. Install Plugin

```
X-Plane 12/Resources/plugins/ArduinoSerialPlugin/64/win.xpl
```

---

### 3. Arduino Setup

* Upload firmware
* Set baud rate: **115200**
* Match COM port in plugin

---

### 4. Run

* Load ToLiss A320Neo
* Enable plugin via Plugin Admin

---

## ⚠️ Notes

* RWY Turn-Off uses command-only (no dataref feedback)
* Requires ToLiss aircraft (not Laminar compatible)
* Initial state sync handled via animation datarefs

---

## 🔧 Future Improvements

* LED feedback to Arduino
* Auto serial reconnect
* Cold & Dark power logic
* Full overhead expansion

---

## 👨‍💻 Author

Aryan Mallick
Home cockpit & avionics integration project

---
