# Autonomous-Robotic-Vaccum-Cleaner-
Autonomous robotic vacuum cleaner built using ESP32 and C programming for automatic floor cleaning without manual effort, featuring obstacle detection using sensors.
Project Overview:
This project presents an Autonomous Robotic Vacuum Cleaner designed to clean floors automatically without human intervention. The robot moves in a random path pattern while continuously detecting obstacles using sensors. When an obstacle is detected, the robot changes its direction and continues cleaning, ensuring effective floor coverage.

The system is built using the ESP32 microcontroller programmed in C, which controls sensors and motor movement. A mini DC fan is used to create suction that collects dust and debris while the robot moves across the floor.
This project demonstrates the use of embedded systems, robotics, and automation for smart cleaning solutions.

Features:
Autonomous floor cleaning,
Random path navigation,
Real-time obstacle detection,
Automatic direction change when obstacles are detected,
Compact robotic cleaning system,

Software used:
Arduino IDE,
Dabble app to just on and off

Hardware Components Used:
ESP32 Microcontroller,
L298N Motor Driver Module,
3 Ultrasonic Sensors,
3 IR Sensors,
Mini DC Fan (for suction),
4 Wheels,
12V Power Supply,
9V Battery,
Buck converter,
DC gear motors.

Working Principle:
The ESP32 microcontroller acts as the main controller of the robot. The robot moves in a random navigation path to cover the floor area.

The ultrasonic sensors and IR sensors continuously monitor the surroundings. When an obstacle is detected, the ESP32 processes the signal and changes the direction of the robot to avoid collision.
The wheels are controlled through the L298N motor driver, which drives the motors responsible for the robot's movement.
A mini DC fan creates suction to collect dust and small particles while the robot moves across the floor.
During testing and operation, the ESP32 is powered through USB connected to a laptop, while separate power supplies (12V and 9V) are used for other hardware components.

Applications:
Smart home cleaning systems,
Autonomous service robots,
Robotics and embedded systems projects,
Educational demonstrations of automation concepts,
Future Improvements,
Smart mapping instead of random path navigation,
Mobile app control using WiFi,
Automatic charging dock,
Improved suction and dust collection system.
