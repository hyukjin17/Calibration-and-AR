# Real-Time Augmented Reality & 3D Rendering Engine

**Author:** Hyuk Jin Chung  
**Date:** March 2026  
**Language:** C++ (OpenCV)

## Overview

A C++ based Augmented Reality (AR) system built from scratch using OpenCV. This project handles real-time camera calibration, ArUco marker tracking, pose estimation, and features a custom-built software rasterizer capable of parsing and rendering shaded 3D `.obj` models purely on the CPU.

## Features

* **Robust Camera Calibration:** Calculates and saves camera intrinsic parameters and distortion coefficients using physical target boards.
* **Real-Time Pose Estimation:** Uses `cv::solvePnP` to continuously track the 6-DOF translation and rotation of the camera relative to the target.
* **Custom Software Rasterizer:** Bypasses standard graphics APIs (like OpenGL) to render 3D models using pure C++ and OpenCV. Features include:
  * Custom `.obj` file parser for 3D vertices and face extraction.
  * Local-to-World space coordinate transformations.
  * Painter's Algorithm (Centroid Z-depth sorting) for proper occlusion.
  * Backface culling to optimize rendering speed.
  * Dynamic flat-shading calculated via surface normal dot products.
* **Natural Feature Tracking:** Includes a standalone module for exploring ORB feature detection, descriptor extraction, and real-time parameter tuning for markerless AR foundations.

## Architecture & Project Structure

The codebase strictly adheres to the Single Responsibility Principle, decoupling the vision math (Model), the rendering (View), and the user input (Controller).

```text
├── CMakeLists.txt
├── README.md
├── include
│   ├── AppController.hpp    # Manages user input, file I/O, and application state
│   ├── Calibrator.hpp       # Handles camera intrinsics, distortion math, and calibration saving
│   ├── Renderer.hpp         # The 3D graphics engine (parsing, transforming, shading, drawing)
│   ├── TargetDetector.hpp   # Vision system for finding and extracting ArUco board corners
│   └── config.hpp           # Centralized global constants (board size, file paths)
└── src
    ├── AppController.cpp
    ├── Calibrator.cpp
    ├── Renderer.cpp
    ├── TargetDetector.cpp
    ├── feature_test.cpp     # Standalone executable for ORB feature detection & UI sliders
    ├── generate_board.cpp   # Utility to generate the printable ArUco tracking target
    └── main.cpp             # The core application loop tying vision and rendering together
```

## Build Instructions

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/hyukjin17/Calibration-and-AR.git
    cd Calibration-and-AR
    ```

2.  **Create a build directory:** (use release config for faster real time video processing)
    ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    ```

3.  **Run CMake and Compile:**
    ```bash
    cmake --build build --config Release
    make
    ```

4.  **Create the custom ArUco board:** (used for camera calibration)
    ```bash
    ./build/board
    ```

5.  **Run the Application:**
    ```bash
    ./build/ar
    ```
    Print out the ArUco board or display it on screen for the camera to detect markers (for calibration)

---

## Interactive Controls

The system is fully controlled via keyboard inputs while the OpenCV video windows are actively in focus.

| Key | Description |
| :---: | :--- |
| **`s`** | Save the current frame for camera calibration. (Take 5-10 images from various angles). |
| **`c`** | Run the calibration math using the saved images. |
| **`d`** | Delete all currently saved calibration images and reset. |
| **`a`** | Toggle the 3D coordinate axes (X, Y, Z) anchored to the target origin. |
| **`v`** | Toggle a custom 3D wireframe object (e.g., floating crystal). |
| **`r`** | Toggle the shaded, flat-rendered .obj 3D model. |
| **`q`** | Safely shuts down the camera and exits the program. |

---

## ORB Feature Tracking

To experiment with natural, markerless feature tracking, compile and run the feature_test.cpp executable. This launches a live camera feed with interactive GUI sliders to test the sensitivity (fastThreshold) and capacity (maxFeatures) of the ORB detector against real-world textures.

```bash
./build/test
```