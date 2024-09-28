# RoverControlsSystem

## Control System Task - I
This project implements a control system that uses a video game controller to operate a rover and an arm mechanism. The system reads inputs from a PS5 controller using the pygame library and sends formatted control packets via the socket library as required by the task specifications. The control packets are formatted to handle two modes: driving the rover (Drive Packet) and controlling the arm (Arm Packet).

### Button Input List and Motor Control Mapping
Below is a detailed mapping of which button or joystick is used to control each specific motor in the rover and arm systems:

Circle Button (Button 1):
- Function: Switches between Drive (D) and Arm (A) control packets.
- Used For: Switching control modes (not directly linked to a specific motor).

Left Analog Stick:
X-axis:
- Used For: Gantry Motor in Arm mode (A).
- Left Movement: Gantry moves up (PWM 128-255).
- Right Movement: Gantry moves down (PWM 128-0).
  
Y-axis:
- Used For: Right Wrist Motor and Left Wrist Motor in Arm mode (A).
- Up Movement: Right wrist motor increases (PWM 128-255), left wrist moves opposite.
- Down Movement: Right wrist motor decreases (PWM 128-0), left wrist moves similarly.

Right Analog Stick:
X-axis:
- Used For: Shoulder Motor in Arm mode (A).
- Left Movement: Shoulder moves up (PWM 128-255).
- Right Movement: Shoulder moves down (PWM 128-0).
Y-axis:
- Used For: Elbow Motor in Arm mode (A).
- Up Movement: Elbow moves up (PWM 128-255).
- Down Movement: Elbow moves down (PWM 128-0).

Triangle Button (Button 3):
- Used For: Claw Motor.
- Action: Opens the claw (PWM 128-255) when held.

X Button (Button 0):
- Used For: Claw Motor.
- Action: Closes the claw (PWM 128-0) when held.

Right Analog Stick in Drive Mode (D):
- Used For: Left Wheels (LeftWheel1, LeftWheel2, LeftWheel3).
- Up Movement: Moves wheels forward (PWM 128-255).
- Down Movement: Moves wheels backward (PWM 128-0).

Left Analog Stick in Drive Mode (D):
- Used For: Right Wheels (RightWheel1, RightWheel2, RightWheel3).
- Up Movement: Moves wheels forward (PWM 128-255).
- Down Movement: Moves wheels backward (PWM 128-0).

### Usage

Running the Program:
- Ensure you have pygame and socket libraries installed.
- Run the script using Python. Connect your PS5 controller and ensure it’s detected by the system.

Controlling the Rover and Arm:
- Use the Circle button to toggle between driving the rover and controlling the arm.
- Use the analog sticks and buttons as listed above to move the different components.

## Control System Task - II
This project is a Morse Code translator that uses an Arduino Uno to convert text input from the Serial Monitor into Morse Code. The Morse code is displayed using an LED and a buzzer, where the LED blinks and the buzzer sounds to represent dots and dashes.

Note: The project also contains code for Ethernet input, but it could not be tested as I did not have access to an Ethernet shield. However, the Ethernet input functionality is implemented to the best of my knowledge based on standard practices.

### Project Overview
This project reads text input from the user via the Serial Monitor and translates it into Morse code. The Morse code is then output through an LED and a buzzer:

- LED: Blinks to represent Morse code (dots and dashes).
- Buzzer: Plays sounds for dots and dashes and emits a higher-pitched tone to indicate word breaks.

### Software Requirements
- Arduino IDE or PlatformIO (VSCode) installed
- Arduino core libraries
- Ethernet library (if using Ethernet functionality)

### Setup and Installation
Clone or Download the Project:

Open the Project:
- If using Arduino IDE, open the .ino file.
- If using PlatformIO, open the project folder.

Install Required Libraries:
- In the Arduino IDE, go to Sketch > Include Library > Manage Libraries, and install the Ethernet library if using Ethernet functionality.
- In PlatformIO, add Ethernet to your platformio.ini under lib_deps.

Upload the Code:
- Connect your Arduino Uno to your computer.
- Select the correct board and port.
- Upload the code to the Arduino.

### How It Works
- The program reads input from the Serial Monitor (or Ethernet, if connected and configured).
- The input string is translated into Morse code using a predefined dictionary.
- The Morse code is output via an LED and a buzzer, where:
- Dots (.) blink the LED and sound the buzzer briefly.
- Dashes (-) blink the LED and sound the buzzer for a longer duration.
- Spaces between words are indicated by a distinct high-pitched tone and longer pauses.
  
### Using the Serial Monitor
- Open the Serial Monitor in the Arduino IDE or PlatformIO (Ctrl + Shift + M).
- Set the Baud Rate to 9600 to match the code configuration.
- Type Your Text: Enter the text you want to translate into Morse code and press Enter.
- Watch the Output: The LED will blink, and the buzzer will sound to represent the Morse code translation.

### Conclusion
This project showcases the successful implementation of a control system for a rover and arm mechanism using a PS5 controller, alongside an Arduino-based Morse code translator. The control system meets all task requirements, providing precise and intuitive control of the rover and arm components via structured packet transmission with Python and the socket library. Simultaneously, the Morse code translator demonstrates the practical application of Arduino in encoding text into Morse code with visual and audio feedback, effectively blending hardware and software.

Although the Ethernet input feature remains untested due to hardware limitations, both systems lay a robust foundation for future expansions, such as integrating displays, wireless communication, and real-time input adjustments. This dual project not only highlights skills in control systems, Arduino programming, and Morse code encoding but also serves as an excellent learning tool for anyone interested in electronics, coding, and communication protocols. Through the completion of these projects, valuable experience was gained in combining software and hardware solutions to create functional, interactive systems.
