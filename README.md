# ChaBots - WRO Future Engineers 2026

<!--<img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-nacional/v-photos/resources/ChaBotsLogo.png?raw=true" width="250">-->


## Follow us!
  <!-- Facebook -->
  <a href="https://www.facebook.com/chabotsMX/">
    <img src="https://cdn-icons-png.flaticon.com/512/733/733547.png" width="40" alt="Facebook">
  </a>
  <!-- Instagram (con degradado real) -->
  <a href="https://www.instagram.com/chabotsmx/" target="_blank">
    <img src="https://cdn-icons-png.flaticon.com/512/2111/2111463.png" width="40" alt="Instagram">
  </a>
  <!-- YouTube -->
  <a href="https://www.youtube.com/@chabotsmx1956/videos" target="_blank">
    <img src="https://cdn-icons-png.flaticon.com/512/1384/1384060.png" width="40" alt="YouTube">
  </a>
  <!-- Página Web (icono de internet) -->
  <a href="https://www.chabots.mx" target="_blank">
    <img src="https://cdn-icons-png.flaticon.com/512/841/841364.png" width="40" alt="Website">
  </a>

This repository contains the documentation for **ChaBots** participation in the **WRO Future Engineers 2026** category. Our robot was designed and built by a Mexican students team,  passionate about robotics and education.


## 📜 Table of Contents

