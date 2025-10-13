# Plastecure

> **Healing the Earth, one bottle at a time.**

![Plastecure Banner](https://github.com/theaniketraj/plastecure/blob/main/assets/logo_clean.jpg)

Plastecure is a smart recycling bin system designed to automate and incentivize the process of recycling plastic bottles. It combines hardware and software to create an interactive and rewarding experience for users.

## Overview

The Plastecure system consists of a physical smart bin and a web application. Users interact with the bin to deposit plastic bottles. The bin automatically detects the bottle, and the user's account is credited with points. The web application provides a dashboard for users to track their contributions and rewards.

The core of the system is a machine learning model that visually identifies plastic bottles, ensuring that only correct items are recycled.

## Features

*   **Automated Bottle Detection**: An ESP32-CAM captures an image of the deposited item, and a Python server with a PyTorch model verifies if it's a bottle.
*   **User Authentication & Rewards**: Users can log in with a unique ID on the machine's keypad. The system tracks the number of bottles deposited and allocates points.
*   **Real-time Bin Status**: An ultrasonic sensor monitors the fill level of the bin, and an LCD screen displays the status.
*   **Web Dashboard**: A user-friendly web interface for users to view their recycling statistics and other information.
*   **Hardware Integration**: The system uses an Arduino to control the physical components like servos, sensors, and the user interface (keypad and LCD).

## Tech Stack

### Hardware
- **Arduino Uno**: Main controller for the smart bin's hardware components.
- **ESP32-CAM**: For capturing images for bottle detection.
- **ESP32**: For I2C communication with the Arduino and potentially Wi-Fi connectivity to the backend.
- **Sensors**:
    - PIR Motion Sensor (on ESP32-CAM)
    - Ultrasonic Sensor (for bin level)
    - IR Sensor (for bottle counting)
- **Actuators**:
    - Servo Motors
- **User Interface**:
    - I2C LCD Display
    - 4x4 Matrix Keypad

### Software & Cloud
- **Backend**:
    - **Flask**: Python web server to handle image processing.
    - **PyTorch**: For the bottle detection machine learning model.
- **Frontend**:
    - HTML5
    - CSS3
    - JavaScript
- **Database & Backend-as-a-Service**:
    - **Firebase**: For user authentication, real-time database, and analytics.
- **Programming Languages**:
    - **C++ (Arduino/ESP)**
    - **Python**
    - **JavaScript**

## Setup and Installation

### 1. API Server

The API server is responsible for the machine learning-based bottle detection.

1.  **Navigate to the API directory:**
    ```bash
    cd API
    ```
2.  **Install Python dependencies:**
    ```bash
    pip install -r requirements.txt
    ```
3.  **Run the server:**
    ```bash
    python server_debug.py
    ```
    The server will start on `http://localhost:5000`. For the ESP32-CAM to access it, you'll need to expose this local server to the internet using a tool like [ngrok](https://ngrok.com/).

### 2. Hardware (Arduino & ESPs)

1.  **Open the `.ino` files** in the `Machine Codes` directory with the Arduino IDE.
2.  **Install necessary libraries** for the Arduino Uno, ESP32, and ESP32-CAM (e.g., `LiquidCrystal_I2C`, `Keypad`, `Servo`, `WiFi`, `HTTPClient`, `ArduinoJson`).
3.  **Configure the code:**
    *   In `esp32cam.ino`, update the Wi-Fi credentials (`ssid` and `password`) and the server URL (`serverBase`) to your ngrok forwarding address.
    *   In `new_arduino.ino`, ensure the I2C address for the LCD and ESP32 are correct.
4.  **Upload the code** to the respective boards.

### 3. Web Application

1.  **Open `index.html`** in your browser to view the landing page.
2.  **Firebase Configuration**: The Firebase configuration is present in `index.html`. Make sure your Firebase project has Authentication and Realtime Database enabled. The security rules for the database should be configured to allow authorized reads and writes.

## Project Structure
```
.
├── API/                # Backend Flask server for ML model
│   ├── server_debug.py
│   └── requirements.txt
├── Machine Codes/      # Code for the microcontrollers
│   ├── Arduino Uno/
│   └── esp32 cam/
├── assets/             # Images and other static assets
├── scripts/            # JavaScript for the web pages
├── style/              # CSS stylesheets
└── *.html              # HTML files for the web application
```

## Contributing

Contributions are welcome! Please feel free to submit a pull request.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
