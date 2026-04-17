# ✈️ Arduino → ToLiss A320neo Overhead Light Panel

![Status](https://img.shields.io/badge/Status-Active-green)
![Platform](https://img.shields.io/badge/X--Plane-12-blue)
![Aircraft](https://img.shields.io/badge/Aircraft-A320neo-blue)
![Hardware](https://img.shields.io/badge/Arduino-Compatible-orange)

A custom X-Plane plugin that connects a physical Arduino-based overhead panel to the ToLiss A320neo using serial communication.

This project enables real-world cockpit switches to control aircraft lighting systems with accurate, state-driven behavior using a closed-loop control system.

---

## 🔗 Project Links

### 💻 Plugin Source Code

https://github.com/aryanma11ick/Home_Cockpit_Project/blob/main/software/sim-interface/ovhd_ext_lights_panel/plugin.cpp

### 🔌 Arduino Firmware

https://github.com/aryanma11ick/Home_Cockpit_Project/blob/main/firmware/arduino/ovhd_ext_lights_panel/ovhd_ext_lights.ino

### 🖼️ Panel Images

https://github.com/aryanma11ick/Home_Cockpit_Project/tree/main/docs/images/ovhd_ext_lights_panel

---

## 📸 Hardware Preview

<p float="left">
  <img src="docs/images/ovhd_ext_lights_panel/image_1.jpg" width="45%" />
  <img src="docs/images/ovhd_ext_lights_panel/image_2.jpg" width="45%" />
</p>

<p float="left">
  <img src="docs/images/ovhd_ext_lights_panel/image_3.jpg" width="45%" />
  <img src="docs/images/ovhd_ext_lights_panel/image_4.jpg" width="45%" />
</p>

---

## 🚀 Features

### 🔌 Hardware Integration

* Arduino-based overhead panel
* Multiple switch inputs (3-position & 2-position)
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

* Uses ToLiss animation datarefs for real-time feedback
* Compares hardware state with aircraft state
* Automatically drives switches using Up/Down commands
* Prevents desynchronization between sim and hardware

---

### ⚡ Performance Optimized

* Non-blocking serial communication
* Buffered input parsing
* Low CPU usage
* No FPS drops or simulator lag

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

### Example

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

* Load ToLiss A320neo
* Enable plugin via Plugin Admin

---

## ⚠️ Notes

* RWY Turn-Off uses command-only logic (no feedback dataref)
* Designed specifically for ToLiss Airbus aircraft
* Initial alignment handled via dataref-based feedback loop

---

## 🔧 Future Improvements

* LED feedback to Arduino (panel lighting sync)
* Auto serial reconnect
* Cold & Dark / power state logic
* Expansion to full overhead panel

---

## 👨‍💻 Author

Aryan Mallick
Home cockpit & avionics integration project

---

## 📜 License

MIT License (recommended)
