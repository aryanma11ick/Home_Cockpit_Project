# 🛫 Home Cockpit System

A custom-built **modular home cockpit system** for flight simulation, focused on replicating real aircraft controls using **PCB design, CAD modeling, and embedded systems**.

This project combines **hardware engineering + software integration** to create a scalable and realistic cockpit environment.

---

## ✈️ Overview

The goal of this project is to design a **high-fidelity, modular cockpit system** that closely mimics real aircraft panels while remaining flexible for expansion.

Key design principles:

* 🔧 **Modularity** – Panels can be developed and used independently
* 🧩 **Scalability** – Easy to expand into a full cockpit
* 🎯 **Realism** – Accurate switch behavior and layout
* ⚡ **Integration** – Seamless connection with flight simulators

---

## 🧱 Project Structure

```text
hardware/
├── pcb/          # KiCad design files + Gerbers
└── cad/          # STEP/STL files for panels and components

firmware/         # Microcontroller code (Arduino / ESP32)

software/         # Simulator interface plugins (X-Plane, etc.)

docs/             # Documentation, diagrams, and images
```

---

## 🔧 Modules

### 🟢 Overhead External Lights Panel

* Fully functional external lighting control panel
* Supports multiple 3-position and 2-position switches
* Integrated with simulator via custom plugin
* Designed for real-time hardware ↔ simulator synchronization

---

### 🟡 Annunciator (Alert) Panel

* Displays aircraft alerts and system states
* Uses custom illuminated switches
* Modular layout for future expansion

---

### 🔘 Korry Switch Module (v2)

* Custom PCB designed in KiCad
* LED backlighting support
* Reusable module for multiple panels
* Includes CAD models for enclosure and mounting

---

## ⚙️ Technologies Used

### 🧠 Hardware

* **KiCad** – PCB design and layout
* **Fusion 360 / CAD tools** – Mechanical design and panel modeling

### 💻 Software

* **C++ (X-Plane SDK)** – Simulator interface plugins
* **Arduino / ESP32** – Firmware and hardware control

### 🔗 Tools

* **GitHub** – Version control and project management

---

## 🔁 System Architecture

```text
[Physical Switches] → [Microcontroller] → [Serial Communication]
                                  ↓
                            [Plugin / Interface]
                                  ↓
                             [Flight Simulator]
                                  ↑
                          [DataRef Feedback Loop]
```

* Closed-loop system ensures **hardware and simulator stay synchronized**
* Supports **real-time control and feedback**

---

## 🚧 Project Status

**Status:** 🟢 Active Development

### ✅ Completed

* Korry Switch PCB (v2)
* Initial CAD panel designs
* Overhead external lights panel (hardware + integration)

### 🔄 In Progress

* Firmware refinement
* Panel integration and wiring
* Simulator interface improvements

---

## 📸 Media

### Hardware Preview

<p align="center">
  <img src="https://raw.githubusercontent.com/aryanma11ick/Home_Cockpit_Project/main/docs/images/ovhd_ext_lights_panel/image_1.jpeg" width="45%" />
  <img src="https://raw.githubusercontent.com/aryanma11ick/Home_Cockpit_Project/main/docs/images/ovhd_ext_lights_panel/image_2.jpeg" width="45%" />
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/aryanma11ick/Home_Cockpit_Project/main/docs/images/ovhd_ext_lights_panel/image_3.jpeg" width="45%" />
  <img src="https://raw.githubusercontent.com/aryanma11ick/Home_Cockpit_Project/main/docs/images/ovhd_ext_lights_panel/image_4.jpeg" width="45%" />
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/aryanma11ick/Home_Cockpit_Project/main/docs/images/alert_panel/image_1.jpeg" width="45%" />
  <img src="https://raw.githubusercontent.com/aryanma11ick/Home_Cockpit_Project/main/docs/images/alert_panel/image_2.jpeg" width="45%" />
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/aryanma11ick/Home_Cockpit_Project/main/docs/images/alert_panel/image_3.jpeg" width="45%" />
  <img src="https://raw.githubusercontent.com/aryanma11ick/Home_Cockpit_Project/main/docs/images/alert_panel/image_4.jpeg" width="45%" />
</p>

---

## 🚀 Future Plans

* Enhanced backlighting and light diffusion
* Full overhead panel completion
* Additional cockpit modules (FCU, MCDU, etc.)
* Improved simulator integration and feedback systems
* Expansion to a complete cockpit build

---

## 👤 Author

**Aryan Mallick**
Building a full-scale home cockpit system from scratch

---

## ⭐ Notes

This project is continuously evolving with improvements in:

* Hardware design
* Firmware reliability
* Simulator integration

Contributions, suggestions, and feedback are welcome.