1. 🧑‍💻 [The Team](#the-team)
2. 🎯 [The Challenge](#the-challenge)
3. 💭 [Discussion](#discussion)
4. 🤖 [Robot Overview](#robot-overview)
5. 🔋 [Sense Overview](#sense-overview)
6. ⚙️ [Mobility Management](#mobility-management)
7. 💡 [Electronics](#electronics)
8. 💻 [Code Overview](#code-overview)
9. 🚧 [Obstacle Management](#obstacle-management)
10. 🛠️ [Construction Guide](#construction-guide)
11. 💰 [Cost Report](#cost-report)
12. 📚 [Resources](#resources)
13. ©️ [License](#license)
---

## 1. The Team <a name="the-team"></a>
<div align="center">
<img src="">
</div>
<div align="center">
    <h2 style="color:#1e90ff; font-size:2.2em; margin-top:0.5em; margin-bottom:0.2em;">
        <span style="color:#222; background:linear-gradient(90deg,#1e90ff,#00c3ff,#00ffb3,#1e90ff);-webkit-background-clip:text;-webkit-text-fill-color:transparent;">We are <b>ChaBots Tuneados</b></span> -[:]
    </h2>
</div>

### Roy Iván Barrón Martínez
**Age:** 21\
**Role:** Captain & Software Designer

I am a self-taught robotics enthusiast with experience in embedded systems, software, and mechanical integration. my team ChaBots Ocelot won Mexico Robocup soccer Open second place and achieved multiple national awards in programming and robotics.

> "I enjoy setting nearly impossible goals to push myself while learning. I believe that learning should always lead to building something real."

---

### Leonardo Villegas López
**Age:** 21\
**Role:** Mechanical Designer

| am a Mechatronics Engineering student passionate about technology and innovation. I have been a contestant for eight years, winning various regional and national competitions, and participating internationally.
> "I will take any opportunity to grow"

---

### Hugo Iván Guerrero Díaz
**Age:** 19\
**Role:** Electronics Designer
I am a Mechanical Engineering student that has been participating in robotics competitions since 2019. I won at the RoboCup Mexican Open and represented Mexico at the international RoboCup event in Brazil.


> "No."

---

### Diego Vitales Medellín
**Age:** 23\
**Role:** Coach

I've been involved in robotics for 14+ years being a programmer for most of the projects I've taken part in. I've had may regional, national and international experiences. Now I'm working in sharing my knowledge with more people to push further their level and potential as well as helping them achieve their goals and find their passion.

> "I like to face challenges and even more so when it's with more people. Learning and creating something is better when shared."

---

## 2. The Challenge <a name="the-challenge"></a>

The **WRO Future Engineers** challenge pushes students to create fully autonomous self-driving vehicles. Each robot must:

- Navigate a dynamically randomized track
- Detect and avoid colored obstacles (green/red blocks)
- Execute a parallel parking maneuver

Scoring is based on:
- Performance on track
- Obstacle handling
- Documentation quality
- Innovation and engineering rigor

For more indo visit: [WRO Official Site](https://wro-association.org/)

---

## 3. Discussion <a name="discussion"></a>

### 3.1. Decisions

We have decided to create a robot with a reduced size, as compact as possible to be able to maneuver more easily and avoid obstacles without further complication. For this reason, we decided to use TOF sensors instead of lidar, as they take up much less space and have several advantages over lidar. Similarly, we decided against using an OTOS and instead opted for a motor with an encoder to determine the robot's position.

### 3.2. Rebuilding the System

**New Steering System**:
- The steering system was redesigned to be much more compact, and the angles were calculated to make it an Ackermann steering system, so that it has a much smaller turning radius without the wheels slipping.

**New PCBs**:
- We decided to have a main PCB to which all the robot's peripherals are connected.

**Testing Infrastructure**:
- Built a complete testing environment that simulates competition conditions
- Developed automated testing scripts for regression testing
- Created comprehensive calibration procedures

---

## 4. Robot Overview <a name="robot-overview"></a>

 **Name:** Pistonudo

<table style="width: 100%;">
  <tbody>
    <tr>
      <td>
        <center><h4>Front</h4></center>
        <img src="" style="width: 100%;">
      </td>
      <td>
        <center><h4>Back</h4></center>
        <img src="" style="width: 100%;">
      </td>
    </tr>
    <tr>
      <td>
        <center><h4>Left</h4></center>
        <img src="" style="width: 100%;">
      </td>
      <td>
        <center><h4>Right</h4></center>
        <img src="" style="width: 100%;">
      </td>
    </tr>
  </tbody>
</table>

---

## 5. Sense Overview <a name="sense-overview"></a>

### 5.1.  Pololu VL53L8CX

<div>
  <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/other-components/tof-vl53l8cx.png?raw=true">
</div>

**Tech specs:**

-  8×8 Multizone detection: Capability to output a 64-zone depth map with a wide 65° diagonal field of view (FoV).
- Extended range:  Accurate distance measurement up to 400 cm (4 meters) across all zones.
    
-  High-speed sampling:  Operates at frequencies up to 60 Hz for real-time motion tracking.
    
-  Integrated voltage regulation:  Onboard regulators and level shifters allowing a wide input voltage range (3.2V to 5V).
    
-   Ambient light immunity:  Advanced silicon-on-insulator (SOI) technology for consistent performance under indoor and outdoor lighting.
    
-   Programmable features:  Configurable regions of interest (ROI) and autonomous low-power modes for energy efficiency.
    
-  Communication interface:  Supports  I2C  (up to 1 MHz) and SPI (up to 3 MHz) for high-bandwidth data transfer.
    
-   Motion indicator:  Built-in motion detection algorithm to identify movement within each specific zone.

**Link:** [VL53L8CX](https://www.pololu.com/product/3419)

### 5.2. OpenMV H7

<div>
  <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/other-components/camera-H7.png?raw=true">
</div>

**Tech specs:**

-  High-performance MCU:  STM32H743VI ARM Cortex-M7 processor running at 480 MHz with 1MB of RAM and 2MB of flash.
    
-  Integrated Image Sensor:  Features the OV7725 sensor capable of capturing 640x480 (VGA) images at 75 FPS.
    
-  Advanced Vision Support:  Native hardware acceleration for color tracking, face detection, QR/barcode decoding, and optical flow.
    
-  Expandable Storage:  Integrated microSD card socket supporting high-speed UHS-I transfers for image and data logging.
    
-  Versatile I/O:  Comprehensive set of pins including SPI,  I2C, USART, PWM, and 12-bit ADC/DAC for peripheral interfacing.
    
-  Machine Learning Ready:  Optimized for running quantized neural networks via TensorFlow Lite for Microcontrollers.
    
-  RGB LED and Flash:  Built-in high-intensity RGB LED and two high-power IR LEDs for night vision or low-light conditions.
    
-  Python Programmability:  Fully programmable via MicroPython, allowing for rapid prototyping and real-time script execution.

**Link:** [OpenMV H7](https://openmv.io/products/openmv-cam-h7?srsltid=AfmBOoo6sZCCZkX54ofHCneaBRR4oAl444A5ZMhdEFs70Z3Cm0ei3gXx)

---

## 6. Mobility Management <a name="mobility-management"></a>

### 6.1. Gearbox:
<img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/gearbox/gearbox-assembly.png?raw=true">
The robot's transmission features a custom-designed gearbox, with the base and gears developed in CAD software and manufactured in-house. For fabrication, the team used a Creality K2 Plus Combo printer, chosen for its reliability in handling engineering-grade materials. The material selected was Polymaker PETG-CF (a carbon-fiber-infused PETG), prized for its high stiffness, dimensional stability, and excellent wear resistance, which are critical for durable mechanical components.
A key design feature is the use of double helical gears. This geometry was chosen over standard spur gears to ensure smoother, quieter power transmission with reduced vibration and superior load distribution. This significantly improves mechanical efficiency and component lifespan.
The drive axle consists of 4 mm steel shafts, which were custom-cut from rod stock. To ensure positive torque transfer from the gearbox to the wheels, the ends of the shafts were manually modified using a Dremel tool to create a "D" shape. This profile prevents slippage between the shaft and the wheel hub, a common failure point in high-torque applications.

| Part | Description | Image |
| --- | --- | --- |
| 6.1.1. Pololu 25D Encouder | The 25D Metal Gearmotor serves as the robust mechanical core of the propulsion system, featuring a high-performance 12V brushed DC motor paired with a 9.7:1 metal spur gearbox. This unit was selected for its exceptional balance of compact form factor and dynamic response. The precision-cut metal gears act as the primary mechanism for torque amplification, achieving a versatile output speed of approximately 600 RPM. Furthermore, the integrated quadrature encoder provides real-time feedback for closed-loop control, ensuring the precise velocity and positional accuracy required for the robot’s agile maneuvering and synchronized movement. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/gearbox/gearbox-25d-metal-gearmotor-4.4-9.7-encoder.png?raw=true" style="width: 100%;"></picture> |
| 6.1.2. Base | We designed the base of the gearbox so that the wheel axle is as close as possible to the steering axis in order to make tighter turns. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/gearbox/gearbox-base.png?raw=true" style="width: 100%;"></picture> |
| 6.1.3. Gears | The custom-printed double helical gears transfer power from the motor's gearhead output to the wheel axle. This external gear stage was designed with a 1:1 gear ratio. This configuration was chosen because the Maxon motor's integrated gearhead already provided the ideal speed reduction (down to 600 RPM) and torque multiplication. The 1:1 external gears, therefore, act as a direct power transmission, simplifying the design while perfectly matching the motor's output speed to the drive wheels. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/gearbox/gearbox-gear.png?raw=true" style="width: 100%;"></picture> |
| 6.1.4. Rhine G | The rear traction system features a custom-engineered oversized rhine, specifically designed to compensate for the robot’s geometry and maintain a consistent ground clearance across the chassis. Precision-machined with a dedicated D-shaft coupler, the rim ensures a high-tolerance press-fit onto the motor’s output shaft, eliminating backlash during high-torque maneuvers. Beyond its functional role in leveling the platform, the wheel incorporates an aggressive, aesthetic-driven spoke pattern that aligns with the "tuned" visual identity of the team. This design prioritizes both structural integrity for payload distribution and a high-profile aesthetic that distinguishes the robot’s stance on the field. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/gearbox/gearbox-rhim-g.png?raw=true" style="width: 100%;"></picture> |

### 6.2. Steering System
<img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-assembly.png?raw=true">
For the steering system, the goal was to simplify the mechanism as much as possible, as this would allow for quick and easy manufacturing. However, we decided to make this an Ackermann system, allowing the inner wheel to have a larger angle than the outer wheel. Thanks to this, we were able to prevent the front wheels from slipping when turning.

| Part | Description | Image |
| --- | --- | :---: |
| 6.2.1. Servo HiTEC HS-85MG | We selected the HiTEC HS-85MG for our robot's Ackermann steering system, primarily due to its robust metal gears (MG). Unlike many standard or smaller servos that use plastic gears, the metal gearing provides the significantly enhanced durability and resistance to stripping that our steering mechanism requires. This is crucial for us to handle the mechanical loads, vibrations, and potential impacts inherent in the system's operation. We also find that this servo packs considerable torque and good precision into a compact "mighty mini" form factor, supported by a top ball bearing. This ensures it provides the strength we need to turn the wheels effectively while maintaining accurate steering angles, minimizing the excessive "slop" or backlash we might see in less robust options. For application, this blend of power, durability, and reliable accuracy in a small package makes it a superior choice over servos that could fail or wear quickly under the demands of steering. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-HS-85MG.png?raw=true" style="width: 100%;"> </picture> |
| 6.2.2. Base | We designed the base around the servo, so that everything was symmetrical. We also designed the base to be modular and easily attach to the robot's chassis for easy repairs. We 3D printed the base using carbon fiber filament, as we did all the other robot parts, to increase strength. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-base.png?raw=true" style="width: 100%;"> </picture> |
| 6.2.3. Servo Connector | We designed the servo connector this way because, as an Ackermann system, the wheels needed to be connected independently of each other. If we used a single connector for the wheels, both would have the same turning angle, but by splitting it, each wheel would turn at a different angle. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-servoconnector.png?raw=true" style="width: 100%;"> </picture> |
| 6.2.4. Bracket Connectors | As mentioned above, we used two connectors, one per wheel, so they rotated independently.| <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-bracketconnector.png?raw=true" style="width: 100%;"> </picture> |
| 6.2.5. Stabilizer | This stabilizer bar is to prevent the wheel brackets from having play and to make them more stable. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-stabilizer.png?raw=true" style="width: 100%;"> </picture> |
| 6.2.6. Wheel Bracket | We designed the L-shaped wheel mount so that the wheels could rotate more easily and not collide with the robot's chassis. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-wheelbracket.png?raw=true" style="width: 100%;"> </picture> |
| 6.2.7. Rhine S | This rhine represents the compact iteration of our wheel assembly, specifically engineered to interface with the front wheel brackets. This downsized variant is critical for maintaining the robot’s horizontal alignment, counterbalancing the larger rear drive wheels to achieve a perfectly leveled center of gravity. Designed for low-friction rotation and high maneuverability, the Rhine S utilizes a streamlined profile that reduces rotational inertia. We then used a mold and polyurethane resin a40 to make the rubber. This process is shown in the following video: [WRO FutureEngineers Custom Wheels - chaBots NERV](https://youtu.be/8JH6QCOU_B0) | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-rhine-s.png?raw=true" style="width: 100%;"> </picture>|

### 6.3. Camera Mount

<img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/cam-mount/cam-mount.png?raw=true">
The camera mounting system features a modular design engineered for precise tilt-axis orientation. This adjustable bracket allows for granular control over the vertical field of view, enabling the sensor to be locked at the optimal angle for task-specific vision processing. The assembly utilizes a friction-fit pivot mechanism combined with secure fastening points to ensure the camera maintains its orientation despite high-vibration environments or rapid chassis acceleration. By providing a customizable pitch range, the mount ensures the optical center can be calibrated to detect floor-level markers or distant environmental cues with consistent reliability.

| Part | Description | Image |
| --- | --- | :---: |
| 6.3.1. Camera Base | This part is mounted on the chassis with the help of some posts. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/cam-mount/cam-base.png?raw=true" style="width: 100%;"> </picture> |
| 6.3.2. Camera Support | This part holds the camera and is attached to the base with screws to allow manipulation of the clamping angle. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/cam-mount/cam-support.png?raw=true" style="width: 100%;"> </picture> |

### 6.4. Chasis

| Part | Description | Image |
| --- | --- | :---: |
| 6.4.1. Camera Base | The chassis is the robot's main structure, as all other systems are mounted on it. A modular design was chosen to facilitate assembly and maintenance. The chassis is made of carbon fiber filament. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/chassis/chassis-base.png?raw=true" style="width: 100%;"> </picture> |

### 6.5. Assembly
<img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-orthogonal.png?raw=true" alt="Robot Assembly Diagram">

The steering system is mounted on the chassis using 20mm-high M3 posts. The gearbox is mounted directly to the rear of the chassis, and the main PCB is mounted on it using 30mm-high M3 posts. The OpenMV H7 camera base is mounted on the main PCB using 30mm-high M3 posts. The TOF sensors are connected on their PCBs which are mounted using 3D printed supports.

<table style="width: 100%;">
  <tbody>
    <tr>
      <td><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-front.png?raw=true" style="width: 100%;"></td>
      <td><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-asssembly-back.png?raw=true" style="width: 100%;"></td>
    </tr>
    <tr>
      <td><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-left.png?raw=true" style="width: 100%;"></td>
      <td><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-right.png?raw=true" style="width: 100%;"></td>
    </tr>
    <tr>
      <td><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-top.png?raw=true" style="width: 100%;"></td>
      <td><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-bottom.png?raw=true" style="width: 100%;"></td>
    </tr>
  </tbody>
</table>

---

## 7. Electronics <a name="electronics"></a>


## 7.1.1 Precision Improvements

### Distance Sensing Upgrade

We transitioned from the **RPLIDAR C1 Lidar Sensor** to four **VL53L8CX Time-of-Flight (ToF) sensors**.

-   **RPLIDAR C1**
    -   360° field of view
    -   Maximum frequency: ~10 Hz (practically ~5 Hz usable)
    -  	Minimum sensing distance: ~5 cm
-   **VL53L8CX (x4)**
    -   Narrow field of view (~45° per sensor, depending on configuration)
    -  	Minimum sensing distance: ~5 cm
    -   8×8 zone depth grid per sensor
    -   Maximum frequency: 60 Hz (practically ~50 Hz usable)
    
    -   Very compact form factor

While the LIDAR provided full 360° environmental coverage, the ToF sensors have a significantly narrower field of view, making their coverage comparatively limited. However, by strategically placing multiple sensors surrounding the robot and leveraging their much higher refresh rate, the overall system achieves faster and more responsive distance measurements. In our use case, the reduced field of view is almost negligible compared to the gain in update speed and reliability.

----------



## 7.1.2 Size and Compactness

A major constraint this year was the significantly smaller robot frame.

-   A custom PCB was designed and reduced to **8 × 8 cm**
-   The design evolved from **multiple PCBs** to a **single integrated board**, simplifying wiring and reducing failure points
-   This is our **first full single-board implementation**, representing a major step forward in system integration

Initially, the PCB was intended to occupy the robot base. However, due to mechanical constraints and limited usable space, the design was changed to a **top-mounted configuration**.

This resulted in:

-   More efficient use of space
-   Reduced wiring complexity
-   A cleaner and more organized layout

----------

## 7.1.3 Serviceability Improvements

Mounting the PCB on top of the robot significantly improved accessibility:

-   All components are now **fully accessible**, including:
    -   Microcontrollers
    -   Sensors
    -   Power systems

This allows for:

-   Faster repairs and adjustments
-   Easier testing and iteration
-   Reduced downtime during development

----------

## 7.1.4 Debugging and Feedback Systems

### Previous Year

-   Required an external monitor via HDMI to interface with the Raspberry Pi 5
-   Bulky and inefficient for rapid testing

### Current System

We implemented multiple onboard debugging and feedback tools:

-   **Buzzer** → audible system feedback
-   **Programmable RGB LED** → system state indication
-   **Dedicated indicator LEDs** for:
    -   Motor driver
    -   Servo activity
    -   Power (ON state)

Additionally:

-   The **OpenMV camera** can be connected via **Micro USB**
-   Full debugging and calibration is done through the **OpenMV IDE** on a laptop

### Wireless Debugging (Development Only)

We also integrated an **XIAO C6 microcontroller** for **wireless debugging during development**:

-   Provides live telemetry and sensor data
-   Enables real-time monitoring without physical connections
-   Used **only during programming and testing**, not during official competition runs

This greatly improves development speed while keeping the competition system simple and reliable.

----------

## 7.1.5 IMU Upgrade

We upgraded from the **BNO055** to the **BNO085**.

### Improvements:

-   Communication switched from **I2C → UART**
    -   More robust and reliable communication
-   Significantly reduced drift
-   Minimal calibration required for our use case

Although UART integration is slightly more complex, the reliability improvements justified the change.

----------

## Summary

This year’s electronics system represents a major step forward in:

-   **High-speed sensing and responsiveness**
-   **Compact, single-board design**
-   **Improved accessibility and serviceability**
-   **Efficient and modern debugging tools**

The system is also designed with **future upgrades in mind**, particularly the easy transition to the OpenMV N6 once firmware limitations are resolved.

## 7.2.1 Microcontroller Selection


|MCU  |Clock speed |  Observed PWM behavior*
|--|--|--|
| Teensy 4.0 | 600 MHz |Excellent|
|Arduino Nano|16 MHz|Good|
|Raspberry Pi Pico|133 MHz|Inconsistent at low duty|
|Seeed Xiao C6|120 MHz|Good + BLE|


*Notes on PWM behavior: Using the same nominal PWM frequency across all three MCUs, we observed different motor responses. On the Teensy 4.0, the motor starts smoothly and can deliver more torque at lower duty cycles. The Raspberry Pi Pico struggled at very low speeds and required careful tuning. The Arduino Nano behaved as expected for an AVR‑based board. We could not find definitive documentation explaining these differences; our working theory is that PWM implementation details (SDK, drivers, timer resolution, and library quality) play a role. Teensy and Arduino platforms are primarily C/C++ with mature vendor‑maintained timer libraries, while typical Pico workflows often lean on MicroPython or community libraries, which may trade convenience for timing granularity.

## 7.2.2 Power architecture

-   *Main battery:* 3S (11.1 V) *LiPo, ~1000 mAh. Powers motor drivers and 5V regulator.
-   *Regulation:* Pololu *S8V9F5*5V 1,5A Step-Up/Step-Down Voltage Regulator. Powers Teensy 4.0, Seeed Xiao C6, motor encoders, RGB LED and the servo. 
- Both the Teensy 4.0 and Seeed Xiao C6 regulate 3.3V to interface with sensors

## 7.2.3 Motor driver

We use the *VNH7070AS* full‑bridge driver. It comfortably handles the required current (≈5A continuous in our use case) and supports supply voltages above 20 V, providing generous headroom. Because there is no widely available off‑the‑shelf module for this device, we integrated it directly onto our PCB. This raises the bar for assembly, but the result is more reliable and, in practice, more cost‑effective.
### Vision System Redesign

Our vision systen is something we realy struggled with last year so we decided to move away from a **Raspberry Pi 5 + Camera Module v3** setup to a fully integrated microcontroller-based vision system.

#### Camera Platforms Considered


| Device | Processing Type | Approx. Current Draw | Max FPS | Notes |
|--|--|--|--|--|
| Raspberry Pi 5 + Camera V3 | SBC (Linux) | ~2–5 A @ 5V | ~60+ FPS | High performance, high power consumption |
| OpenMV N6 (planned) | MCU | ~140–200 mA | ~240FPS | New, compact, efficient |
| OpenMV H7 (used) | MCU | ~120–180 mA | ~80 FPS | Stable and well-supported |
| MaixCam Lite (considered) | AI SoC | ~300–500 mA | ~60 FPS (sensor-limited) | Higher compute, limited documentation |


#### Final Decision

-   The **OpenMV N6** was initially selected due to its:
    -   Low power consumption
    -   Small size
    -   Simplified embedded workflow
-   However, due to firmware limitations (manual exposure and white balance control not available), we transitioned to the **OpenMV H7**, which provided reliable and predictable performance.
-   The **MaixCam Lite** was ultimately not used due to:
    -   Limited documentation
    -   Less mature ecosystem
    -   Frame rate limitations imposed by its image sensor

**Future Upgrade Path:**  
The system is designed so that switching from the H7 to the N6 can be done almost instantly once stable firmware becomes available.

----------

## 7.2.4 Odometry System Redesign

We transitioned away from the **SparkFun OTOS optical tracking sensor** due to reliability issues observed during testing:

-   Significant drift over time
-   Inconsistent readings depending on speed
-   Failure to reliably detect movement at low speeds
-   Poor accuracy at higher speeds

These limitations made it unsuitable for precise and repeatable odometry.

### New Approach

We replaced the OTOS with a combined system using:

-   **BNO085 IMU (UART)**
-   **Pololu 25D motors with integrated encoders**

This hybrid approach provides a much more reliable solution:

-   **Encoders** → Accurate wheel-based distance measurement
-   **BNO085** → Stable orientation tracking with minimal drift
-   **Sensor fusion** → Improved overall position estimation

### Results

-   Consistent tracking across all speed ranges
-   Reliable detection of small movements
-   Significantly reduced drift compared to the previous system
-   More predictable and tunable behavior

This change resulted in a robust and competition-ready odometry system, better suited for precise navigation and control.


## 7.2.5 PCB design

For the first iteration of this robots PCB we opted for a simple 2 layer board, as it made it much easier to diagnose and fix any problems that might arise. However, by using ground pour on both layers combined with stitching vias, short return paths, and careful placement, we were able to produce a fully integrated board with low EMI emissions. Despite still having the motor as close as *5 cm* to sensitive wiring, we can still communicate at high baud rates reliably.

<table style="width: 100%; table-layout: fixed;">
  <tr>
    <td>
   The main PCB intrgrates two microcontrollers (Teensy 4.0 and Seeed Xiao C6),  a built-in full-bridge motor driver (VNH7070ASTR), servo control and all sensors. It includes , an XT30 connector for direct battery input, and protection against overcurrent, overvoltage, surges, and reverse polarity. 
    </td>
    <td>
      <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Main-PCB.png" style="width: 100%;">
    </td>
  </tr>
</table>

---
## 7.3.1 Schematic details

###  7.3.2 Schematic Overview
 <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Full.png" style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">
<table style="width: 100%; table-layout: fixed;">
  <tr>
    <td>
      <h4> 7.3.3 Microcontrollers</h4>
      <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Microcontrollers.png" style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">
      <p style="font-size: 0.9em; margin-top: 0.5em;">Teensy 4.0 and Seeed Xiao C6 logic connexions.</p>
    </td>
  </tr>
  <tr>
    <td>
      <h4> 7.3.4 Power Delivery</h4>
      <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Power%20Delivery.png" style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">
      <p style="font-size: 0.9em; margin-top: 0.5em;">Battery Conector XT30 , Slide Switch 6A 12V, Voltage Regulator S8V9F5 5V 1.5A, Filtered Servo Output and Motor Output. </p>
    </td>
  </tr>
  <tr>
    <td>
      <h4> 7.3.5 Debugging</h4>
      <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Debugging.png" style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">
      <p style="font-size: 0.9em; margin-top: 0.5em;">Buzzer, On LED, Button and Programable LED.</p>
    </td>
  </tr>
  <tr>
    <td>
      <h4>7.3.6 Sensors</h4>
      <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Sensors.png" style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">
      <p style="font-size: 0.9em; margin-top: 0.5em;">IDC conectors for our TOF sensors through I2C and Camera through UART. BNO085 mounted directly to PCB.</p>
    </td>
  </tr>
</table>

### 7.4.1 Teensy 4.0 section

-   *Role:* Real‑time control (PWM generation, sensor timing, safety interlocks). The Pi handles high‑level logic/vision.

-   *Supply isolation:* The Teensy is fed from a *clean/isolated input* to protect it from motor noise. Decouple 3.3 V with multiple *0.1 µF* and a *10 µF* bulk close to VIN/3V3.

-   *I/O mapping:* Dedicated pins for *PWM* to the motor driver and for *servo. A **UART header (H3)* exposes *TX/RX/GND* for diagnostics.

-   *Signal integrity:* If runs exceed ~10–15 cm, add *33–100 Ω* series resistors on digital lines (IN1/IN2/PWM) to damp edges and reduce ringing into the VNH driver.


###  7.4.2 Motor driver stage (VNH7070AS)

-   *Device:*  *VNH7070AS* integrated H‑bridge; internal MOSFETs handle current spikes and integrate flyback paths.

-   *Interface:* Pins *INA/INB/PWM/EN/CS* (nomenclature varies per package) connect to the Teensy. Keep logic ground common with power ground at a single low‑impedance point.

-   *Decoupling:* Place *≥ 470 µF* bulk right at *VCC* of the driver and multiple *100 nF* ceramics. Add an input *TVS (SMBJ18 class)* across motor supply near the driver.

-   *Snubbing:* For very noisy motors, an *RC snubber* (e.g., 100 nF + 1–2 Ω, tuned empirically) across the motor terminals can reduce EMI. Keep motor leads twisted and short.

-   *Thermal:* Provide a solid copper pour under the thermal pad with many *thermal vias* to GND to spread heat to inner planes.


###  7.4.3 Connectors & peripherals

-   *Servo header:* Powered from the regulated 5 V rail; keep return path next to the 5 V trace.

-   *Battery sense / telemetry (optional):* If you expose the pack to the MCU, use a *divider* + *RC* and consider *TVS/small **series resistor* to protect the ADC.

-   *NeoPixel header (P1):* Decouple with *100 nF* at the connector and, if long strips are used, a *large electrolytic* (≥1000 µF) at the first LED.


###  7.4.4 Bring‑up & test checklist

1.  Power the board with a *current‑limited bench supply* (e.g., 0.5–1 A) and verify no abnormal draw.

2.  Measure *5 V* rail at the Pi and servo connector under light load.

3.  With the motor disconnected, toggle the power switch—check gate voltage of the high‑side MOSFET for clean transitions.

4.  Connect a small DC motor and run *10–20% PWM; scope **VCC* and *GND* near the driver for spikes.

5.  Increase load gradually; verify the driver’s *thermal pad* stays below spec and that bulk caps do not heat.

6.  Verify UART debug works; confirm CS/FAULT lines (if used) change as expected under stall or overcurrent tests.

----------
## 7. Code Overview <a name="code-overview"></a>

...


---

## 8. Obstacle Management <a name="obstacle-management"></a>

The robot's obstacle management system (`teensy_obs_node`) implements a sophisticated multi-sensor approach for autonomous navigation in environments with colored obstacles. This system integrates LIDAR, odometry, and computer vision to provide intelligent obstacle detection and avoidance capabilities operating at 100 Hz (10ms cycle).

### 8.1. Detection Methods
- **Primary:** Enhanced color detection via PiCamera2 system (30 FPS)
- **Verification:** LIDAR distance measurements for obstacle confirmation and multi-directional navigation
- **Backup:** OTOS position tracking for navigation consistency and heading maintenance

### 8.2. Multi-Sensor Data Processing

#### Vision Integration
The system processes object detection outputs containing:
- **Distance:** Object distance in centimeters (color-based detection)
- **Angle:** Relative angle to object in degrees (-180° to 180°)
- **Color Classification:** 0 = GREEN, 1 = RED
- **Status:** Detection flag (0 = no object, 1 = detected)

#### LIDAR Sector Analysis
The LIDAR processes real-time distance measurements across four directional sectors:
```
Front Sector:   1.39 to 1.74 radians (79-100°)
Back Sector:    -1.74 to -1.39 radians (-100 to -79°)
Left Sector:    0.0 to 0.52 radians (0-30°)
Right Sector:   2.79 to 3.14 radians (160-180°)
```

### 8.3. Navigation States

The system operates as a state machine with three primary modes:

#### 8.3.1. Normal Navigation (No Obstacle Detected)
- **Sector-based movement:** Robot maintains orientation within four 90° sectors
- **Heading correction:** PID-based orientation control to maintain target yaw
- **Turn initiation:** Triggers when front distance < 1.0m AND heading error < 7°
- **Speed:** Constant PWM of 40 for controlled movement

#### 8.3.2. Obstacle Avoidance (Object Detected)
The system implements **color-specific evasion strategies**:

**GREEN OBSTACLE - Pass Always by Left Side:**
```
Decision Logic:
├─ If angle < 0° (object on left):
│  └─ Always evade LEFT (maximum offset: -20°)
│
└─ If angle ≥ 0° (object on right):
   ├─ If within "safe cone" (30° threshold):
   │  └─ Evade LEFT (reduced by angular weighting)
   │
   └─ If outside safe cone:
      └─ No evasion required (maintain heading)

Distance Weighting:
├─ Min Distance: 30cm (full avoidance)
├─ Max Distance: 100cm (no avoidance)
└─ Proportional weight: (maxDis - distance) / (maxDis - minDis)
```

**RED OBSTACLE - Pass Always by Right Side:**
```
Decision Logic (inverse of green):
├─ If angle ≥ 0° (object on right):
│  └─ Always evade RIGHT (maximum offset: +20°)
│
└─ If angle < 0° (object on left):
   ├─ If within "safe cone" (30° threshold):
   │  └─ Evade RIGHT (reduced by angular weighting)
   │
   └─ If outside safe cone:
      └─ No evasion required (maintain heading)
```

**Evasion Control:**
- **Servo command smoothing:** ±3°/tick (anti-jerk limiting)
- **Angular correction:** Proportional to heading error
- **Speed:** Constant PWM of 40
- **Steering range:** 60° to 150° (servo centered at 90°)

#### 8.3.3. Turn Execution (Obstacle Round Phase)
The system dynamically selects turning strategy based on available space:

**Turn Type Classification:**
```
Lateral Distance Measurement:
├─ ≥ 0.70m  → Type 3: Wide Turn (Standard 90° maneuver)
├─ 0.30-0.70m → Type 2: Medium Turn (Complex multi-step)
└─ < 0.30m  → Type 1: Close Turn (Backup required)
```

**Type 1 - Close Turn (Backup Maneuver):**
1. **Step 0:** Align to target heading (90° center, PWM 40)
   - Condition: `|correction| < 20°`
2. **Step 1:** Reverse in place (PWM 40, reverse direction)
   - Condition: `back_distance < 0.5m`

**Type 2 - Medium Turn (Complex Multi-Step):**
1. **Step 0:** Forward approach with 45° offset (PWM 30)
   - Condition: `front_distance < 0.7m`
2. **Step 1:** Reverse turn with heading correction (PWM 40)
   - Condition: `|correction| < 20°`
3. **Step 2:** Secondary reverse maneuver (PWM 40)
   - Condition: `|correction| < 10°`
4. **Step 3:** Final reverse phase (PWM 40)
   - Condition: `back_distance < 0.5m`

**Type 3 - Wide Turn (Standard Maneuver):**
1. **Step 0:** Forward approach to corner (PWM 40)
   - Condition: `front_distance < 0.30m`
2. **Step 1:** Directional turn based on side
   - Left turn: 150° (PWM 40, reverse)
   - Right turn: 30° (PWM 40, reverse)
   - Condition: Heading aligned to target (< 5° error)
3. **Step 2:** Final reverse phase (PWM 40)
   - Condition: `back_distance < 0.5m`

### 8.4. Sector-Based Navigation

The robot organizes space into four 90° sectors with defined movement targets:

```
Sector Layout:
       Front (0°)
          ↑
    Sector 0
  315°        45°
    Sector 3    Sector 1
  225°        135°
    Sector 2
          ↓
       Back (180°)

Target Orientations:
├─ Sector 0: 0° (Forward)
├─ Sector 1: 90° (Right)
├─ Sector 2: 180° (Backward)
└─ Sector 3: 270° (Left)
```

**Sector Transitions:**
- Robot tracks current sector based on heading accumulator
- Automatic sector transitions at boundary angles (45°, 135°, 225°, 315°)
- Direction preference system (left/right) initialized on first turn
- Turn allowed flag resets upon sector entry

### 8.5. Adaptive Speed Control

The system implements dynamic PWM adjustment based on error:

```
Speed Control Algorithm (controlACDA):
├─ Base PWM Selection:
│  ├─ Target < 0.6 m/s → 35 PWM
│  ├─ 0.6-1.2 m/s → 40 PWM
│  └─ > 1.2 m/s → 60 PWM
│
├─ PID Correction:
│  ├─ Kp = 8.25 (proportional gain)
│  ├─ Kd = 0.1 (derivative gain)
│  └─ Jerk limit = 10 PWM/cycle
│
└─ Speed Protection:
   ├─ Zero command: |error| ≤ -0.5 m/s
   ├─ Reduced command: -0.1 < error ≤ -0.5 m/s
   └─ Full control: error > -0.1 m/s
```

### 8.6. Safety Systems

#### Collision Prevention
- Continuous front distance monitoring (1.0m threshold)
- Immediate turn initiation upon obstacle detection
- Multi-sensor validation before maneuvers

#### State Validation
- Finite state machine prevents invalid state transitions
- Atomic operations ensure thread-safe state updates across threads
- Timeout mechanisms prevent stuck conditions

#### Error Recovery
- Automatic retry for failed turn sequences
- Fallback to normal navigation if vision system fails
- Robust UART communication with XOR checksum validation (2 Mbps baud)

### 8.7. Communication Protocol

**Serial Frame Format (6 bytes):**
```
[Header] [Error_High] [Error_Low] [PWM] [Direction] [Checksum]
  0xAB       uint8_t     uint8_t   uint8_t  uint8_t   uint8_t
```
- **Error:** 16-bit steering command (40-160 degrees)
- **PWM:** Motor speed (0-255)
- **Direction:** 0 = Forward, 1 = Reverse
- **Checksum:** XOR of all previous bytes

### 8.8. Key Advantages of This Implementation

**Deterministic Color-Based Strategy**
- Predefined passing rules (green left, red right) ensure consistent navigation patterns
- Eliminates ambiguity in obstacle avoidance decisions
- Makes behavior predictable and reproducible for competition scoring

**Adaptive Turn Mechanism**
- Three-level turn selection (close, medium, wide) optimizes maneuver efficiency
- Reduces time wasted on inappropriate turning strategies
- Handles variable corridor widths without manual recalibration

**Multi-Sensor Fusion**
- Vision provides precise object classification and angular position
- LIDAR validates distances and supplies environmental context
- Cross-validation of detections reduces false positives
- Heading accumulator maintains orientation memory across sensor failures

---

## 9. Construction Guide <a name="construction-guide"></a>

**Models file folder:** `models/`

### 9.1. Steps
- Step 1: 3D designing
- Step 2: 3D printing
- Step 3: Electronic layout
- Step 4: Wiring
- Step 5: Mounting
- Step 6: Programming
- Step 7: Testing

### 9.2. Construction Tools
- 3D Printer (Creality K2 Plus, QIDI Q1 Pro)
- Polymaker PTG CF filament
- Mini Electric Soldering Iron Kit TS101
- Dremel Tool
- Screwdriver Set Fanttik


## 10. Cost Report <a name="cost-report"></a>

| Item                         | Qty | Unit Cost (MXN) | Total (MXN) |
|------------------------------|-----|------------------|-------------|
| Teensy 4.0                   | 1   | $800              | 800         |
| Raspberry Pi 5                | 1   | $2,800             | $2,800        |
| RPlidar C1                    | 1   | $2,500             | $2,500        |
| Raspberry Pi Camera 12mp V3   | 1   | $920              | $920         |
| Raspberry Pi 5 Camera Cable   | 1   | $64               | $64          |
| 2.2Ah LiPo 11.1V Battery     | 1   | $600              | $600         |
| 1Ah LiPo 3.3V Battery     | 1   | $70               | $70          |
| Maxon Motor DCX19            | 1   | $8,500             | $8,500        |
| HS85MG Micro Servo            | 1   | $2,000             | $2,000        |
| SparkFun OTOS                 | 1   | $2,400             | $2,400        |
| POLYMAKER PLA Filament (prototypes)    | 1kg   | $900        | $900         |
| POLYMAKER PLA-CF Filament (finals)     | 0.5kg   | $450       | $450         |
| Carbon Fiber                  | 1   | $2,000             | $2,000        |
| SMD Components & Misc.   | -   |         $1,500      | $1,500        |
| PCB Manufacturing             | 1   | $800              | $800         |
| Spike Wheels (LEGO)         | 4   | $150              | $600         |
| EV3 Wheels (LEGO)          | 2   | $10              | $20         |
| **Total**                     |     |                  | **$26,924**|


---

## Resources <a name="resources"></a>

- [Chabots Main Site](https://www.chabots.mx)
- [WRO Future Engineers Rules PDF](https://wro-association.org/wp-content/uploads/WRO-2024-Future-Engineers-Self-Driving-Cars-General-Rules.pdf)
- [GitHub Repos](https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026)

---

## License <a name="license"></a>

```
MIT License
Permission is hereby granted, free of charge, to any person obtaining a copy of this software.
```

---

> *Document maintained by Chabots | Last updated: Apr 2026*

<!--stackedit_data:
eyJoaXN0b3J5IjpbMTcyMzM3ODYxNCwtMzc2NTM2MDM5LDM1ND
c4NDQyMCwxMjQ4Mzg0MTM1LC0yODM3NTcxNywtMTMyNzEwNTIy
MywxMjg3Nzk2NjQsLTQ4MTYzMzM4MF19
-->


> Written with [StackEdit](https://stackedit.io/).