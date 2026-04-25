
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

This repository contains the documentation for **ChaBots** participation in the **WRO Future Engineers 2025** category. Our robot was designed and built by a Mexican students team,  passionate about robotics and education.


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
<img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/photos/t-photos/t-photo.png?raw=true">
</div>
<div align="center">
    <h2 style="color:#1e90ff; font-size:2.2em; margin-top:0.5em; margin-bottom:0.2em;">
        <span style="color:#222; background:linear-gradient(90deg,#1e90ff,#00c3ff,#00ffb3,#1e90ff);-webkit-background-clip:text;-webkit-text-fill-color:transparent;">We are <b>ChaBots NERV</b></span> -[:]
    </h2>
</div>

### Roy Iván Barrón Martínez
**Age:** 21\
**Role:** Captain, Electronics & Software Designer

I am a self-taught robotics enthusiast with experience in embedded systems, software, and mechanical integration. my team ChaBots Ocelot won Mexico Robocup soccer Open second place and achieved multiple national awards in programming and robotics.

> "I enjoy setting nearly impossible goals to push myself while learning. I believe that learning should always lead to building something real."

---

### Leonardo Villegas López
**Age:** 21\
**Role:** Mechanical Designer

I am a Mechatronics Engineering student passionate about technology and innovation. I have been a contestant for eight years, winning various regional and national competitions, and participating internationally.
> "I will take any opportunity to grow"

---

### Hugo Iván Guerrero Díaz
**Age:** 19\
**Role:** Software Developer

...

> "..."

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

...

#### Hardware Selection Rationale

**TOF**:..

### 3.2. Rebuilding the System

#### What We Rebuilt

**Complete Software Stack**:
- ..

**Hardware Redesign**:
- ...

**Testing Infrastructure**:
- Built a complete testing environment that simulates competition conditions
- Developed automated testing scripts for regression testing
- Created comprehensive calibration procedures

### 3.3. Technical Achievements

...

### 3.4. Competition Strategy

...

---

## 4. Robot Overview <a name="robot-overview"></a>

 **Name:** Pistonudo

<table style="width: 100%;">
  <tbody>
    <tr>
      <td>
        <center><h4>Front</h4></center>
        <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-nacional/v-photos/national/v-front.jpeg?raw=true" style="width: 100%;">
      </td>
      <td>
        <center><h4>Back</h4></center>
        <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-nacional/v-photos/national/v-back.jpeg?raw=true" style="width: 100%;">
      </td>
    </tr>
    <tr>
      <td>
        <center><h4>Left</h4></center>
        <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-nacional/v-photos/national/v-left.jpeg?raw=true" style="width: 100%;">
      </td>
      <td>
        <center><h4>Right</h4></center>
        <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-nacional/v-photos/national/v-right.jpeg?raw=true" style="width: 100%;">
      </td>
    </tr>
  </tbody>
</table>

---

## 5. Sense Overview <a name="sense-overview"></a>

### 5.1. RPLiDAR C1
360° laser scanner for environmental mapping and obstacle detection.
<div>
  <img src="https://github.com/user-attachments/assets/6e67d0ac-5d60-4c0c-aa2a-dc37211ad280" height="350">
</div>

**Tech specs:**
- 360° scanning with 0.9° resolution
- Up to 8m range with 10Hz update rate
- Quality filtering for reliable data
- ROS2 integration via `rplidar_ros` package

