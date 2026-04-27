# ChaBots - WRO Future Engineers 2026

  

<!--<img src="https://github.com/chaBotsMX/chaBots-NERV-WRO-Future-Engineers-2025/blob/docs-nacional/v-photos/resources/ChaBotsLogo.png?raw=true" width="250">-->

  
  

## Follow us!

<!-- Facebook -->

<a  href="https://www.facebook.com/chabotsMX/">

<img  src="https://cdn-icons-png.flaticon.com/512/733/733547.png"  width="40"  alt="Facebook">

</a>

<!-- Instagram (con degradado real) -->

<a  href="https://www.instagram.com/chabotsmx/"  target="_blank">

<img  src="https://cdn-icons-png.flaticon.com/512/2111/2111463.png"  width="40"  alt="Instagram">

</a>

<!-- YouTube -->

<a  href="https://www.youtube.com/@chabotsmx1956/videos"  target="_blank">

<img  src="https://cdn-icons-png.flaticon.com/512/1384/1384060.png"  width="40"  alt="YouTube">

</a>

<!-- Página Web (icono de internet) -->

<a  href="https://www.chabots.mx"  target="_blank">

<img  src="https://cdn-icons-png.flaticon.com/512/841/841364.png"  width="40"  alt="Website">

</a>

  

This repository contains the documentation for **ChaBots** participation in the **WRO Future Engineers 2026** category. Our robot was designed and built by a Mexican students team, passionate about robotics and education.

  
  

## 📜 Table of Contents

  

