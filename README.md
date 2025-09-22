# 3D-Solar-System-Simulator-

# 🌌 3D Solar System Simulator

A real-time **3D Solar System Simulator** built with **Qt + OpenGL + C++**, featuring interactive camera controls, planet orbits, and adjustable simulation speed.

## 🚀 Features

* 🌞 **Sun, Earth, Moon, and other planets** rendered with OpenGL.
* 🌍 **Planetary orbits** with rotation and revolution.
* 🖱️ **Interactive controls** (rotate, zoom, pan).
* 🎛️ **Qt UI integration** (sliders, buttons to control simulation speed).
* 💡 **Lighting and shading** for realistic look.
* ⏩ Adjustable **simulation speed** (slow motion or fast-forward).

---

## 🛠️ Tech Stack

* **C++17**
* **Qt 6 (Widgets + OpenGLWidgets)**
* **OpenGL (GL + GLU)**
* **CMake** (for build system)
* Tested on **Windows 10** and **Ubuntu 20.04**

---

## ⚙️ Installation & Setup

### 1. Prerequisites

* Install [Qt](https://www.qt.io/download-open-source) (with MinGW on Windows or system packages on Linux).
* Install [CMake](https://cmake.org/download/).
* Install [Visual Studio ] or use **Qt Creator**.

### 2. Clone Repository

```sh
git clone https://github.com/yourusername/solar-system-simulator.git
cd solar-system-simulator
```

### 3. Build (CMake)

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

### 4. Run

```sh
./SolarSystemSimulator   # Linux
SolarSystemSimulator.exe # Windows
```

---

## 🎮 Controls

* **Mouse Drag** → Rotate camera.
* **Mouse Scroll** → Zoom in/out.
* **Arrow Keys** → Pan view.
* **Spacebar** → Pause/Resume simulation.
* **Slider (Qt UI)** → Adjust simulation speed.

---

## 📂 Project Structure

```
SolarSystemSimulator/
├── CMakeLists.txt
├── main.cpp
├── SolarSystemWidget.h / .cpp
├── resources/        # textures, icons, shaders
├── docs/             # screenshots, documentation
└── README.md
```

---

## 🖼️ Future Improvements

* Add **planet textures** (Earth map, Mars, etc.).
* Implement **satellite / asteroid belt simulation**.
* Use **modern OpenGL shaders** (Phong, PBR).
* VR headset support.

---

## 📜 License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.

---

## 🙌 Credits

* Qt Framework – [https://www.qt.io](https://www.qt.io)
* OpenGL – [https://www.opengl.org](https://www.opengl.org)


---

👉 This structure makes your README professional and clear for anyone landing on your repo.

Would you like me to also create a **badge-style header** (build status, license, language) like the ones you see in popular GitHub projects?