**Link:** [RPLiDAR C1](https://www.slamtec.com/en/C1)

### 5.2. Raspberry Pi Camera V3
High-resolution camera for color object detection.

<div>
  <img src="https://github.com/user-attachments/assets/aeb8fb7f-716d-4731-aa02-60a23a4a5158" height="350">
</div>

**Tech specs:**
- 12MP IMX708 Quad Bayer sensor and features a High Dynamic Range mode
- Supports 1080p30, 720p60, and VGA90 video modes

**Link:** [Raspberry Pi Camera V3](https://www.raspberrypi.com/products/camera-module-3/)

### 5.3. SparkFun Optical Tracking Odometry Sensor
High-precision odometry sensor for accurate position tracking.

<div>
  <img src="https://github.com/user-attachments/assets/f2fa519e-0818-484d-805f-129e02615010" height="350">
</div>

**Tech specs:**
- Measures linear and angular displacement
- High-resolution optical flow sensor
- ROS2 integration via custom `otos_reader` node

**Link:** [SparkFun OTOS](https://www.sparkfun.com/sparkfun-optical-tracking-odometry-sensor-paa5160e1-qwiic.html)

---

## 6. Mobility Management <a name="mobility-management"></a>

### 6.1. Gearbox:
<img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/gearbox/gearbox-assembly.png?raw=true">
The robot's transmission features a custom-designed gearbox, with the base and gears developed in CAD software and manufactured in-house. For fabrication, the team used a Creality K2 Plus Combo printer, chosen for its reliability in handling engineering-grade materials. The material selected was Polymaker PETG-CF (a carbon-fiber-infused PETG), prized for its high stiffness, dimensional stability, and excellent wear resistance, which are critical for durable mechanical components.
A key design feature is the use of double helical gears. This geometry was chosen over standard spur gears to ensure smoother, quieter power transmission with reduced vibration and superior load distribution. This significantly improves mechanical efficiency and component lifespan.
The drive axle consists of 4 mm steel shafts, which were custom-cut from rod stock. To ensure positive torque transfer from the gearbox to the wheels, the ends of the shafts were manually modified using a Dremel tool to create a "D" shape. This profile prevents slippage between the shaft and the wheel hub, a common failure point in high-torque applications.

| Part | Description | Image |
| --- | --- | --- |
| 6.1.1. Maxon DCX19 | The powerhouse for the gearbox is the Maxon DCX19, a 19 mm brushed DC motor equipped with an integrated planetary gearhead. This motor was selected for its high power density, precision, and proven reliability. The integrated gearhead is the primary source of torque multiplication, providing a final output speed of 600 RPM. This combination delivers the high torque and controlled speed necessary to meet the robot's performance requirements for acceleration and payload handling. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/gearbox/gearbox-MaxonDCX19.png?raw=true" alt="Maxon DCX19 Motor with Planetary Gearhead" style="width: 100%;"></picture> |
| 6.1.2. Base | We designed the base of the gearbox so that the wheel axle is as close as possible to the steering axis in order to make tighter turns. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/gearbox/gearbox-base.png" alt="Gearbox Base Structure" style="width: 100%;"></picture> |
| 6.1.3. Gears | The custom-printed double helical gears transfer power from the motor's gearhead output to the wheel axle. This external gear stage was designed with a 1:1 gear ratio. This configuration was chosen because the Maxon motor's integrated gearhead already provided the ideal speed reduction (down to 600 RPM) and torque multiplication. The 1:1 external gears, therefore, act as a direct power transmission, simplifying the design while perfectly matching the motor's output speed to the drive wheels. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/gearbox/gearbox-gear.png" alt="Double Helical Gears" style="width: 100%;"></picture> |
| 6.1.4. Wheels | The rear wheel hubs (rims) were also custom-designed and 3D-printed to integrate perfectly with the transmission. The central feature of the hub is a D-shaped bore (hole). This profile is precisely matched to the D-shaped 4 mm steel shafts, ensuring a secure, non-slip mechanical lock. This method guarantees that all torque generated by the motor is effectively transferred from the axle directly to the wheel. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/gearbox/gearbox-rim.png?raw=true" alt="Custom Wheel Hub with D-shaped Bore" style="width: 100%;"></picture> |

### 6.2. Steering System
<img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/steering-system/steering-system-assembly.png?raw=true">
For the steering system, the goal was to simplify the mechanism as much as possible, as this would allow for quick and easy manufacturing. However, we decided to make this an Ackermann system, allowing the inner wheel to have a larger angle than the outer wheel. Thanks to this, we were able to prevent the front wheels from slipping when turning, something that occurred with the previous non-Ackermann model.

| Part | Description | Image |
| --- | --- | :---: |
| 6.2.1. Servo HiTEC HS-85MG | We selected the HiTEC HS-85MG for our robot's Ackermann steering system, primarily due to its robust metal gears (MG). Unlike many standard or smaller servos that use plastic gears, the metal gearing provides the significantly enhanced durability and resistance to stripping that our steering mechanism requires. This is crucial for us to handle the mechanical loads, vibrations, and potential impacts inherent in the system's operation. We also find that this servo packs considerable torque and good precision into a compact "mighty mini" form factor, supported by a top ball bearing. This ensures it provides the strength we need to turn the wheels effectively while maintaining accurate steering angles, minimizing the excessive "slop" or backlash we might see in less robust options. For application, this blend of power, durability, and reliable accuracy in a small package makes it a superior choice over servos that could fail or wear quickly under the demands of steering. | <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/steering-system/steering-system-HS-85MG.png" alt="HiTEC HS-85MG Servo Motor" width="1280" height="720"> |
| 6.2.2. Base | We designed the base around the servo, so that everything was symmetrical. We also designed the base to be modular and easily attach to the robot's chassis for easy repairs. We 3D printed the base using carbon fiber filament, as we did all the other robot parts, to increase strength. | <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/steering-system/steering-system-base.png" alt="Steering System Base" width="1280" height="720"> |
| 6.2.3.1. Servo Connector | We designed the servo connector this way because, as an Ackermann system, the wheels needed to be connected independently of each other. If we used a single connector for the wheels, both would have the same turning angle, but by splitting it, each wheel would turn at a different angle. | <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/steering-system/steering-system-servoconnector.png?raw=true" alt="Servo Connector" width="1280" height="720"> |
| 6.2.3.2. Wheel Connectors | As mentioned above, we used two connectors, one per wheel, so they rotated independently. These connectors aren't straight, as this shape allows us to more clearly define the angle differences between the wheels. However, we arrived at this shape experimentally, as if the shape were more pronounced, we reached a point where the wheel wouldn't return to its original position. | <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/steering-system/steering-system-bracketconnector.png?raw=true" alt="Wheel Connectors Bracket" > |
| 6.2.4. Wheel Supports | We designed the L-shaped wheel mount so that the wheels could rotate more easily and not collide with the robot's chassis. | <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/steering-system/steering-system-bracketwheel.png?raw=true" alt="L-shaped Wheel Support Mount" width="1280" height="720"> |
| 6.2.5. Wheels | We decided to make our own wheels because we couldn't find any commercial wheels that fit our robot. We previously used the Lego Spike wheels, but they had very little contact surface area, so we decided to create our own wheels using the measurements of Spike's wheels. To do this, we created a rim and 3D printed it. We then used a mold and polyurethane resin to make the rubber. This process is shown in the following video: [WRO FutureEngineers Custom Wheels - chaBots NERV](https://youtu.be/8JH6QCOU_B0) | <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/steering-system/steering-system-rhine.png?raw=true" alt="Custom Polyurethane Wheel Assembly" width="1280" height="720"> |

### 6.3. Bases

| Part | Description | Image |
| --- | --- | :---: |
| 6.3.1. Lidar Base | The base for the lidar was designed in the simplest way possible, with eight holes, four for screwing the lidar in and the rest for fixing the base. | <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/chassis/lidar-base.png?raw=true" alt="RPLiDAR Mounting Base" width="1280" height="720"> |
| 6.3.2. Camera Base | The camera mount is a copy-paste of the lidar mount, modified to support the camera. We experimentally set the angle of this mount to give the camera the widest possible field of view without it looking outside the track. | <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/chassis/camera-base.png?raw=true" alt="Camera Mounting Base" width="1280" height="720"> |
| 6.3.3. Chasis | The chassis is the robot's main structure, as all other systems are mounted on it. A modular design was chosen to facilitate assembly and maintenance. The chassis is made of carbon fiber, which was cut in China. | <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/chassis/chassis-base.png?raw=true" alt="Carbon Fiber Robot Chassis Base" width="1280" height="720"> |

### 6.4. Assembly
<img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/v-assembly.png?raw=true" alt="Robot Assembly Diagram">

The steering system is mounted on the chassis using 20mm-high M3 posts. The odometer PCB is anchored below the steering system, as this makes better use of space. The gearbox is mounted directly to the rear of the chassis, and the Raspberry Pi 5 is mounted on it using 20mm-high M2.5 posts. The main PCB is mounted in the middle, and the Lidar base is mounted on 20mm-high M3 posts. The Raspberry Pi camera v2 base is mounted on the Lidar base using 40mm-high M3 posts. Using these poles helped us keep the robot as low as possible, allowing the Lidar sensor to be leveled with the runway walls.

<table style="width: 100%;">
  <tbody>
    <tr>
      <td><img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/v-assembly-front.png?raw=true" style="width: 100%;"></td>
      <td><img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/v-assembly-back.png?raw=true" style="width: 100%;"></td>
    </tr>
    <tr>
      <td><img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/v-assembly-left.png?raw=true" style="width: 100%;"></td>
      <td><img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/v-assembly-right.png?raw=true" style="width: 100%;"></td>
    </tr>
    <tr>
      <td><img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/v-assembly-top.png?raw=true" style="width: 100%;"></td>
      <td><img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/models/v-assembly-bottom.png?raw=true" style="width: 100%;"></td>
    </tr>
  </tbody>
</table>

---

## 8. Electronics <a name="electronics"></a>

...

## Microcontroller comparison


|MCU  |Clock speed |  Observed PWM behavior*
|--|--|--|
| Teensy 4.0 | 600 MHz |Excellent|
|Arduino Nano|16 MHz|Good|
|Raspberry Pi Pico|133 MHz|Inconsistent at low duty|


*Notes on PWM behavior: Using the same nominal PWM frequency across all three MCUs, we observed different motor responses. On the Teensy 4.0, the motor starts smoothly and can deliver more torque at lower duty cycles. The Raspberry Pi Pico struggled at very low speeds and required careful tuning. The Arduino Nano behaved as expected for an AVR‑based board. We could not find definitive documentation explaining these differences; our working theory is that PWM implementation details (SDK, drivers, timer resolution, and library quality) play a role. Teensy and Arduino platforms are primarily C/C++ with mature vendor‑maintained timer libraries, while typical Pico workflows often lean on MicroPython or community libraries, which may trade convenience for timing granularity.

## Power architecture

-   *Main battery:* 3S (11.1 V) *LiPo, ~2200 mAh*.

-   *Regulation:* Pololu *D42V55F5* step‑down regulator (5 V, up to 6 A) feeding the Raspberry Pi 5 and the servo.

-   *Isolation:* The Teensy 4.0 is powered from an *isolated supply* to protect it from motor/EMI transients; all other subsystems draw from the main rail via appropriate regulators and filtering.


## Motor driver

We use the *VNH7070AS* full‑bridge driver. It comfortably handles the required current (≈8 A continuous in our use case) and supports supply voltages above 20 V, providing generous headroom. Because there is no widely available off‑the‑shelf module for this device, we integrated it directly onto our PCB. This raises the bar for assembly, but the result is more reliable and, in practice, more cost‑effective.

## PCB design

Our PCB uses *6 layers*:
|Layer  |Type  |
|--|--|
| 1 |Signal  |
|2|GND
|3|Signal
|4|Signal
|5|GND
|6|GND

This stack‑up, combined with stitching vias, short return paths, and careful placement, yielded a robust, fully integrated board with *low EMI emissions. Despite having the motor as close as **5 cm* to sensitive wiring, we can communicate at high baud rates reliably.

<table style="width: 100%; table-layout: fixed;">
  <tr>
    <td>
   The main PCB controls the motors and the servo. It includes a built-in full-bridge motor driver (VNH7070ASTR), an XT60 connector for direct battery input, and protection against overcurrent, overvoltage, surges, and reverse polarity. It also provides UART connections between the Teensy 4.0 and Raspberry Pi 5, plus a header for the servo pins.
    </td>
    <td>
      <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/schemes/pcb-main.jpeg?raw=true" style="width: 100%;">
    </td>
  </tr>
  <tr>
    <td>
The odometry PCB provides a better mounting solution for our OTOS sensor. Because the sensor doesn’t have space for JST headers, this board serves as an intermediate adapter and offers a secure mechanical mount.
    </td>
    <td>
      <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/schemes/pcb-odometry.jpeg?raw=true" style="width: 100%;">
    </td>
  </tr>
  <tr>
    <td>
The Raspberry Pi PCB is designed as a HAT. It offers a more robust way to manage connections than using loose DuPont leads: you can solder jumper wires to it, making the setup more reliable than DuPont jumpers alone.
    </td>
    <td>
      <img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-international/schemes/pcb-rasp.jpeg?raw=true" style="width: 100%;">
    </td>
  </tr>
</table>

---

*Summary:* Separating real‑time control (Teensy 4.0) from high‑level compute (Raspberry Pi 5), providing clean power (isolated Teensy rail + regulated 5 V), and integrating an automotive‑grade driver (VNH7070AS) on a 6‑layer PCB gives us the responsiveness and robustness demanded by the challenge.

## Schematic details

Our PCB design is implemented across three specialized boards to maximize reliability and minimize electromagnetic interference:

### Schematic Overview

<table style="width: 100%; table-layout: fixed;">
  <tr>
    <td>
      <h4>Main PCB</h4>
      <img src="https://github.com/user-attachments/assets/37cf531e-884b-4197-9f4b-ac8e0524a3e2" style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">
      <p style="font-size: 0.9em; margin-top: 0.5em;">Primary power distribution, motor driver (VNH7070AS), and system control.</p>
    </td>
  </tr>
  <tr>
    <td>
      <h4>Odometry PCB</h4>
      <img src="https://github.com/user-attachments/assets/a57949cb-582a-4894-92e9-1005c59148ba" style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">
      <p style="font-size: 0.9em; margin-top: 0.5em;">OTOS sensor mount and signal conditioning.</p>
    </td>
  </tr>
  <tr>
    <td>
      <h4>Raspberry Pi HAT</h4>
      <img src="https://github.com/user-attachments/assets/09b86430-3fb3-458e-897e-d7f9e89d459a" style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">
      <p style="font-size: 0.9em; margin-top: 0.5em;">Robust I/O connections and UART bridge for Pi.</p>
    </td>
  </tr>
</table>

### 1) Battery input & primary protection

-   *Connector:* XT30 (U14/U19) for main power.

-   *Transient suppression:* TVS diodes on the input and 5 V rails (e.g., *SMBJ18* class at the battery side and *SMAJ5.0CA* on the 5 V rail) clamp surges from motor commutation and cable hot‑plugging.

-   *Input filtering:* Bulk electrolytic (≈*470 µF) plus local ceramics (100 nF*) provide low‑ and high‑frequency decoupling. Keep the electrolytic close to the power switch/step‑down input and sprinkle 100 nF near every IC supply pin.

-   *(Recommended)* Add a resettable fuse (polyfuse) sized for your continuous draw and an ideal‑diode OR high‑side FET for reverse‑polarity protection if you expect frequent battery swaps.


### 2) Latching power switch (high‑side)

-   *Function:* A soft‑latching high‑side switch drives a low‑RDS(on) P‑channel MOSFET (*TPH1R712MDL*) to connect/disconnect the main rail.

-   *Control:* A momentary/toggle switch (*SW3*) biases a small driver network (Q3 + resistors) that pulls the MOSFET’s gate. The RC on the gate provides gentle inrush and reduces connector arcing.

-   *Why high‑side:* Keeps grounds common and only switches the positive rail, avoiding ground‑bounce issues with USB/UART connections to the PC.


### 3) 5 V regulation rail (Raspberry Pi + servo)

-   *Module:* Pololu *D42V55F5* (5 V @ up to 6 A). Place it so that current from the battery flows battery → switch → regulator → loads.

-   *Post‑filtering:* An *LC section* (e.g., *L1 = 22 µH, **C9 ≈ 470 µF, plus **100 nF* ceramics) reduces switching ripple seen by the Pi and the servo.

-   *Surge/ESD:* A *SMAJ5.0CA* across the 5 V rail provides additional protection. Keep diode and bulk cap leads short and wide.

-   *Grounding:* Return the regulator ground straight to the *GND plane* and keep the high‑di/dt loop (switch → inductor → diode/cap → switch) as tight as possible.


### 4) Teensy 4.0 section

-   *Role:* Real‑time control (PWM generation, sensor timing, safety interlocks). The Pi handles high‑level logic/vision.

-   *Supply isolation:* The Teensy is fed from a *clean/isolated input* to protect it from motor noise. Decouple 3.3 V with multiple *0.1 µF* and a *10 µF* bulk close to VIN/3V3.

-   *I/O mapping:* Dedicated pins for *PWM* to the motor driver and for *servo. A **UART header (H3)* exposes *TX/RX/GND* for diagnostics.

-   *Signal integrity:* If runs exceed ~10–15 cm, add *33–100 Ω* series resistors on digital lines (IN1/IN2/PWM) to damp edges and reduce ringing into the VNH driver.


### 5) Motor driver stage (VNH7070AS)

-   *Device:*  *VNH7070AS* integrated H‑bridge; internal MOSFETs handle current spikes and integrate flyback paths.

-   *Interface:* Pins *INA/INB/PWM/EN/CS* (nomenclature varies per package) connect to the Teensy. Keep logic ground common with power ground at a single low‑impedance point.

-   *Decoupling:* Place *≥ 470 µF* bulk right at *VCC* of the driver and multiple *100 nF* ceramics. Add an input *TVS (SMBJ18 class)* across motor supply near the driver.

-   *Snubbing:* For very noisy motors, an *RC snubber* (e.g., 100 nF + 1–2 Ω, tuned empirically) across the motor terminals can reduce EMI. Keep motor leads twisted and short.

-   *Thermal:* Provide a solid copper pour under the thermal pad with many *thermal vias* to GND to spread heat to inner planes.


### 6) Connectors & peripherals

-   *Servo header:* Powered from the regulated 5 V rail; keep return path next to the 5 V trace.

-   *Battery sense / telemetry (optional):* If you expose the pack to the MCU, use a *divider* + *RC* and consider *TVS/small **series resistor* to protect the ADC.

-   *NeoPixel header (P1):* Decouple with *100 nF* at the connector and, if long strips are used, a *large electrolytic* (≥1000 µF) at the first LED.


### 7) Layout guidance (applied)

-   *Stack‑up:* 6‑layer with *three GND planes* (L2/L5/L6) provides low impedance return and shields signals.

-   *Star power:* Route battery → switch → regulator → loads with *star‑like branching*; do not daisy‑chain sensitive logic behind motor currents.

-   *Keep loops tight:* Especially the driver’s *switching loop* and the regulator loop. Use wide pours for battery and motor paths.

-   *Segregate zones:* Physically separate *power* (battery, driver, regulator) from *logic* (Teensy, level‑signals). Cross at right angles if they must cross.

-   *Stitching vias:* Surround the driver and the high‑current paths with plenty of GND stitching vias to contain fields and lower EMI (already used here).


### 8) Bring‑up & test checklist

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
- [GitHub Repos](https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025)

---

## License <a name="license"></a>

```
MIT License
Permission is hereby granted, free of charge, to any person obtaining a copy of this software.
```

---

> *Document maintained by Chabots | Last updated: Sept 2025*

<!--stackedit_data:
eyJoaXN0b3J5IjpbMTcyMzM3ODYxNCwtMzc2NTM2MDM5LDM1ND
c4NDQyMCwxMjQ4Mzg0MTM1LC0yODM3NTcxNywtMTMyNzEwNTIy
MywxMjg3Nzk2NjQsLTQ4MTYzMzM4MF19
-->


> Written with [StackEdit](https://stackedit.io/).