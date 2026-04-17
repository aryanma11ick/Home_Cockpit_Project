# ✈️ Arduino → ToLiss A320Neo Overhead Light Panel

A custom X-Plane plugin that connects a physical Arduino-based overhead panel to the ToLiss A320Neo aircraft using serial communication.

This project enables real-world cockpit switches to control aircraft lighting systems with accurate, state-driven behavior.

---

## 🚀 Features

### 🔌 Hardware Integration

* Arduino-based input system
* Supports multi-switch overhead panels
* Serial communication (115200 baud)

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

* Reads aircraft state using ToLiss animation datarefs
* Compares hardware position vs simulator state
* Automatically drives switches using Up/Down commands
* Prevents desynchronization between sim and hardware

---

### ⚡ Performance Optimized

* Non-blocking serial communication
* Buffered input parsing
* Minimal CPU usage
* No FPS drops or sim freezes

---

## 🧠 Architecture

```text
Arduino → Serial → Plugin → Commands → Aircraft
                                ↑
                          DataRefs (feedback)
```

### Flow:

1. Arduino sends switch position (e.g. `N:2`)
2. Plugin parses input
3. Plugin reads current aircraft state (DataRef)
4. Plugin issues commands until states match

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

### Commands (ToLiss)

Uses:

```
toliss_airbus/lightcommands/*
```

### DataRefs (State Feedback)

Uses animation refs:

```
ckpt/oh/.../anim
```

---

## 🛠️ Setup

### 1. Build Plugin

* Open project in Visual Studio
* Build as **64-bit DLL**
* Rename output:

```
win.xpl
```

---

### 2. Install Plugin

Place in:

```
X-Plane 12/Resources/plugins/YourPluginName/64/win.xpl
```

---

### 3. Arduino Setup

* Upload Arduino code
* Set baud rate to **115200**
* Ensure correct COM port in plugin

---

### 4. Run X-Plane

* Load ToLiss A333
* Enable plugin via Plugin Admin

---

## ⚠️ Notes

* RWY Turn-Off uses direct commands (no feedback dataref)
* First-time sync depends on aircraft state
* Requires ToLiss A333 (not compatible with Laminar A333)

---

## 🔧 Future Improvements

* LED feedback to Arduino
* Auto serial reconnect
* Power state logic (cold & dark)
* Full overhead panel expansion

---

## 👨‍💻 Author

Aryan Mallick
Custom home cockpit + avionics integration project

---