1. 🧑‍💻 [The Team](#the-team)

2. 🎯 [The Challenge](#the-challenge)

3. 🏆 [Experience from Last Year](#experience-from-last-year)

4. 🔬 [R&D](#rd)

5. 💭 [Discussion](#discussion)

6. 🤖 [Robot Overview](#robot-overview)

7. 🔋 [Sense Overview](#sense-overview)

8. ⚙️ [Mobility Management](#mobility-management)

9. 💡 [Electronics](#electronics)

10. 💻 [Code Overview](#code-overview)

11. 🚧 [Obstacle Management](#obstacle-management)

12. 🛠️ [Construction Guide](#construction-guide)

13. 💰 [Cost Report](#cost-report)

14. 📚 [Resources](#resources)

15. ©️ [License](#license)

---

  

## 1. The Team <a name="the-team"></a>

<div  align="center">

<img  src="">

</div>

<div  align="center">

<h2  style="color:#1e90ff; font-size:2.2em; margin-top:0.5em; margin-bottom:0.2em;">

<span  style="color:#222; background:linear-gradient(90deg,#1e90ff,#00c3ff,#00ffb3,#1e90ff);-webkit-background-clip:text;-webkit-text-fill-color:transparent;">We are <b>ChaBots Tuneados</b></span> -[:]

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

  

## 3. Experience from Last Year <a name="experience-from-last-year"></a>

  

### 3.1 What Happened

  

Last year was an incredible experience. We were able to achieve first place nationally and later reached 36th place at the international competition. Beyond the results, we identified several important issues in our robot. We also took notes on what we observed and discussed with other teams, which gave us valuable information to improve almost every aspect of our design.

  

### 3.2 Design Philosophy

  

Our design philosophy has not changed much from last year. Instead, we found better ways to accomplish what we had originally planned.

  

Historically, in this and other competitions, we have preferred to build a new robot from scratch every year. We believe this is one of the best ways to push ourselves to improve, learn from previous mistakes, and avoid repeating old limitations. Because of that, this remains one of our core design principles.

  

Our main design goals are:

  

- Small size

- Reliability

- Avoiding unnecessary overengineering

- Consistency

  

### 3.3 What Went Wrong

  

Our robot had three main issues, which affected us mostly during the obstacle round:

  

- Poor steering design

- Poor camera software and camera ecosystem

- Poor low-speed movement control

  

Our steering design was good enough for the open round. In fact, we were able to complete some runs in only 22 seconds. However, it performed very poorly in the obstacle round because the robot was not able to take tight turns. This was mainly caused by the mechanical design of the steering system. We will explain this in more detail later.

  

The camera software itself was not necessarily terrible, but the entire camera ecosystem was problematic. Last year, we decided to use ROS2, which turned out to be a bad choice for our specific needs. Using ROS2 usually means working with Ubuntu or another Linux distribution instead of simply using Raspberry Pi OS. Although ROS2 can be used on Raspberry Pi OS, it is generally easier to use it with Ubuntu.

  

This decision caused several problems. Our camera required unusual drivers that were not official and had almost no documentation. As a result, we had trouble disabling white balance and automatic brightness. We also experienced significant camera delay, processing delay, and inconsistent image behavior.

  

The movement system was good, but it was not optimal for the obstacle round. We needed precision, but our motors were focused more on speed. Since our motors did not have encoders, we could not control low-speed movement accurately. Even though the motors had good torque, it was not enough to move smoothly at low speeds. This was one of the reasons why we were unable to leave the parking lot last year.

  

## 4. R&D <a name="rd"></a>

  

This year, we spent a lot of time on research and development to solve the issues mentioned above and improve our overall performance.

  

This section will be quite large because we want to include everything we found important and useful, not only for us, but also for other teams.

  

We decided to go back to the basics and review everything we might have missed last year.

  

### 4.1 Mobility

  

The first thing that comes to mind when talking about mobility is motors. Motors are one of the most important parts of any robot. However, we found that even a good motor is not enough without a good steering system.

  

Still, let us start by talking about the motors.

  

#### 4.1.1 Which Motor Should We Use?

  

First, we need to understand what makes one motor different from another.

  

The most important characteristics are:

  

- Speed, measured in RPM

- Torque, usually measured in kg·cm or N·m

- Current consumption, measured in amps

- Weight, measured in grams

- Price

  

The first thing most people consider when choosing a motor is speed. However, speed cannot be analyzed alone. In electric motors, speed is directly related to torque. In general, the more torque a motor has, the less speed it will have. Likewise, the more speed it has, the less torque it will provide.

  

This relationship is known as the torque-speed curve, and it is present in every motor. If we want more torque, we usually have to sacrifice speed.

  

The only way to reduce this limitation is to buy a better motor or use a more powerful one. However, using a more powerful motor usually means higher current consumption.

  

The following table compares some motors we considered. We usually prefer motors from known brands because generic motors often have poor quality and unrealistic specifications.

  



| Model          | Speed (RPM) | Torque (kg/cm) | Current (A) | Weight (g) |voltage| Price ($) |  
|----------------|-------------|----------------|-------------|------------|---|-----------|
| Maxon DCX19    | 600         | 6.5            | 2.0         | 80         |12V| 500       |
| Pololu 25D HP 20.4:1   | 480 | 4.8            | 6.0         | 107        | 6V|37.95      |
| Pololu 25D LP 9.7:1     | 630 | 1.3           | 2.0         | 100        | 6V|33.00      |
| generic (pololu 25D copy)    | 620         | 0.22           | not specified        | 120         |12V|10.00      |
| lego EV3 (EV3 Medium Motor, 45503) | 240-260 (no-load) |~2.2 kg·cm stalled| no-load ~0.10 A, stall ~0.62 A | 42 g | ~9V (powered by EV3) | low ($) |
| Pololu 25D HP 20.4:1 (with encoder)| 500 | 7 kg·cm stalled| no-load 0.30 A, stall 5.00 A | 120 g | 12V  | 56.95 |

  

Before choosing a motor, we need to understand what we need from it. In this case, a robot built for Future Engineers must weigh a maximum of 1.5 kg. Torque is not only important for making the robot move. It also affects:

  

- The ability to accelerate quickly

- The ability to climb small inclines

- The ability to overcome small obstacles

- The ability to maintain speed under load

- The ability to stop quickly

- The ability to reverse quickly and change direction

  

So, how much torque do we need?

  

This is a complex question because it depends on many factors, such as the weight of the robot, wheel friction, track surface, and target speed. However, we can use our experience as a starting point.

  

In our case, we know from experience that we need at least twice the robot’s weight in torque. If our robot weighs 1.5 kg, we need at least 3 kg·cm of torque. This is only a starting point, but it is a useful reference.

  

Regarding RPM, we know that we do not need extremely high speed because the track is small and the robot must be able to stop quickly. For this reason, we can sacrifice some speed in exchange for more torque. A target speed of around 350 to 400 RPM is usually enough.

  

However, this target refers to the actual speed at which we want the motor to run, not necessarily the motor’s rated RPM. It is not ideal to run a motor at its maximum rated speed all the time because torque will be lower and current consumption will be higher. Instead, it is better to choose a motor with a higher rated speed and operate it below its maximum.

  

For example, if we need around 400 RPM, we can choose a motor rated for 500 to 700 RPM. This allows us to run the motor at a lower percentage of its rated speed, which gives us better torque and lower current consumption. As a reference, good-quality motors can usually run at 70% or 80% of their rated RPM without problems. A 600 RPM motor, for example, can be used around 420 RPM reliably.

  

Now that we know the approximate torque and speed we need, we can talk about power consumption.

  

Choosing between a 6 V motor and a 12 V motor mainly depends on the battery we plan to use. If we use a 6 V battery, we should choose a 6 V motor. If we use a 12 V battery, we should choose a 12 V motor.

  

However, in practice, it is not always that simple. LiPo batteries do not provide a constant voltage. For example, a 2S LiPo battery has a nominal voltage of 7.4 V, but it can be fully charged at 8.4 V and drop close to 6 V when it is almost empty.

  

This is usually not a serious problem because most motors can handle a small amount of overvoltage. Undervoltage is also not dangerous for a DC motor; the motor will simply run slower or may not move if the voltage is too low.

  

The real issue is that torque and RPM are directly related to voltage. If the battery voltage drops, the robot may lose torque and speed. This is something to consider when diagnosing movement problems. Sometimes the robot may not move correctly simply because the battery is almost empty.

  

One possible solution is to measure the battery voltage and compensate for it in software to keep the speed more constant. This is not strictly necessary, but it can help a lot.

  

There are two common types of motors we can use: simple DC motors and DC motors with encoders. In many cases, they are the same motors, but with an encoder added. Encoders are important when precision and control are needed.

  

Last year, we completed the challenge without encoders, but we found that having full control over motor speed would allow us to implement features that could help in specific situations, such as leaving the parking lot. We are also preparing for possible new mystery rules this year, so having better control gives us more flexibility.

  

Finally, we need to talk about price. Good-quality motors are expensive, but they are usually worth it because they last longer, perform better, and are more reliable. If you can afford them, it is usually a good investment. However, if you cannot, there are still good options available. Pololu motors, for example, are good enough for many hobby and competition applications.

  

There are much better motors, such as Maxon motors, which are used in industrial and even aerospace applications. However, for many robotics competitions, Pololu motors offer a good balance between price, quality, and performance.

  

It is also important to mention that motor requirements can be calculated. There are many online calculators that can help estimate torque and speed requirements. However, the most important thing is to test the motor under real conditions because theoretical calculations are not always accurate.

  

After considering all of this, we chose the Pololu 25D HP 12 V motor with encoder.

  

Last year, we used the Maxon DCX19, and it worked very well for speed. We were able to complete the open round in less than 20 seconds. Our fastest run was 18 seconds. Even in the tightest case, where all four inner walls were present and there was only around 40 cm of space for the robot to drive, we were able to complete the round in 35 seconds.

  

However, we had many issues in the obstacle round because we did not have good control at low speeds. This is the main reason why we chose motors with encoders this year instead of using the Maxon motors again.

  

The Maxon motors are linear, smooth, and easy to predict. However, with encoders, we can achieve accurate movement through feedback control instead of relying only on the quality and natural behavior of the motor. This also gives us the possibility to move at lower speeds with much better accuracy.

  

#### 4.1.2 Which Motor Should We Use? Servo Edition

  

Why use a servo?

  

We need a simple and reliable steering system, and a servo is perfect for this. We do not need extremely high speed or extreme torque. We only need to turn the wheels accurately and consistently.

  

Most teams already know this, but we want to share our experience. This section will be shorter because choosing a servo is not as complex as choosing a drive motor.

  

Here are some servo options:

  


| Model      | Torque (kg/cm)| Current (A) | Size|Weight (g) |Quality| Price ($) |  
|------------|---------------|-------------|-----|------|---|-----------|
| HS 85MG    | 3             | 1.2         | Small|    21    |Excellent| 40.00      |
| MG996      |  13           | 1.2         | Really big|    66    | Bad|10.00      |
| MG95       |  1.8          | 1.0         | small|    9   | Bad|2.00      |


This is only a comparison between the servo we chose and some generic options. There are many good servo brands, but comparing all of them would make this section too long.

  

We chose the HS-85MG because it is a high-quality servo with metal gears, good torque, and a compact size.

  

The most important factor when choosing a servo is torque. The servo must have enough torque to turn the wheels. If there is too much weight over the steering system, the servo may struggle to move the wheels. Because of this, we need a servo with enough torque for the mechanical load.

  

It is easy to find servos with a lot of torque, but they are usually large. This may not be a problem for every team, but it is a problem for us because we want to keep the robot as low as possible. That is why we chose this servo: it is small, has good torque, and works reliably.

  

The HS-85MG provides around 3.0 kg·cm of torque at 5 V. It is expensive, but in our case, it was worth it. If you need a more affordable option, you can use a generic servo, but make sure to test it carefully before using it in a real competition robot.

  

#### 4.1.3 What Makes a Servo Good Quality?

  

It is easier to understand servo quality by looking at the problems that low-quality servos can have. The first issue is that the specifications are often inaccurate or exaggerated. However, that is only the beginning.

  

Bad-quality servos can have problems such as:

  

- Poor control

- Imprecise movement

- High backlash

- Short lifespan

- Inconsistent behavior under load

  

Servos are usually easier to choose than drive motors. Good servos are common, and even some generic servos can work well. However, there are also many low-quality clones, so it is important to test them before using them in an important project.

  

#### 4.1.4 Integrating Both Motors into a Steering System

  

Now that we have selected the components of the steering system, we can start designing how they will work together.

  

This may be one of the most complex parts of the entire challenge, and it is probably one of the most important mechanical design decisions in the robot.

## 5. Discussion <a name="discussion"></a>

  

### 5.1 Decisions

  

We have decided to create a robot with a reduced size, as compact as possible to be able to maneuver more easily and avoid obstacles without further complication. For this reason, we decided to use TOF sensors instead of lidar, as they take up much less space and have several advantages over lidar. Similarly, we decided against using an OTOS and instead opted for a motor with an encoder to determine the robot's position.

  

### 5.2 Rebuilding the System

  

**New Steering System**:

- The steering system was redesigned to be much more compact, and the angles were calculated to make it an Ackermann steering system, so that it has a much smaller turning radius without the wheels slipping.

  

**New PCBs**:

- We decided to have a main PCB to which all the robot's peripherals are connected.

  

**Testing Infrastructure**:

- Built a complete testing environment that simulates competition conditions

- Developed automated testing scripts for regression testing

- Created comprehensive calibration procedures

  

---

  

## 6. Robot Overview <a name="robot-overview"></a>

  

**Name:** Pistonudo

  

<table  style="width: 100%;">

<tbody>

<tr>

<td>

<center><h4>Front</h4></center>

<img  src=""  style="width: 100%;">

</td>

<td>

<center><h4>Back</h4></center>

<img  src=""  style="width: 100%;">

</td>

</tr>

<tr>

<td>

<center><h4>Left</h4></center>

<img  src=""  style="width: 100%;">

</td>

<td>

<center><h4>Right</h4></center>

<img  src=""  style="width: 100%;">

</td>

</tr>

</tbody>

</table>

  

---

  

## 7. Sense Overview <a name="sense-overview"></a>

  

### 7.1 Pololu VL53L8CX

  

<div>

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/other-components/tof-vl53l8cx.png?raw=true">

</div>

  

**Tech specs:**

  

- 8×8 Multizone detection: Capability to output a 64-zone depth map with a wide 65° diagonal field of view (FoV).

- Extended range: Accurate distance measurement up to 400 cm (4 meters) across all zones.

- High-speed sampling: Operates at frequencies up to 60 Hz for real-time motion tracking.

- Integrated voltage regulation: Onboard regulators and level shifters allowing a wide input voltage range (3.2V to 5V).

- Ambient light immunity: Advanced silicon-on-insulator (SOI) technology for consistent performance under indoor and outdoor lighting.

- Programmable features: Configurable regions of interest (ROI) and autonomous low-power modes for energy efficiency.

- Communication interface: Supports I2C (up to 1 MHz) and SPI (up to 3 MHz) for high-bandwidth data transfer.

- Motion indicator: Built-in motion detection algorithm to identify movement within each specific zone.

  

**Link:** [VL53L8CX](https://www.pololu.com/product/3419)

  

### 7.2 OpenMV H7

  

<div>

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/other-components/camera-H7.png?raw=true">

</div>

  

**Tech specs:**

  

- High-performance MCU: STM32H743VI ARM Cortex-M7 processor running at 480 MHz with 1MB of RAM and 2MB of flash.

- Integrated Image Sensor: Features the OV7725 sensor capable of capturing 640x480 (VGA) images at 75 FPS.

- Advanced Vision Support: Native hardware acceleration for color tracking, face detection, QR/barcode decoding, and optical flow.

- Expandable Storage: Integrated microSD card socket supporting high-speed UHS-I transfers for image and data logging.

- Versatile I/O: Comprehensive set of pins including SPI, I2C, USART, PWM, and 12-bit ADC/DAC for peripheral interfacing.

- Machine Learning Ready: Optimized for running quantized neural networks via TensorFlow Lite for Microcontrollers.

- RGB LED and Flash: Built-in high-intensity RGB LED and two high-power IR LEDs for night vision or low-light conditions.

- Python Programmability: Fully programmable via MicroPython, allowing for rapid prototyping and real-time script execution.

  

**Link:** [OpenMV H7](https://openmv.io/products/openmv-cam-h7?srsltid=AfmBOoo6sZCCZkX54ofHCneaBRR4oAl444A5ZMhdEFs70Z3Cm0ei3gXx)

  

---

  

## 8. Mobility Management <a name="mobility-management"></a>

  

### 8.1 Gearbox

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/gearbox/gearbox-assembly.png?raw=true">

The robot's transmission features a custom-designed gearbox, with the base and gears developed in CAD software and manufactured in-house. For fabrication, the team used a Creality K2 Plus Combo printer, chosen for its reliability in handling engineering-grade materials. The material selected was Polymaker PETG-CF (a carbon-fiber-infused PETG), prized for its high stiffness, dimensional stability, and excellent wear resistance, which are critical for durable mechanical components.

A key design feature is the use of double helical gears. This geometry was chosen over standard spur gears to ensure smoother, quieter power transmission with reduced vibration and superior load distribution. This significantly improves mechanical efficiency and component lifespan.

The drive axle consists of 4 mm steel shafts, which were custom-cut from rod stock. To ensure positive torque transfer from the gearbox to the wheels, the ends of the shafts were manually modified using a Dremel tool to create a "D" shape. This profile prevents slippage between the shaft and the wheel hub, a common failure point in high-torque applications.

  

| Part | Description | Image |

| --- | --- | --- |

| 8.1.1 Pololu 25D Encoder | The 25D Metal Gearmotor serves as the robust mechanical core of the propulsion system, featuring a high-performance 12V brushed DC motor paired with a 9.7:1 metal spur gearbox. This unit was selected for its exceptional balance of compact form factor and dynamic response. The precision-cut metal gears act as the primary mechanism for torque amplification, achieving a versatile output speed of approximately 600 RPM. Furthermore, the integrated quadrature encoder provides real-time feedback for closed-loop control, ensuring the precise velocity and positional accuracy required for the robot’s agile maneuvering and synchronized movement. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/gearbox/gearbox-25d-metal-gearmotor-4.4-9.7-encoder.png?raw=true" style="width: 100%;"></picture> |

| 8.1.2 Base | We designed the base of the gearbox so that the wheel axle is as close as possible to the steering axis in order to make tighter turns. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/gearbox/gearbox-base.png?raw=true" style="width: 100%;"></picture> |

| 8.1.3 Gears | The custom-printed double helical gears transfer power from the motor's gearhead output to the wheel axle. This external gear stage was designed with a 1:1 gear ratio. This configuration was chosen because the Maxon motor's integrated gearhead already provided the ideal speed reduction (down to 600 RPM) and torque multiplication. The 1:1 external gears, therefore, act as a direct power transmission, simplifying the design while perfectly matching the motor's output speed to the drive wheels. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/gearbox/gearbox-gear.png?raw=true" style="width: 100%;"></picture> |

| 8.1.4 Rhine G | The rear traction system features a custom-engineered oversized rhine, specifically designed to compensate for the robot’s geometry and maintain a consistent ground clearance across the chassis. Precision-machined with a dedicated D-shaft coupler, the rim ensures a high-tolerance press-fit onto the motor’s output shaft, eliminating backlash during high-torque maneuvers. Beyond its functional role in leveling the platform, the wheel incorporates an aggressive, aesthetic-driven spoke pattern that aligns with the "tuned" visual identity of the team. This design prioritizes both structural integrity for payload distribution and a high-profile aesthetic that distinguishes the robot’s stance on the field. | <picture style="display: block; margin: 0 auto;"><img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/gearbox/gearbox-rhim-g.png?raw=true" style="width: 100%;"></picture> |

  

### 8.2 Steering System

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-assembly.png?raw=true">

For the steering system, the goal was to simplify the mechanism as much as possible, as this would allow for quick and easy manufacturing. However, we decided to make this an Ackermann system, allowing the inner wheel to have a larger angle than the outer wheel. Thanks to this, we were able to prevent the front wheels from slipping when turning.

  

| Part | Description | Image |

| --- | --- | :---: |

| 8.2.1 Servo HiTEC HS-85MG | We selected the HiTEC HS-85MG for our robot's Ackermann steering system, primarily due to its robust metal gears (MG). Unlike many standard or smaller servos that use plastic gears, the metal gearing provides the significantly enhanced durability and resistance to stripping that our steering mechanism requires. This is crucial for us to handle the mechanical loads, vibrations, and potential impacts inherent in the system's operation. We also find that this servo packs considerable torque and good precision into a compact "mighty mini" form factor, supported by a top ball bearing. This ensures it provides the strength we need to turn the wheels effectively while maintaining accurate steering angles, minimizing the excessive "slop" or backlash we might see in less robust options. For application, this blend of power, durability, and reliable accuracy in a small package makes it a superior choice over servos that could fail or wear quickly under the demands of steering. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-HS-85MG.png?raw=true" style="width: 100%;"> </picture> |

| 8.2.2 Base | We designed the base around the servo, so that everything was symmetrical. We also designed the base to be modular and easily attach to the robot's chassis for easy repairs. We 3D printed the base using carbon fiber filament, as we did all the other robot parts, to increase strength. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-base.png?raw=true" style="width: 100%;"> </picture> |

| 8.2.3 Servo Connector | We designed the servo connector this way because, as an Ackermann system, the wheels needed to be connected independently of each other. If we used a single connector for the wheels, both would have the same turning angle, but by splitting it, each wheel would turn at a different angle. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-servoconnector.png?raw=true" style="width: 100%;"> </picture> |

| 8.2.4 Bracket Connectors | As mentioned above, we used two connectors, one per wheel, so they rotated independently.| <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-bracketconnector.png?raw=true" style="width: 100%;"> </picture> |

| 8.2.5 Stabilizer | This stabilizer bar is to prevent the wheel brackets from having play and to make them more stable. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-stabilizer.png?raw=true" style="width: 100%;"> </picture> |

| 8.2.6 Wheel Bracket | We designed the L-shaped wheel mount so that the wheels could rotate more easily and not collide with the robot's chassis. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-wheelbracket.png?raw=true" style="width: 100%;"> </picture> |

| 8.2.7 Rhine S | This rhine represents the compact iteration of our wheel assembly, specifically engineered to interface with the front wheel brackets. This downsized variant is critical for maintaining the robot’s horizontal alignment, counterbalancing the larger rear drive wheels to achieve a perfectly leveled center of gravity. Designed for low-friction rotation and high maneuverability, the Rhine S utilizes a streamlined profile that reduces rotational inertia. We then used a mold and polyurethane resin a40 to make the rubber. This process is shown in the following video: [WRO FutureEngineers Custom Wheels - chaBots NERV](https://youtu.be/8JH6QCOU_B0) | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/steering-system/steering-system-rhine-s.png?raw=true" style="width: 100%;"> </picture>|

  

### 8.3 Camera Mount

  

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/cam-mount/cam-mount.png?raw=true">

The camera mounting system features a modular design engineered for precise tilt-axis orientation. This adjustable bracket allows for granular control over the vertical field of view, enabling the sensor to be locked at the optimal angle for task-specific vision processing. The assembly utilizes a friction-fit pivot mechanism combined with secure fastening points to ensure the camera maintains its orientation despite high-vibration environments or rapid chassis acceleration. By providing a customizable pitch range, the mount ensures the optical center can be calibrated to detect floor-level markers or distant environmental cues with consistent reliability.

  

| Part | Description | Image |

| --- | --- | :---: |

| 8.3.1 Camera Base | This part is mounted on the chassis with the help of some posts. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/cam-mount/cam-base.png?raw=true" style="width: 100%;"> </picture> |

| 8.3.2 Camera Support | This part holds the camera and is attached to the base with screws to allow manipulation of the clamping angle. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/cam-mount/cam-support.png?raw=true" style="width: 100%;"> </picture> |

  

### 8.4 Chassis

  

| Part | Description | Image |

| --- | --- | :---: |

| 8.4.1 Chassis Base | The chassis is the robot's main structure, as all other systems are mounted on it. A modular design was chosen to facilitate assembly and maintenance. The chassis is made of carbon fiber filament. | <picture style="display: block; margin: 0 auto;"> <img src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/chassis/chassis-base.png?raw=true" style="width: 100%;"> </picture> |

  

### 8.5 Assembly

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-orthogonal.png?raw=true"  alt="Robot Assembly Diagram">

  

The steering system is mounted on the chassis using 20mm-high M3 posts. The gearbox is mounted directly to the rear of the chassis, and the main PCB is mounted on it using 30mm-high M3 posts. The OpenMV H7 camera base is mounted on the main PCB using 30mm-high M3 posts. The TOF sensors are connected on their PCBs which are mounted using 3D printed supports.

  

<table  style="width: 100%;">

<tbody>

<tr>

<td><img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-front.png?raw=true"  style="width: 100%;"></td>

<td><img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-asssembly-back.png?raw=true"  style="width: 100%;"></td>

</tr>

<tr>

<td><img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-left.png?raw=true"  style="width: 100%;"></td>

<td><img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-right.png?raw=true"  style="width: 100%;"></td>

</tr>

<tr>

<td><img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-top.png?raw=true"  style="width: 100%;"></td>

<td><img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/assemblys/v-assembly-bottom.png?raw=true"  style="width: 100%;"></td>

</tr>

</tbody>

</table>

  

---

  

## 9. Electronics <a name="electronics"></a>

  
  

### 9.1 Precision Improvements

  

#### 9.1.1 Distance Sensing Upgrade

  

We transitioned from the **RPLIDAR C1 Lidar Sensor** to four **VL53L8CX Time-of-Flight (ToF) sensors**.

  

-  **RPLIDAR C1**

- 360° field of view

- Maximum frequency: ~10 Hz (practically ~5 Hz usable)

- Minimum sensing distance: ~5 cm

-  **VL53L8CX (x4)**

- Narrow field of view (~45° per sensor, depending on configuration)

- Minimum sensing distance: ~5 cm

- 8×8 zone depth grid per sensor

- Maximum frequency: 60 Hz (practically ~50 Hz usable)

- Very compact form factor

  

While the LIDAR provided full 360° environmental coverage, the ToF sensors have a significantly narrower field of view, making their coverage comparatively limited. However, by strategically placing multiple sensors surrounding the robot and leveraging their much higher refresh rate, the overall system achieves faster and more responsive distance measurements. In our use case, the reduced field of view is almost negligible compared to the gain in update speed and reliability.

  

----------

  
  
  

### 9.2 Size and Compactness

  

A major constraint this year was the significantly smaller robot frame.

  

- A custom PCB was designed and reduced to **8 × 8 cm**

- The design evolved from **multiple PCBs** to a **single integrated board**, simplifying wiring and reducing failure points

- This is our **first full single-board implementation**, representing a major step forward in system integration

  

Initially, the PCB was intended to occupy the robot base. However, due to mechanical constraints and limited usable space, the design was changed to a **top-mounted configuration**.

  

This resulted in:

  

- More efficient use of space

- Reduced wiring complexity

- A cleaner and more organized layout

  

----------

  

### 9.3 Serviceability Improvements

  

Mounting the PCB on top of the robot significantly improved accessibility:

  

- All components are now **fully accessible**, including:

- Microcontrollers

- Sensors

- Power systems

  

This allows for:

  

- Faster repairs and adjustments

- Easier testing and iteration

- Reduced downtime during development

  

----------

  

### 9.4 Debugging and Feedback Systems

  

#### 9.4.1 Previous Year

  

- Required an external monitor via HDMI to interface with the Raspberry Pi 5

- Bulky and inefficient for rapid testing

  

#### 9.4.2 Current System

  

We implemented multiple onboard debugging and feedback tools:

  

-  **Buzzer** → audible system feedback

-  **Programmable RGB LED** → system state indication

-  **Dedicated indicator LEDs** for:

- Motor driver

- Servo activity

- Power (ON state)

  

Additionally:

  

- The **OpenMV camera** can be connected via **Micro USB**

- Full debugging and calibration is done through the **OpenMV IDE** on a laptop

  

#### 9.4.3 Wireless Debugging (Development Only)

  

We also integrated an **XIAO C6 microcontroller** for **wireless debugging during development**:

  

- Provides live telemetry and sensor data

- Enables real-time monitoring without physical connections

- Used **only during programming and testing**, not during official competition runs

  

This greatly improves development speed while keeping the competition system simple and reliable.

  

----------

  

### 9.5 IMU Upgrade

  

We upgraded from the **BNO055** to the **BNO085**.

  

#### 9.5.1 Improvements

  

- Communication switched from **I2C → UART**

- More robust and reliable communication

- Significantly reduced drift

- Minimal calibration required for our use case

  

Although UART integration is slightly more complex, the reliability improvements justified the change.

  

----------

  

### 9.6 Summary

  

This year’s electronics system represents a major step forward in:

  

-  **High-speed sensing and responsiveness**

-  **Compact, single-board design**

-  **Improved accessibility and serviceability**

-  **Efficient and modern debugging tools**

  

The system is also designed with **future upgrades in mind**, particularly the easy transition to the OpenMV N6 once firmware limitations are resolved.

  

### 9.7 Microcontroller Selection

  
  

|MCU |Clock speed | Observed PWM behavior|
|--|--|--|
| Teensy 4.0 | 600 MHz |Excellent|
|Arduino Nano|16 MHz|Good|
|Raspberry Pi Pico|133 MHz|Inconsistent at low duty|
|Seeed Xiao C6|120 MHz|Good + BLE|

  
  

*Notes on PWM behavior: Using the same nominal PWM frequency across all three MCUs, we observed different motor responses. On the Teensy 4.0, the motor starts smoothly and can deliver more torque at lower duty cycles. The Raspberry Pi Pico struggled at very low speeds and required careful tuning. The Arduino Nano behaved as expected for an AVR‑based board. We could not find definitive documentation explaining these differences; our working theory is that PWM implementation details (SDK, drivers, timer resolution, and library quality) play a role. Teensy and Arduino platforms are primarily C/C++ with mature vendor‑maintained timer libraries, while typical Pico workflows often lean on MicroPython or community libraries, which may trade convenience for timing granularity.

  

### 9.8 Power Architecture

  

-  *Main battery:* 3S (11.1 V) *LiPo, ~1000 mAh. Powers motor drivers and 5V regulator.

-  *Regulation:* Pololu *S8V9F5*5V 1,5A Step-Up/Step-Down Voltage Regulator. Powers Teensy 4.0, Seeed Xiao C6, motor encoders, RGB LED and the servo.

- Both the Teensy 4.0 and Seeed Xiao C6 regulate 3.3V to interface with sensors

  

### 9.9 Motor Driver

  

We use the *VNH7070AS* full‑bridge driver. It comfortably handles the required current (≈5A continuous in our use case) and supports supply voltages above 20 V, providing generous headroom. Because there is no widely available off‑the‑shelf module for this device, we integrated it directly onto our PCB. This raises the bar for assembly, but the result is more reliable and, in practice, more cost‑effective.

#### 9.9.1 Vision System Redesign

  

Our vision systen is something we realy struggled with last year so we decided to move away from a **Raspberry Pi 5 + Camera Module v3** setup to a fully integrated microcontroller-based vision system.

  

##### 9.9.1.1 Camera Platforms Considered

  
  

| Device | Processing Type | Approx. Current Draw | Max FPS | Notes |
|--|--|--|--|--|
| Raspberry Pi 5 + Camera V3 | SBC (Linux) | ~2–5 A @ 5V | ~60+ FPS | High performance, high power consumption |
| OpenMV N6 (planned) | MCU | ~140–200 mA | ~240FPS | New, compact, efficient |
| OpenMV H7 (used) | MCU | ~120–180 mA | ~80 FPS | Stable and well-supported |
| MaixCam Lite (considered) | AI SoC | ~300–500 mA | ~60 FPS (sensor-limited) | Higher compute, limited documentation |

  
  

##### 9.9.1.2 Final Decision

  

- The **OpenMV N6** was initially selected due to its:

- Low power consumption

- Small size

- Simplified embedded workflow

- However, due to firmware limitations (manual exposure and white balance control not available), we transitioned to the **OpenMV H7**, which provided reliable and predictable performance.

- The **MaixCam Lite** was ultimately not used due to:

- Limited documentation

- Less mature ecosystem

- Frame rate limitations imposed by its image sensor

  

**Future Upgrade Path:**

The system is designed so that switching from the H7 to the N6 can be done almost instantly once stable firmware becomes available.

  

----------

  

### 9.10 Odometry System Redesign

  

We transitioned away from the **SparkFun OTOS optical tracking sensor** due to reliability issues observed during testing:

  

- Significant drift over time

- Inconsistent readings depending on speed

- Failure to reliably detect movement at low speeds

- Poor accuracy at higher speeds

  

These limitations made it unsuitable for precise and repeatable odometry.

  

#### 9.10.1 New Approach

  

We replaced the OTOS with a combined system using:

  

-  **BNO085 IMU (UART)**

-  **Pololu 25D motors with integrated encoders**

  

This hybrid approach provides a much more reliable solution:

  

-  **Encoders** → Accurate wheel-based distance measurement

-  **BNO085** → Stable orientation tracking with minimal drift

-  **Sensor fusion** → Improved overall position estimation

  

#### 9.10.2 Results

  

- Consistent tracking across all speed ranges

- Reliable detection of small movements

- Significantly reduced drift compared to the previous system

- More predictable and tunable behavior

  

This change resulted in a robust and competition-ready odometry system, better suited for precise navigation and control.

  
  

### 9.11 PCB Design

  

For the first iteration of this robots PCB we opted for a simple 2 layer board, as it made it much easier to diagnose and fix any problems that might arise. However, by using ground pour on both layers combined with stitching vias, short return paths, and careful placement, we were able to produce a fully integrated board with low EMI emissions. Despite still having the motor as close as *5 cm* to sensitive wiring, we can still communicate at high baud rates reliably.

  

<table  style="width: 100%; table-layout: fixed;">

<tr>

<td>

The main PCB intrgrates two microcontrollers (Teensy 4.0 and Seeed Xiao C6), a built-in full-bridge motor driver (VNH7070ASTR), servo control and all sensors. It includes , an XT30 connector for direct battery input, and protection against overcurrent, overvoltage, surges, and reverse polarity.

</td>

<td>

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Main-PCB.png"  style="width: 100%;">

</td>

</tr>

</table>

  

---

### 9.12 Schematic Details

  

#### 9.12.1 Schematic Overview

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Full.png"  style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">

<table  style="width: 100%; table-layout: fixed;">

<tr>

<td>

<h4>9.12.2 Microcontrollers</h4>

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Microcontrollers.png"  style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">

<p  style="font-size: 0.9em; margin-top: 0.5em;">Teensy 4.0 and Seeed Xiao C6 logic connexions.</p>

</td>

</tr>

<tr>

<td>

<h4>9.12.3 Power Delivery</h4>

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Power%20Delivery.png"  style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">

<p  style="font-size: 0.9em; margin-top: 0.5em;">Battery Conector XT30 , Slide Switch 6A 12V, Voltage Regulator S8V9F5 5V 1.5A, Filtered Servo Output and Motor Output. </p>

</td>

</tr>

<tr>

<td>

<h4>9.12.4 Debugging</h4>

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Debugging.png"  style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">

<p  style="font-size: 0.9em; margin-top: 0.5em;">Buzzer, On LED, Button and Programable LED.</p>

</td>

</tr>

<tr>

<td>

<h4>9.12.5 Sensors</h4>

<img  src="https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026/blob/main/models/PCBs/Sensors.png"  style="width: 100%; border: 1px solid #ddd; border-radius: 5px;">

<p  style="font-size: 0.9em; margin-top: 0.5em;">IDC conectors for our TOF sensors through I2C and Camera through UART. BNO085 mounted directly to PCB.</p>

</td>

</tr>

</table>

  

### 9.13 Teensy 4.0 Section

  

-  *Role:* Real‑time control (PWM generation, sensor timing, safety interlocks). The Pi handles high‑level logic/vision.

  

-  *Supply isolation:* The Teensy is fed from a *clean/isolated input* to protect it from motor noise. Decouple 3.3 V with multiple *0.1 µF* and a *10 µF* bulk close to VIN/3V3.

  

-  *I/O mapping:* Dedicated pins for *PWM* to the motor driver and for *servo. A **UART header (H3)* exposes *TX/RX/GND* for diagnostics.

  

-  *Signal integrity:* If runs exceed ~10–15 cm, add *33–100 Ω* series resistors on digital lines (IN1/IN2/PWM) to damp edges and reduce ringing into the VNH driver.

  
  

### 9.14 Motor Driver Stage (VNH7070AS)

  

-  *Device:*  *VNH7070AS* integrated H‑bridge; internal MOSFETs handle current spikes and integrate flyback paths.

  

-  *Interface:* Pins *INA/INB/PWM/EN/CS* (nomenclature varies per package) connect to the Teensy. Keep logic ground common with power ground at a single low‑impedance point.

  

-  *Decoupling:* Place *≥ 470 µF* bulk right at *VCC* of the driver and multiple *100 nF* ceramics. Add an input *TVS (SMBJ18 class)* across motor supply near the driver.

  

-  *Snubbing:* For very noisy motors, an *RC snubber* (e.g., 100 nF + 1–2 Ω, tuned empirically) across the motor terminals can reduce EMI. Keep motor leads twisted and short.

  

-  *Thermal:* Provide a solid copper pour under the thermal pad with many *thermal vias* to GND to spread heat to inner planes.

  
  

### 9.15 Connectors & Peripherals

  

-  *Servo header:* Powered from the regulated 5 V rail; keep return path next to the 5 V trace.

  

-  *Battery sense / telemetry (optional):* If you expose the pack to the MCU, use a *divider* + *RC* and consider *TVS/small **series resistor* to protect the ADC.

  

-  *NeoPixel header (P1):* Decouple with *100 nF* at the connector and, if long strips are used, a *large electrolytic* (≥1000 µF) at the first LED.

  
  

### 9.16 Bring-Up & Test Checklist

  

1. Power the board with a *current‑limited bench supply* (e.g., 0.5–1 A) and verify no abnormal draw.

  

2. Measure *5 V* rail at the Pi and servo connector under light load.

  

3. With the motor disconnected, toggle the power switch—check gate voltage of the high‑side MOSFET for clean transitions.

  

4. Connect a small DC motor and run *10–20% PWM; scope **VCC* and *GND* near the driver for spikes.

  

5. Increase load gradually; verify the driver’s *thermal pad* stays below spec and that bulk caps do not heat.

  

6. Verify UART debug works; confirm CS/FAULT lines (if used) change as expected under stall or overcurrent tests.

  

----------

## 10. Code Overview <a name="code-overview"></a>

  

The robot’s code was completely remade from last year. This change was necessary because the system architecture is now different, but also because last year’s software was not as well structured as we wanted.

  

Last year, the robot used a Raspberry Pi as the main processing unit, while the Teensy 4.0 worked mostly as a low-level slave for motor control. This year, we moved to a simpler and more deterministic architecture. The Teensy 4.0 is now the main controller, while the Seeed Studio XIAO ESP32-C6 is used as an auxiliary microcontroller for sensor reading and wireless debugging during development.

  

For this year’s code, we are focusing on improving every aspect of the software, especially:

  

- Better code structure

- More reliable sensor communication

- Cleaner libraries

- More consistent variable names

- Easier debugging

- Better separation between sensing, control, and actuation

  

The current code is still being improved, but it already includes several important features.

  

---

  

### 10.1 Seeed Studio XIAO ESP32-C6

  

The main reason for adding this microcontroller was to make the robot easier to debug using wireless communication, especially during testing. However, while testing the new VL53L8CX ToF sensors, we found an unexpected issue.

  

For a reason we still do not fully understand, the ToF sensors did not work reliably with the Teensy 4.0. We tried several software changes, different wiring configurations, and even a direct connection between a single ToF sensor and the Teensy, but the problem remained.

  

As a final test, we tried using the XIAO ESP32-C6 instead of the Teensy, and the sensors worked correctly. Because of this, we modified the PCB connections by adding bridges from the original Teensy ToF connections to available pins on the XIAO ESP32-C6.

  

Right now, the XIAO ESP32-C6 reads the ToF sensors and sends the distance data to the Teensy through UART. The Teensy remains the main controller of the robot.

  

The current XIAO code initializes the four VL53L8CX sensors, reads the front, left, and right distances, and sends them through UART using a simple binary packet.

  

```cpp

#include  "TOF4Walls.h"

#include  "Wire.h"

  

constexpr  int  LPN_FRONT = 2;

constexpr  int  LPN_BACK = 3;

constexpr  int  LPN_LEFT = 0;

constexpr  int  LPN_RIGHT = 1;

  

TOF4Walls  tofs(Wire, LPN_FRONT, LPN_BACK, LPN_LEFT, LPN_RIGHT);

  

void  setup() {

pinMode(13, OUTPUT);

digitalWrite(13, LOW);

  

Serial.begin(115200);

Serial1.begin(2000000, SERIAL_8N1, D7, D6);

  

delay(1000);

Serial.println("Starting...");

  

if (!tofs.begin(30)) {

Serial.println("Error initializing VL53L8CX sensors");

while (true) { }

}

  

Serial.println("TOF sensors ready");

}

  

void  loop() {

tofs.update();

  

if (tofs.hasFreshData(TOF4Walls::FRONT)) {

uint16_t  front = (uint16_t)tofs.getDistance(TOF4Walls::FRONT);

uint16_t  left = (uint16_t)tofs.getDistance(TOF4Walls::LEFT);

uint16_t  right = (uint16_t)tofs.getDistance(TOF4Walls::RIGHT);

  

Serial.print("F: ");

Serial.print(front);

Serial.print(" L: ");

Serial.print(left);

Serial.print(" R: ");

Serial.println(right);

  

Serial1.write(0xAA); // Start byte

  

Serial1.write(lowByte(front));

Serial1.write(highByte(front));

  

Serial1.write(lowByte(left));

Serial1.write(highByte(left));

  

Serial1.write(lowByte(right));

Serial1.write(highByte(right));

}

}

```

  

This code is currently working, but we have already found some issues in this iteration. We plan to keep improving it, especially the communication protocol, sensor validation, and error handling.

  

---

  

### 10.2 Teensy 4.0

  

The Teensy 4.0 is the main controller of the robot. It receives data from the XIAO ESP32-C6, reads the IMU, controls the steering servo, drives the main motor, and manages the robot’s navigation logic.

  

The current navigation code works mainly as a wall follower. We chose this approach because it is simpler, more reliable, and easier to tune than last year’s method. It also allows the robot to adapt to different track configurations, since the robot always follows a path based on the nearest available wall.

  

The Teensy code currently includes:

  

- UART packet reading from the XIAO ESP32-C6

- ToF data validation

- Continuous distance estimation using encoder and IMU data

- Wall following using ToF and IMU correction

- Automatic turn direction selection

- Left-wall or right-wall following depending on the detected direction

- Servo steering control

- Dynamic speed control

- Final stop logic after completing the required number of turns

  

---

  

### 10.2.1 ToF UART Communication

  

The Teensy receives ToF data from the XIAO ESP32-C6 through UART. Each packet starts with `0xAA`, followed by three 16-bit distance values:

  

- Front distance

- Left distance

- Right distance

  

The function also filters invalid readings. If a distance is greater than `3000 mm`, it is discarded and the previous valid value is preserved.

  

```cpp

bool  readTOFPacket(HardwareSerial  &port, uint16_t  &front, uint16_t  &left, uint16_t  &right) {

static  uint8_t  state = 0;

static  uint8_t  buffer[6];

static  uint8_t  index = 0;

  

while (port.available()) {

uint8_t  c = port.read();

  

switch (state) {

case  0:

if (c == 0xAA) {

index = 0;

state = 1;

}

break;

  

case  1:

buffer[index++] = c;

  

if (index >= 6) {

uint16_t  newFront = (uint16_t)buffer[0] | ((uint16_t)buffer[1] << 8);

uint16_t  newLeft = (uint16_t)buffer[2] | ((uint16_t)buffer[3] << 8);

uint16_t  newRight = (uint16_t)buffer[4] | ((uint16_t)buffer[5] << 8);

  

if (newFront <= TOF_MAX_VALID_MM) front = newFront;

if (newLeft <= TOF_MAX_VALID_MM) left = newLeft;

if (newRight <= TOF_MAX_VALID_MM) right = newRight;

  

state = 0;

return  true;

}

break;

}

}

  

return  false;

}

```

  

This makes the communication more robust because corrupted or unrealistic readings do not immediately affect the robot’s movement.

  

---

  

### 10.2.2 Encoder and IMU-Based Distance Estimation

  

The robot does not rely only on fresh ToF readings. Between sensor updates, it estimates the front, left, and right distances using the motor encoder and the IMU heading.

  

This helps the robot react more smoothly instead of waiting for a new ToF packet every time.

  

```cpp

void  updateEstimatedDistances(int  yaw) {

static  int32_t  lastTicks = 0;

  

int32_t  ticks = getEncoderTicks();

int32_t  deltaTicks = ticks - lastTicks;

lastTicks = ticks;

  

if (!distInitialized || deltaTicks == 0) return;

  

float  ds = (float)deltaTicks * MM_PER_TICK;

  

float  alpha = radians((float)wrap180(yaw - setPointIMU));

  

float  dForward = ds  *  cos(alpha);

float  dLeft = ds  *  sin(alpha);

  

estFront -= dForward;

estRight += dLeft;

estLeft -= dLeft;

  

estFront = constrain(estFront, 0.0f, (float)TOF_MAX_VALID_MM);

estLeft = constrain(estLeft, 0.0f, (float)TOF_MAX_VALID_MM);

estRight = constrain(estRight, 0.0f, (float)TOF_MAX_VALID_MM);

}

```

  

When a new ToF packet arrives, the estimated distances are corrected using the real sensor data.

  

```cpp

if (newTof) {

syncEstimateFromCurrentDistances();

}

```

  

---

  

### 10.2.3 Wall Following

  

The robot follows either the right wall or the left wall depending on the detected direction of the track.

  

At the beginning, the robot assumes right-wall following. After the first turn, it decides whether the track direction is clockwise or counterclockwise. If the robot detects that the correct direction is clockwise, it switches to left-wall following.

  

```cpp

bool  followingLeftWall() {

return  directionChosen && turnDirection == -1;

}

  

uint16_t  getFollowWallDistance() {

if (followingLeftWall()) {

return  left;

}

  

return  right;

}

```

  

The servo correction changes depending on which wall is being followed.

  

```cpp

int  getWallServoSign() {

if (followingLeftWall()) {

return +1;

}

  

return -1;

}

```

  

This allows the same controller to work in both clockwise and counterclockwise track directions.

  

---

  

### 10.2.4 Steering Control

  

The steering system combines IMU correction and wall distance correction.

  

The IMU correction keeps the robot aligned with the target heading, while the wall correction keeps the robot at the desired distance from the wall.

  

```cpp

void  updateSteering(int  yaw) {

errorIMU = angleErrorDeg(setPointIMU, yaw);

  

float  dtIMU = max(1.0f, (float)lastReadIMU);

float  dErrIMU = (errorIMU - lastErrorIMU) / dtIMU;

float  imuTerm = kpIMU * errorIMU + kdIMU * dErrIMU;

  

lastErrorIMU = errorIMU;

lastReadIMU = 0;

  

float  wallTerm = 0.0f;

uint16_t  wallDist = getFollowWallDistance();

  

bool  useWall = wallDistanceAllowedForSteering(wallDist);

  

if (useWall) {

float  limitedAngle = constrain((float)errorIMU, -35.0f, 35.0f);

float  theta = radians(limitedAngle);

  

float  wallCorrected = (float)wallDist * cos(theta);

  

int  errorWall = (int)wallCorrected - setPointWall;

errorWall = constrain(errorWall, -maxWallError, maxWallError);

  

float  wallWeight = 1.0f - min(abs(errorIMU) / 40.0f, 1.0f);

  

float  dtWall = max(1.0f, (float)lastReadWall);

float  dErrWall = (errorWall - lastErrorWall) / dtWall;

  

wallTerm = wallWeight * (kpWall * errorWall + kdWall * dErrWall);

  

lastErrorWall = errorWall;

lastReadWall = 0;

} else {

lastErrorWall = 0.0f;

lastReadWall = 0;

}

  

int  wallServoSign = getWallServoSign();

  

int  servoCmd = constrain(

(int)(servoCenter + imuTerm + wallServoSign * wallTerm),

servoMin,

servoMax

);

  

outError = abs(90 - servoCmd);

servo.write(servoCmd);

}

```

  

The wall correction is reduced when the IMU error is too large. This prevents the robot from making aggressive wall-following corrections while it is still misaligned.

  

---

  

### 10.2.5 Wall Jump Protection

  

Before the robot knows the real track direction, it may temporarily follow the wrong wall. If this happens, the measured wall distance can suddenly jump from a normal value to a very large value.

  

To avoid a sudden steering reaction, the code detects large jumps in wall distance and temporarily ignores the wall correction. In that case, the robot uses only the IMU to keep moving smoothly.

  

```cpp

bool  wallDistanceAllowedForSteering(uint16_t  wallDist) {

if (!directionChosen) {

if (lastWallDistValidForJump) {

int  jump = abs((int)wallDist - (int)lastWallDistForJump);

  

if (jump > WALL_JUMP_THRESHOLD_MM) {

return  false;

}

}

  

if (wallDist >= wallMinValid && wallDist <= wallMaxValid) {

lastWallDistForJump = wallDist;

lastWallDistValidForJump = true;

}

}

  

if (wallDist < wallMinValid || wallDist > wallMaxValid) {

return  false;

}

  

return  true;

}

```

  

This feature makes the robot more stable during the first lap, especially when the correct wall-following side has not been selected yet.

  

---

  

### 10.2.6 Automatic Direction Selection

  

When the robot reaches the first corner, it stops briefly and compares the left and right distances. Based on the side with more available space, it chooses the turn direction.

  

```cpp

void  finishDirectionChoiceAndStartTurn() {

choosingDirection = false;

directionChosen = true;

  

if (left >= right) {

turnDirection = 1; // Counterclockwise: turn left and follow right wall

tone(BUZZER, 1500, 100);

} else {

turnDirection = -1; // Clockwise: turn right and follow left wall

tone(BUZZER, 700, 100);

}

  

resetWallJumpFilter();

  

startTurnByDirection();

turnCount++;

}

```

  

After this first decision, the robot remembers the selected direction for the rest of the run.

  

---

  

### 10.2.7 Turn Execution

  

Turns are handled by updating the IMU setpoint by 90 degrees in the selected direction.

  

```cpp

void  startTurnByDirection() {

setPointIMU = wrap180(setPointIMU + (90 * turnDirection));

turning = true;

turnArmed = false;

  

resetWallJumpFilter();

}

```

  

During the turn, the robot uses the IMU error to steer toward the new target heading.

  

```cpp

void  updateTurn(int  yaw) {

int  turnError = angleErrorDeg(setPointIMU, yaw);

  

int  servoCmd = constrain(

(int)(servoCenter + kpTurn * turnError),

servoMin,

servoMax

);

  

servo.write(servoCmd);

  

if (abs(turnError) <= turnFinishError) {

turning = false;

}

}

```

  

This makes the turn behavior independent from the wall distance while the robot is rotating.

  

---

  

### 10.2.8 Speed Control

  

The robot adjusts its speed depending on the front distance and the current steering angle. If the steering angle is large, the robot reduces speed to avoid unstable turns.

  

```cpp

int  calcSpeed() {

if (!turning) {

int  reductionError = outError * rP;

float  disVel = constrain(front / 2000.0f, 0.0f, 1.0f) * 70.0f;

int  outSpeed = 100 + (int)disVel - reductionError;

  

return  constrain(outSpeed, minPWM, maxPWM);

} else {

return  80;

}

}

```

  

This keeps the robot faster on straight sections and slower during sharper corrections.

  

---

  

### 10.2.9 Final Stop Logic

  

After completing the expected number of turns, the robot does not stop immediately. Instead, it moves forward for 1.5 seconds and then stops.

  

```cpp

else  if (turnCount == 12 && !turning) {

if (!finalAdvanceStarted) {

finalTimer = 0;

finalAdvanceStarted = true;

}

  

if (finalTimer < 1500) {

updateSteering(imuRead);

motorForwardPWM(calcSpeed());

}

else {

finish = 1;

}

}

```

  

This prevents the robot from stopping exactly at the end of a turn and allows it to finish in a more controlled position.

  

---

  

### 10.3 Current Software Architecture

  

The current software architecture can be summarized as follows:

  

```text

VL53L8CX ToF Sensors

↓

Seeed Studio XIAO ESP32-C6

↓ UART, 2 Mbps

Teensy 4.0

├── Reads IMU through UART

├── Reads encoder using Encoder.h

├── Estimates distances between ToF updates

├── Executes wall-following control

├── Controls steering servo

└── Controls main drive motor

```

  

This architecture keeps the control loop on the Teensy while moving the ToF sensor handling to the XIAO ESP32-C6.

  

---

  

### 10.4 Change Log

  

#### 25/04/2026

  

- The base code is working.

- Fixed several inconsistencies in variable names.

- Added new helper functions and libraries.

- Added UART communication between the XIAO ESP32-C6 and the Teensy 4.0.

- Added ToF filtering to discard readings above `3000 mm`.

- Added encoder and IMU-based distance estimation.

- Added automatic direction selection.

- Added support for switching between right-wall and left-wall following.

- Added final forward movement before stopping.
	
  

## 11. Obstacle Management <a name="obstacle-management"></a>

  

The robot's obstacle management system (`teensy_obs_node`) implements a sophisticated multi-sensor approach for autonomous navigation in environments with colored obstacles. This system integrates LIDAR, odometry, and computer vision to provide intelligent obstacle detection and avoidance capabilities operating at 100 Hz (10ms cycle).

  


## 11. Obstacle Management <a name="obstacle-management"></a>

  

The robot’s obstacle management system is handled directly by the Teensy 4.0. The system combines color detection from the OpenMV camera, distance measurements from the VL53L8CX ToF sensors, and heading feedback from the IMU.

  

Unlike last year’s system, this version does not use LIDAR, OTOS, Raspberry Pi, or PiCamera2. The goal of this redesign was to make the obstacle logic simpler, faster, and easier to debug.

  

The current system uses:

  

-  **OpenMV camera** for red and green obstacle detection

-  **VL53L8CX ToF sensors** for front, left, and right distance measurements

-  **IMU** for heading control and turn execution

-  **Teensy 4.0** as the main controller for decision-making, steering, and motor control

  

---

  

### 11.1 Detection Methods

  

The robot uses two main sensing systems during the obstacle round.

  

#### 11.1.1 Vision Detection

  

The OpenMV camera detects colored obstacles and sends the following data to the Teensy:

  

-  **Color ID**

-  `1` = Red obstacle

-  `2` = Green obstacle

-  **X position** of the detected blob

-  **Y position** of the detected blob

  

The Teensy does not receive the obstacle distance directly from the camera. Instead, it uses the horizontal position of the blob to calculate an approximate relative angle.

  

#### 11.1.2 Distance Detection

  

The VL53L8CX ToF sensors provide the robot with three main distance readings:

  

-  **Front distance**

-  **Left distance**

-  **Right distance**

  

These distances are used to detect walls, confirm turns, prevent collisions, and avoid getting too close to the sides of the track.

  

#### 11.1.3 Heading Detection

  

The IMU provides the current yaw angle of the robot. This is used to:

  

- Keep the robot aligned with the current target heading

- Execute 90-degree turns

- Detect when a turn has finished

- Recover if the robot gets stuck during a turn

  

---

  

### 11.2 Multi-Sensor Data Processing

  

#### 11.2.1 OpenMV Integration

  

The OpenMV sends a binary packet through UART to the Teensy.

  

**OpenMV Packet Format:**

  

```text

[START] [Color_ID] [X_Low] [X_High] [Y_Low] [Y_High] [END]

0xAA uint8 uint8 uint8 uint8 uint8 0x55

```

  

The Teensy reads this packet and stores the detected color and blob position.

  

```cpp

struct  OpenMVBlob {

uint16_t  x = 0;

uint16_t  y = 0;

uint8_t  colorId = 0;

bool  valid = false;

};

```

  

The X position is converted into an approximate angle using the camera center and the configured field of view.

  

```cpp

float  getBlobAngleDeg(const  OpenMVBlob  &blob) {

int  dx = (int)blob.x - CAM_CENTER_X;

  

float  angle = ((float)dx * CAM_HALF_FOV_DEG) / (float)CAM_CENTER_X;

  

if (CAM_INVERT_X) {

angle = -angle;

}

  

return  constrain(angle, -CAM_HALF_FOV_DEG, CAM_HALF_FOV_DEG);

}

```

  

A camera detection is considered valid only for a short time. If no new packet is received after `300 ms`, the detection is ignored.

  

```cpp

bool  hasFreshBlob() {

return  camBlob.valid && lastCamPacket <= CAM_TIMEOUT_MS;

}

```

  

This prevents the robot from reacting to old obstacle detections.

  

---

  

#### 11.2.2 ToF Integration

  

The ToF data is received by the Teensy through UART from the XIAO ESP32-C6.

  

**ToF Packet Format:**

  

```text

[START] [Front_Low] [Front_High] [Left_Low] [Left_High] [Right_Low] [Right_High]

0xAA uint8 uint8 uint8 uint8 uint8 uint8

```

  

The packet contains three 16-bit distance values in millimeters.

  

- Front distance

- Left distance

- Right distance

  

Any reading above `3000 mm` is considered invalid and discarded. When this happens, the previous valid value is preserved.

  

```cpp

if (newFront <= TOF_MAX_VALID_MM) front = newFront;

if (newLeft <= TOF_MAX_VALID_MM) left = newLeft;

if (newRight <= TOF_MAX_VALID_MM) right = newRight;

```

  

This prevents corrupted or unrealistic ToF readings from affecting the robot’s movement.

  

---

  

#### 11.2.3 IMU Integration

  

The IMU is connected to the Teensy through UART. Its yaw value is converted to the `-180°` to `180°` range.

  

```cpp

int  wrap180(int  ang) {

while (ang > 180) ang -= 360;

while (ang <= -180) ang += 360;

return  ang;

}

```

  

The robot calculates heading error using the current yaw and the target IMU setpoint.

  

```cpp

int  angleErrorDeg(int  target, int  current) {

return  wrap180(target - current);

}

```

  

This allows the robot to keep a stable heading and execute 90-degree turns accurately.

  

---

  

### 11.3 Obstacle Avoidance Logic

  

The obstacle avoidance logic is based on modifying the robot’s target heading when a valid colored blob is detected.

  

The robot does not directly command a fixed servo angle based on the obstacle. Instead, it calculates a temporary IMU target and lets the heading controller steer toward it.

  

```cpp

int  getVisionTargetIMU() {

if (!hasFreshBlob()) {

return  setPointIMU;

}

  

float  blobAngle = getBlobAngleDeg(camBlob);

float  correction = 0.0f;

  

if (camBlob.colorId == COLOR_GREEN) {

if (blobAngle > -VISION_DEAD_ZONE_DEG) {

correction = blobAngle + PASS_OFFSET_DEG;

}

}

else  if (camBlob.colorId == COLOR_RED) {

if (blobAngle < VISION_DEAD_ZONE_DEG) {

correction = blobAngle - PASS_OFFSET_DEG;

}

}

  

correction = constrain(correction * CORR_KP, -MAX_VISION_CORRECTION_DEG, MAX_VISION_CORRECTION_DEG);

  

return  wrap180(setPointIMU + (int)correction);

}

```

  

This means that the robot keeps its normal heading when there is no obstacle, but shifts its target heading when a red or green obstacle is detected.

  

#### Green Obstacle

  

For green obstacles, the robot applies a positive correction to the target heading when the obstacle is inside the active vision zone.

  

```text

GREEN obstacle:

├─ If the obstacle is inside the active zone

│ └─ Shift target heading using a positive offset

└─ If the obstacle is outside the active zone

└─ Keep current heading

```

  

#### Red Obstacle

  

For red obstacles, the robot applies a negative correction to the target heading when the obstacle is inside the active vision zone.

  

```text

RED obstacle:

├─ If the obstacle is inside the active zone

│ └─ Shift target heading using a negative offset

└─ If the obstacle is outside the active zone

└─ Keep current heading

```

  

#### Vision Parameters

  

```text

Camera width: 320 px

Camera center X: 160 px

Camera half FOV: 30°

Vision dead zone: 20°

Pass offset: 30°

Correction gain: 4

Max vision correction: ±60°

Camera timeout: 300 ms

```

  

---

  

### 11.4 Heading and Steering Control

  

The robot uses the IMU target calculated by the obstacle logic and applies a PD controller to steer the servo.

  

```cpp

void  updateHeadingSteering(int  yaw, int  targetIMU) {

errorIMU = angleErrorDeg(targetIMU, yaw);

  

float  dtIMU = max(1.0f, (float)lastReadIMU);

float  dErrIMU = (errorIMU - lastErrorIMU) / dtIMU;

  

float  imuTerm = kpIMU * errorIMU + kdIMU * dErrIMU;

  

lastErrorIMU = errorIMU;

lastReadIMU = 0;

  

int  servoCmd = constrain((int)(servoCenter + imuTerm), servoMin, servoMax);

  

if (left < 100) {

writeServoCmd(60);

}

else  if (right < 100) {

writeServoCmd(120);

}

else {

writeServoCmd(servoCmd);

}

}

```

  

The side ToF sensors also act as emergency protection. If the robot gets too close to the left or right side, the servo is forced away from that side.

  

```text

Side safety:

├─ If left distance < 100 mm

│ └─ Force servo to 60°

├─ If right distance < 100 mm

│ └─ Force servo to 120°

└─ Otherwise

└─ Use normal IMU-based steering

```

  

---

  

### 11.5 Navigation States

  

The robot operates through several simple states instead of a complex sector-based navigation system.

  

```text

Main states:

├─ Waiting for start

├─ Normal driving

├─ Direction selection

├─ Turning

├─ Reversing after turn

├─ Recovery reverse

└─ Finished

```

  

---

  

#### 11.5.1 Waiting for Start

  

Before the start button is pressed, the robot keeps the motor stopped and the servo centered. During this time, it can still read ToF and OpenMV packets for debugging.

  

```cpp

while (butState == 0) {

motorStop();

writeServoCmd(servoCenter);

  

if (digitalRead(BUTTON) == 1) {

butState = 1;

tone(BUZZER, 700, 500);

delay(300);

  

resetWallConfirmTimers();

}

  

readTOFPacket(Serial5, front, left, right);

readOpenMVPacket(Serial3, camBlob);

}

```

  

---

  

#### 11.5.2 Normal Driving

  

In normal driving mode, the robot calculates a vision-based target heading and follows it using the IMU controller.

  

```cpp

int  visionTarget = getVisionTargetIMU();

updateHeadingSteering(imuRead, visionTarget);

motorForwardPWM(60);

```

  

During this state, the robot also checks if it should start a turn.

  

---

  

#### 11.5.3 Direction Selection

  

The robot does not assume the track direction permanently from the start. When it reaches the first confirmed wall, it stops briefly and compares the left and right distances.

  

```cpp

void  finishDirectionChoiceAndStartTurn() {

choosingDirection = false;

directionChosen = true;

  

if (left >= right) {

turnDirection = 1; // Counterclockwise, left turns

tone(BUZZER, 1500, 100);

} else {

turnDirection = -1; // Clockwise, right turns

tone(BUZZER, 700, 100);

}

  

startTurnByDirection();

turnCount++;

}

```

  

If the left side has more space, the robot chooses counterclockwise movement. Otherwise, it chooses clockwise movement.

  

---

  

#### 11.5.4 Turn Execution

  

Turns are performed by changing the IMU target by 90 degrees in the selected direction.

  

```cpp

void  startTurnByDirection() {

setPointIMU = wrap180(setPointIMU + (90 * turnDirection));

turning = true;

turnArmed = false;

turnTimer = 0;

  

resetWallConfirmTimers();

}

```

  

During the turn, the robot drives forward at a lower PWM and uses the IMU error to steer toward the new target heading.

  

```cpp

void  updateTurn(int  yaw) {

int  turnError = angleErrorDeg(setPointIMU, yaw);

  

int  servoCmd = constrain((int)(servoCenter + kpTurn * turnError), servoMin, servoMax);

writeServoCmd(servoCmd);

  

if (abs(turnError) <= turnFinishError) {

turning = false;

}

}

```

  

A turn is considered complete when the heading error is within `3°`.

  

---

  

#### 11.5.5 Reverse After Turn

  

After finishing a turn, the robot performs a controlled reverse maneuver. This helps it recover space and prevents it from staying too close to the wall after the turn.

  

```cpp

void  startReverse() {

reversing = true;

reverseTimer = 0;

turnArmed = false;

writeServoCmd(servoCenter);

  

resetWallConfirmTimers();

}

```

  

During this state, the robot reverses with the servo centered.

  

```text

Reverse after turn:

├─ Servo centered

├─ Motor reverse PWM: 100

├─ Reverse time: 3000 ms

└─ Turn is re-armed only when the front distance is clear again

```

  

---

  

### 11.6 Wall Confirmation Logic

  

The robot does not start a turn immediately when the front ToF sensor detects something close. This is important because a colored obstacle could be in front of the robot, and the robot should not confuse it with a wall.

  

The robot starts a turn only when:

  

- The front distance is below `600 mm`

- No fresh OpenMV blob is visible

- The “no pillar” timer has passed `350 ms`

- The wall confirmation timer has passed `120 ms`

- The robot is aligned with its current heading, with less than `10°` of error

- The turn is armed

  

```cpp

void  updateTurnCondition(int  imuRead) {

int  baseHeadingError = angleErrorDeg(setPointIMU, imuRead);

  

bool  pillarVisible = hasFreshBlob();

  

if (pillarVisible) {

resetWallConfirmTimers();

return;

}

  

if (front >= frontTurnThreshold) {

wallConfirmTimer = 0;

return;

}

  

bool  noPillarConfirmed = noPillarTimer >= NO_PILLAR_BEFORE_TURN_MS;

bool  wallConfirmed = wallConfirmTimer >= WALL_CONFIRM_MS;

  

if (turnArmed &&

wallConfirmed &&

noPillarConfirmed &&

abs(baseHeadingError) < 10) {

  

if (!directionChosen) {

startDirectionChoice();

} else {

startTurnByDirection();

turnCount++;

tone(BUZZER, 1500, 100);

}

}

}

```

  

This prevents false turns caused by colored obstacles.

  

---

  

### 11.7 Recovery Logic

  

The robot has a recovery state for two main cases:

  

1. A pillar or obstacle is too close in front of the robot

2. The robot appears to be stuck during a turn

  

---

  

#### 11.7.1 Pillar Too Close

  

If a fresh camera detection exists and the front ToF distance is less than `50 mm`, the robot starts a short reverse recovery.

  

```cpp

bool  pillarTooClose() {

return  hasFreshBlob() && front > 0 && front < PILLAR_DANGER_FRONT_MM;

}

```

  

This prevents the robot from pushing into an obstacle.

  

---

  

#### 11.7.2 Turn Stuck Detection

  

If the robot is turning, the heading error is still too large, and the front distance remains too close for too long, the robot assumes the turn is stuck.

  

```cpp

bool  turnLooksStuck(int  imuRead) {

if (!turning) {

return  false;

}

  

int  turnError = abs(angleErrorDeg(setPointIMU, imuRead));

  

if (turnError <= turnFinishError + 2) {

return  false;

}

  

if (front < frontTurnThreshold && turnTimer >= TURN_FAIL_TIME_MS) {

return  true;

}

  

return  false;

}

```

  

If this happens, the robot performs a recovery reverse and then retries the same turn.

  

---

  

#### 11.7.3 Recovery Reverse

  

During recovery, the robot reverses while using the inverted version of the last servo command. This helps the robot undo the movement that caused it to get stuck.

  

```cpp

int  getInvertedServoCmd() {

int  inverted = (servoCenter * 2) - lastServoCmd;

return  constrain(inverted, servoMin, servoMax);

}

```

  

The recovery reverse lasts `1500 ms`, unless the robot becomes aligned before the timer finishes.

  

```text

Recovery reverse:

├─ Reverse PWM: 100

├─ Servo command: inverted last steering command

├─ Duration: 1500 ms

├─ If recovering from a stuck turn:

│ └─ Retry the same turn

└─ If recovering from a pillar:

└─ Return to normal driving

```

  

---

  

### 11.8 Communication Protocols

  

The current version uses two UART communication protocols.

  

#### 11.8.1 ToF UART Protocol

  

```text

UART: Serial5

Baud rate: 2,000,000

  

Packet:

[START] [Front_Low] [Front_High] [Left_Low] [Left_High] [Right_Low] [Right_High]

0xAA uint8 uint8 uint8 uint8 uint8 uint8

```

  

There is no checksum in the current ToF packet. Instead, the system uses a start byte and filters invalid readings above `3000 mm`.

  

#### 11.8.2 OpenMV UART Protocol

  

```text

UART: Serial3

Baud rate: 115,200

  

Packet:

[START] [Color_ID] [X_Low] [X_High] [Y_Low] [Y_High] [END]

0xAA uint8 uint8 uint8 uint8 uint8 0x55

```

  

The end byte `0x55` is used to validate the OpenMV packet.

  

#### 11.8.3 IMU UART

  

```text

UART: Serial4

Baud rate: 115,200

```

  

The IMU provides the yaw angle used for heading correction and turn execution.

  

---

  

### 11.9 Safety Systems

  

#### 11.9.1 Distance Filtering

  

ToF values above `3000 mm` are ignored to prevent invalid distance readings from affecting the robot.

  

#### 11.9.2 Side Collision Prevention

  

If the robot gets too close to either side, the steering command is forced away from the wall.

  

```text

Left < 100 mm → Servo forced to 60°

Right < 100 mm → Servo forced to 120°

```

  

#### 11.9.3 Pillar Protection

  

The robot avoids starting a wall turn if a fresh camera blob is visible. This helps prevent the robot from confusing a colored obstacle with a wall.

  

#### 11.9.4 Turn Failure Recovery

  

If a turn takes too long while the front distance remains too close, the robot reverses and retries the turn.

  

#### 11.9.5 Turn Arming

  

The robot only allows a new turn after the front distance becomes clear again.

  

```cpp

if (!turning && front > frontTurnThreshold + 100) {

turnArmed = true;

}

```

  

This prevents the robot from triggering multiple turns from the same wall detection.

  

---

  

### 11.10 Key Advantages of This Implementation

  

#### Simpler System Architecture

  

The robot no longer depends on a Raspberry Pi, LIDAR, or OTOS. This reduces complexity and makes the system easier to debug.

  

#### Direct Embedded Control

  

All control decisions are handled directly on the Teensy 4.0, making the robot’s behavior more deterministic and responsive.

  

#### Vision-Based Obstacle Avoidance

  

The OpenMV provides simple and fast color-based obstacle detection. The Teensy then converts the obstacle position into a temporary heading correction.

  

#### Reliable Turn Confirmation

  

The robot confirms that a wall is actually present before turning. It also ignores wall-turn triggers when a camera blob is visible, reducing false positives caused by obstacles.

  

#### Recovery Behavior

  

The robot can recover when it gets too close to a pillar or when a turn gets stuck, improving reliability during the obstacle round.

  

#### Modular Communication

  

The system separates sensor responsibilities clearly:

  

```text

OpenMV → Color obstacle detection

XIAO ESP32-C6 → ToF sensor reading

IMU → Heading feedback

Teensy 4.0 → Main control logic

```## 11. Obstacle Management <a name="obstacle-management"></a>

  

The robot’s obstacle management system is handled directly by the Teensy 4.0. The system combines color detection from the OpenMV camera, distance measurements from the VL53L8CX ToF sensors, and heading feedback from the IMU.

  

Unlike last year’s system, this version does not use LIDAR, OTOS, Raspberry Pi, or PiCamera2. The goal of this redesign was to make the obstacle logic simpler, faster, and easier to debug.

  

The current system uses:

  

-  **OpenMV camera** for red and green obstacle detection

-  **VL53L8CX ToF sensors** for front, left, and right distance measurements

-  **IMU** for heading control and turn execution

-  **Teensy 4.0** as the main controller for decision-making, steering, and motor control

  

---

  

### 11.1 Detection Methods

  

The robot uses two main sensing systems during the obstacle round.

  

#### 11.1.1 Vision Detection

  

The OpenMV camera detects colored obstacles and sends the following data to the Teensy:

  

-  **Color ID**

-  `1` = Red obstacle

-  `2` = Green obstacle

-  **X position** of the detected blob

-  **Y position** of the detected blob

  

The Teensy does not receive the obstacle distance directly from the camera. Instead, it uses the horizontal position of the blob to calculate an approximate relative angle.

  

#### 11.1.2 Distance Detection

  

The VL53L8CX ToF sensors provide the robot with three main distance readings:

  

-  **Front distance**

-  **Left distance**

-  **Right distance**

  

These distances are used to detect walls, confirm turns, prevent collisions, and avoid getting too close to the sides of the track.

  

#### 11.1.3 Heading Detection

  

The IMU provides the current yaw angle of the robot. This is used to:

  

- Keep the robot aligned with the current target heading

- Execute 90-degree turns

- Detect when a turn has finished

- Recover if the robot gets stuck during a turn

  

---

  

### 11.2 Multi-Sensor Data Processing

  

#### 11.2.1 OpenMV Integration

  

The OpenMV sends a binary packet through UART to the Teensy.

  

**OpenMV Packet Format:**

  

```text

[START] [Color_ID] [X_Low] [X_High] [Y_Low] [Y_High] [END]

0xAA uint8 uint8 uint8 uint8 uint8 0x55

```

  

The Teensy reads this packet and stores the detected color and blob position.

  

```cpp

struct  OpenMVBlob {

uint16_t  x = 0;

uint16_t  y = 0;

uint8_t  colorId = 0;

bool  valid = false;

};

```

  

The X position is converted into an approximate angle using the camera center and the configured field of view.

  

```cpp

float  getBlobAngleDeg(const  OpenMVBlob  &blob) {

int  dx = (int)blob.x - CAM_CENTER_X;

  

float  angle = ((float)dx * CAM_HALF_FOV_DEG) / (float)CAM_CENTER_X;

  

if (CAM_INVERT_X) {

angle = -angle;

}

  

return  constrain(angle, -CAM_HALF_FOV_DEG, CAM_HALF_FOV_DEG);

}

```

  

A camera detection is considered valid only for a short time. If no new packet is received after `300 ms`, the detection is ignored.

  

```cpp

bool  hasFreshBlob() {

return  camBlob.valid && lastCamPacket <= CAM_TIMEOUT_MS;

}

```

  

This prevents the robot from reacting to old obstacle detections.

  

---

  

#### 11.2.2 ToF Integration

  

The ToF data is received by the Teensy through UART from the XIAO ESP32-C6.

  

**ToF Packet Format:**

  

```text

[START] [Front_Low] [Front_High] [Left_Low] [Left_High] [Right_Low] [Right_High]

0xAA uint8 uint8 uint8 uint8 uint8 uint8

```

  

The packet contains three 16-bit distance values in millimeters.

  

- Front distance

- Left distance

- Right distance

  

Any reading above `3000 mm` is considered invalid and discarded. When this happens, the previous valid value is preserved.

  

```cpp

if (newFront <= TOF_MAX_VALID_MM) front = newFront;

if (newLeft <= TOF_MAX_VALID_MM) left = newLeft;

if (newRight <= TOF_MAX_VALID_MM) right = newRight;

```

  

This prevents corrupted or unrealistic ToF readings from affecting the robot’s movement.

  

---

  

#### 11.2.3 IMU Integration

  

The IMU is connected to the Teensy through UART. Its yaw value is converted to the `-180°` to `180°` range.

  

```cpp

int  wrap180(int  ang) {

while (ang > 180) ang -= 360;

while (ang <= -180) ang += 360;

return  ang;

}

```

  

The robot calculates heading error using the current yaw and the target IMU setpoint.

  

```cpp

int  angleErrorDeg(int  target, int  current) {

return  wrap180(target - current);

}

```

  

This allows the robot to keep a stable heading and execute 90-degree turns accurately.

  

---

  

### 11.3 Obstacle Avoidance Logic

  

The obstacle avoidance logic is based on modifying the robot’s target heading when a valid colored blob is detected.

  

The robot does not directly command a fixed servo angle based on the obstacle. Instead, it calculates a temporary IMU target and lets the heading controller steer toward it.

  

```cpp

int  getVisionTargetIMU() {

if (!hasFreshBlob()) {

return  setPointIMU;

}

  

float  blobAngle = getBlobAngleDeg(camBlob);

float  correction = 0.0f;

  

if (camBlob.colorId == COLOR_GREEN) {

if (blobAngle > -VISION_DEAD_ZONE_DEG) {

correction = blobAngle + PASS_OFFSET_DEG;

}

}

else  if (camBlob.colorId == COLOR_RED) {

if (blobAngle < VISION_DEAD_ZONE_DEG) {

correction = blobAngle - PASS_OFFSET_DEG;

}

}

  

correction = constrain(correction * CORR_KP, -MAX_VISION_CORRECTION_DEG, MAX_VISION_CORRECTION_DEG);

  

return  wrap180(setPointIMU + (int)correction);

}

```

  

This means that the robot keeps its normal heading when there is no obstacle, but shifts its target heading when a red or green obstacle is detected.

  

#### Green Obstacle

  

For green obstacles, the robot applies a positive correction to the target heading when the obstacle is inside the active vision zone.

  

```text

GREEN obstacle:

├─ If the obstacle is inside the active zone

│ └─ Shift target heading using a positive offset

└─ If the obstacle is outside the active zone

└─ Keep current heading

```

  

#### Red Obstacle

  

For red obstacles, the robot applies a negative correction to the target heading when the obstacle is inside the active vision zone.

  

```text

RED obstacle:

├─ If the obstacle is inside the active zone

│ └─ Shift target heading using a negative offset

└─ If the obstacle is outside the active zone

└─ Keep current heading

```

  

#### Vision Parameters

  

```text

Camera width: 320 px

Camera center X: 160 px

Camera half FOV: 30°

Vision dead zone: 20°

Pass offset: 30°

Correction gain: 4

Max vision correction: ±60°

Camera timeout: 300 ms

```

  

---

  

### 11.4 Heading and Steering Control

  

The robot uses the IMU target calculated by the obstacle logic and applies a PD controller to steer the servo.

  

```cpp

void  updateHeadingSteering(int  yaw, int  targetIMU) {

errorIMU = angleErrorDeg(targetIMU, yaw);

  

float  dtIMU = max(1.0f, (float)lastReadIMU);

float  dErrIMU = (errorIMU - lastErrorIMU) / dtIMU;

  

float  imuTerm = kpIMU * errorIMU + kdIMU * dErrIMU;

  

lastErrorIMU = errorIMU;

lastReadIMU = 0;

  

int  servoCmd = constrain((int)(servoCenter + imuTerm), servoMin, servoMax);

  

if (left < 100) {

writeServoCmd(60);

}

else  if (right < 100) {

writeServoCmd(120);

}

else {

writeServoCmd(servoCmd);

}

}

```

  

The side ToF sensors also act as emergency protection. If the robot gets too close to the left or right side, the servo is forced away from that side.

  

```text

Side safety:

├─ If left distance < 100 mm

│ └─ Force servo to 60°

├─ If right distance < 100 mm

│ └─ Force servo to 120°

└─ Otherwise

└─ Use normal IMU-based steering

```

  

---

  

### 11.5 Navigation States

  

The robot operates through several simple states instead of a complex sector-based navigation system.

  

```text

Main states:

├─ Waiting for start

├─ Normal driving

├─ Direction selection

├─ Turning

├─ Reversing after turn

├─ Recovery reverse

└─ Finished

```

  

---

  

#### 11.5.1 Waiting for Start

  

Before the start button is pressed, the robot keeps the motor stopped and the servo centered. During this time, it can still read ToF and OpenMV packets for debugging.

  

```cpp

while (butState == 0) {

motorStop();

writeServoCmd(servoCenter);

  

if (digitalRead(BUTTON) == 1) {

butState = 1;

tone(BUZZER, 700, 500);

delay(300);

  

resetWallConfirmTimers();

}

  

readTOFPacket(Serial5, front, left, right);

readOpenMVPacket(Serial3, camBlob);

}

```

  

---

  

#### 11.5.2 Normal Driving

  

In normal driving mode, the robot calculates a vision-based target heading and follows it using the IMU controller.

  

```cpp

int  visionTarget = getVisionTargetIMU();

updateHeadingSteering(imuRead, visionTarget);

motorForwardPWM(60);

```

  

During this state, the robot also checks if it should start a turn.

  

---

  

#### 11.5.3 Direction Selection

  

The robot does not assume the track direction permanently from the start. When it reaches the first confirmed wall, it stops briefly and compares the left and right distances.

  

```cpp

void  finishDirectionChoiceAndStartTurn() {

choosingDirection = false;

directionChosen = true;

  

if (left >= right) {

turnDirection = 1; // Counterclockwise, left turns

tone(BUZZER, 1500, 100);

} else {

turnDirection = -1; // Clockwise, right turns

tone(BUZZER, 700, 100);

}

  

startTurnByDirection();

turnCount++;

}

```

  

If the left side has more space, the robot chooses counterclockwise movement. Otherwise, it chooses clockwise movement.

  

---

  

#### 11.5.4 Turn Execution

  

Turns are performed by changing the IMU target by 90 degrees in the selected direction.

  

```cpp

void  startTurnByDirection() {

setPointIMU = wrap180(setPointIMU + (90 * turnDirection));

turning = true;

turnArmed = false;

turnTimer = 0;

  

resetWallConfirmTimers();

}

```

  

During the turn, the robot drives forward at a lower PWM and uses the IMU error to steer toward the new target heading.

  

```cpp

void  updateTurn(int  yaw) {

int  turnError = angleErrorDeg(setPointIMU, yaw);

  

int  servoCmd = constrain((int)(servoCenter + kpTurn * turnError), servoMin, servoMax);

writeServoCmd(servoCmd);

  

if (abs(turnError) <= turnFinishError) {

turning = false;

}

}

```

  

A turn is considered complete when the heading error is within `3°`.

  

---

  

#### 11.5.5 Reverse After Turn

  

After finishing a turn, the robot performs a controlled reverse maneuver. This helps it recover space and prevents it from staying too close to the wall after the turn.

  

```cpp

void  startReverse() {

reversing = true;

reverseTimer = 0;

turnArmed = false;

writeServoCmd(servoCenter);

  

resetWallConfirmTimers();

}

```

  

During this state, the robot reverses with the servo centered.

  

```text

Reverse after turn:

├─ Servo centered

├─ Motor reverse PWM: 100

├─ Reverse time: 3000 ms

└─ Turn is re-armed only when the front distance is clear again

```

  

---

  

### 11.6 Wall Confirmation Logic

  

The robot does not start a turn immediately when the front ToF sensor detects something close. This is important because a colored obstacle could be in front of the robot, and the robot should not confuse it with a wall.

  

The robot starts a turn only when:

  

- The front distance is below `600 mm`

- No fresh OpenMV blob is visible

- The “no pillar” timer has passed `350 ms`

- The wall confirmation timer has passed `120 ms`

- The robot is aligned with its current heading, with less than `10°` of error

- The turn is armed

  

```cpp

void  updateTurnCondition(int  imuRead) {

int  baseHeadingError = angleErrorDeg(setPointIMU, imuRead);

  

bool  pillarVisible = hasFreshBlob();

  

if (pillarVisible) {

resetWallConfirmTimers();

return;

}

  

if (front >= frontTurnThreshold) {

wallConfirmTimer = 0;

return;

}

  

bool  noPillarConfirmed = noPillarTimer >= NO_PILLAR_BEFORE_TURN_MS;

bool  wallConfirmed = wallConfirmTimer >= WALL_CONFIRM_MS;

  

if (turnArmed &&

wallConfirmed &&

noPillarConfirmed &&

abs(baseHeadingError) < 10) {

  

if (!directionChosen) {

startDirectionChoice();

} else {

startTurnByDirection();

turnCount++;

tone(BUZZER, 1500, 100);

}

}

}

```

  

This prevents false turns caused by colored obstacles.

  

---

  

### 11.7 Recovery Logic

  

The robot has a recovery state for two main cases:

  

1. A pillar or obstacle is too close in front of the robot

2. The robot appears to be stuck during a turn

  

---

  

#### 11.7.1 Pillar Too Close

  

If a fresh camera detection exists and the front ToF distance is less than `50 mm`, the robot starts a short reverse recovery.

  

```cpp

bool  pillarTooClose() {

return  hasFreshBlob() && front > 0 && front < PILLAR_DANGER_FRONT_MM;

}

```

  

This prevents the robot from pushing into an obstacle.

  

---

  

#### 11.7.2 Turn Stuck Detection

  

If the robot is turning, the heading error is still too large, and the front distance remains too close for too long, the robot assumes the turn is stuck.

  

```cpp

bool  turnLooksStuck(int  imuRead) {

if (!turning) {

return  false;

}

  

int  turnError = abs(angleErrorDeg(setPointIMU, imuRead));

  

if (turnError <= turnFinishError + 2) {

return  false;

}

  

if (front < frontTurnThreshold && turnTimer >= TURN_FAIL_TIME_MS) {

return  true;

}

  

return  false;

}

```

  

If this happens, the robot performs a recovery reverse and then retries the same turn.

  

---

  

#### 11.7.3 Recovery Reverse

  

During recovery, the robot reverses while using the inverted version of the last servo command. This helps the robot undo the movement that caused it to get stuck.

  

```cpp

int  getInvertedServoCmd() {

int  inverted = (servoCenter * 2) - lastServoCmd;

return  constrain(inverted, servoMin, servoMax);

}

```

  

The recovery reverse lasts `1500 ms`, unless the robot becomes aligned before the timer finishes.

  

```text

Recovery reverse:

├─ Reverse PWM: 100

├─ Servo command: inverted last steering command

├─ Duration: 1500 ms

├─ If recovering from a stuck turn:

│ └─ Retry the same turn

└─ If recovering from a pillar:

└─ Return to normal driving

```

  

---

  

### 11.8 Communication Protocols

  

The current version uses two UART communication protocols.

  

#### 11.8.1 ToF UART Protocol

  

```text

UART: Serial5

Baud rate: 2,000,000

  

Packet:

[START] [Front_Low] [Front_High] [Left_Low] [Left_High] [Right_Low] [Right_High]

0xAA uint8 uint8 uint8 uint8 uint8 uint8

```

  

There is no checksum in the current ToF packet. Instead, the system uses a start byte and filters invalid readings above `3000 mm`.

  

#### 11.8.2 OpenMV UART Protocol

  

```text

UART: Serial3

Baud rate: 115,200

  

Packet:

[START] [Color_ID] [X_Low] [X_High] [Y_Low] [Y_High] [END]

0xAA uint8 uint8 uint8 uint8 uint8 0x55

```

  

The end byte `0x55` is used to validate the OpenMV packet.

  

#### 11.8.3 IMU UART

  

```text

UART: Serial4

Baud rate: 115,200

```

  

The IMU provides the yaw angle used for heading correction and turn execution.

  

---

  

### 11.9 Safety Systems

  

#### 11.9.1 Distance Filtering

  

ToF values above `3000 mm` are ignored to prevent invalid distance readings from affecting the robot.

  

#### 11.9.2 Side Collision Prevention

  

If the robot gets too close to either side, the steering command is forced away from the wall.

  

```text

Left < 100 mm → Servo forced to 60°

Right < 100 mm → Servo forced to 120°

```

  

#### 11.9.3 Pillar Protection

  

The robot avoids starting a wall turn if a fresh camera blob is visible. This helps prevent the robot from confusing a colored obstacle with a wall.

  

#### 11.9.4 Turn Failure Recovery

  

If a turn takes too long while the front distance remains too close, the robot reverses and retries the turn.

  

#### 11.9.5 Turn Arming

  

The robot only allows a new turn after the front distance becomes clear again.

  

```cpp

if (!turning && front > frontTurnThreshold + 100) {

turnArmed = true;

}

```

  

This prevents the robot from triggering multiple turns from the same wall detection.

  

---

  

### 11.10 Key Advantages of This Implementation

  

#### Simpler System Architecture

  

The robot no longer depends on a Raspberry Pi, LIDAR, or OTOS. This reduces complexity and makes the system easier to debug.

  

#### Direct Embedded Control

  

All control decisions are handled directly on the Teensy 4.0, making the robot’s behavior more deterministic and responsive.

  

#### Vision-Based Obstacle Avoidance

  

The OpenMV provides simple and fast color-based obstacle detection. The Teensy then converts the obstacle position into a temporary heading correction.

  

#### Reliable Turn Confirmation

  

The robot confirms that a wall is actually present before turning. It also ignores wall-turn triggers when a camera blob is visible, reducing false positives caused by obstacles.

  

#### Recovery Behavior

  

The robot can recover when it gets too close to a pillar or when a turn gets stuck, improving reliability during the obstacle round.

  

#### Modular Communication

  

The system separates sensor responsibilities clearly:

  

```text

OpenMV → Color obstacle detection

XIAO ESP32-C6 → ToF sensor reading

IMU → Heading feedback

Teensy 4.0 → Main control logic

```

## 12. Construction Guide <a name="construction-guide"></a>

  

**Models file folder:**  `models/`

  

### 12.1 Steps

- Step 1: 3D designing

- Step 2: 3D printing

- Step 3: Electronic layout

- Step 4: Wiring

- Step 5: Mounting

- Step 6: Programming

- Step 7: Testing

  

### 12.2 Construction Tools

- 3D Printer (Creality K2 Plus, QIDI Q1 Pro)

- Polymaker PTG CF filament

- Mini Electric Soldering Iron Kit TS101

- Dremel Tool

- Screwdriver Set Fanttik

  
  

## 13. Cost Report <a name="cost-report"></a>

  Total cost of developmnet, including last year robot, and this new year robot, we decided to show full cost of both together also because this new robot is an improvement and also an other iterarion of the last year robot in every aspect, so its part of the same development path.

| Item | Qty | Unit Cost (MXN) | Total (MXN) |
|------------------------------|-----|------------------|-------------|
| Teensy 4.0 | 1 | $800 | 800 |
| RPlidar C1 | 1 | $2,500 | $2,500 |
| Raspberry Pi Camera 12mp V3 | 1 | $920 | $920 |
| Raspberry Pi 5 Camera Cable | 1 | $64 | $64 |
| 2.2Ah LiPo 11.1V Battery | 1 | $600 | $600 |
| 1Ah LiPo 3.3V Battery | 1 | $70 | $70 |
| Maxon Motor DCX19 | 1 | $8,500 | $8,500 |
| HS85MG Micro Servo | 1 | $2,000 | $2,000 |
| SparkFun OTOS | 1 | $2,400 | $2,400 |
| POLYMAKER PLA Filament (prototypes) | 1kg | $900 | $900 |
| POLYMAKER PLA-CF Filament (finals) | 0.5kg | $450 | $450 |
| Carbon Fiber | 1 | $2,000 | $2,000 |
| SMD Components & Misc. | - | $1,500 | $1,500 |
| PCB Manufacturing | 1 | $800 | $800 |
| Spike Wheels (LEGO) | 4 | $150 | $600 |
| EV3 Wheels (LEGO) | 2 | $10 | $20 |
| **Total** | | | **$26,924**|
---
TODO: this year robot price
| Item | Qty | Unit Cost (MXN) | Total (MXN) |
|------------------------------|-----|------------------|-------------|
| Teensy 4.0 | 1 | $800 | 800 |
| HS85MG Micro Servo | 1 | $2,000 | $2,000 |
|Pololu 25D 500 RPM| 1| 1100|1100
||
| POLYMAKER PLA Filament (prototypes) | 1kg | $900 | $900 |
| POLYMAKER PLA-CF Filament (finals) | 0.5kg | $450 | $450 |
| SMD Components & Misc. | - | $1,500 | $1,500 |
| PCB Manufacturing | 1 | $800 | $800 |
| **Total** | | | ** TODO **|
  

## 14. Resources <a name="resources"></a>

  

- [Chabots Main Site](https://www.chabots.mx)

- [WRO Future Engineers Rules PDF](https://wro-association.org/wp-content/uploads/WRO-2024-Future-Engineers-Self-Driving-Cars-General-Rules.pdf)

- [GitHub Repos](https://github.com/chaBotsMX/chaBots-Tuneados-WRO-Future-Engineers-2026)

  

---

  

## 15. License <a name="license"></a>

  

```

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy of this software.

```

  

---

  

>  *Document maintained by Chabots | Last updated: Apr 2026*

  

<!--stackedit_data:

eyJoaXN0b3J5IjpbMTcyMzM3ODYxNCwtMzc2NTM2MDM5LDM1ND

c4NDQyMCwxMjQ4Mzg0MTM1LC0yODM3NTcxNywtMTMyNzEwNTIy

MywxMjg3Nzk2NjQsLTQ4MTYzMzM4MF19

-->

  
  

> Written with [StackEdit](https://stackedit.io/).