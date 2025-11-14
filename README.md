# Braccio robotarm (Tinkerkit) controlled by Raspberry Pi Flask

This project implements a control system for a Braccio robot arm using the BraccioV2 library and integrates a 20x4 I2C LCD for real-time serial input logging. The core innovation of the code is a custom, non-blocking, time-based movement routine designed to ensure smooth, slow motion for the robot arm, overcoming the "jerky" movements caused by the library's direct control methods.

## 1. Braccio Control and Smooth Movement Logic
The main function of the code is to receive movement commands from the Arduino's Serial Monitor (intended to be used by an external program like Flask) and execute them gently.

Command Input: The loop() function continuously checks for serial data. When a full line is received, it checks for the format: MOVE b s e wv wr g.

Target Setting: The values are stored in target variables (target_b, etc.) via the setTargetPosition() function, and the isMoving flag is set to true.

Incremental Movement (updateSmoothMovement()): This is the custom solution for soft motion, based on a non-blocking timer (millis()).

Speed Control: The constant MOVE_DELAY = 50ms sets the movement speed.

1-Degree Steps: In each update cycle, the current_ position of every joint is moved only 1 degree closer to its respective target_ position.

Execution: The updated current_ positions are sent to the arm using the low-level command: arm.setAllNow(...).

Non-Blocking Design: This time-based approach ensures the robot moves slowly while preventing the main loop() from being blocked, allowing the LCD and Serial input to remain responsive.

## 2. LCD Display and Scrolling Log
The code incorporates a robust system for logging all incoming serial commands onto the 20x4 LCD.

Display Header: The top row (Row 0) is reserved for the static header: "Serial monitor:".

Circular Buffer: A string array, lines[MAX_LINES], acts as a circular buffer to store the last four incoming serial lines.

Scrolling Logic (updateLCD()): This function calculates the oldest line using the modulo operator and prints the four log lines consecutively to the visible rows (Row 1 to Row 4).

Formatting: Log entries are padded with spaces to ensure clarity and avoid "ghosting" from previous entries.

## 3. Setup and Initialization
BraccioV2: Initializes the arm object (arm.begin(false)), sets the initial joint centers, and moves the arm to its starting position (arm.setAllNow).

LCD: Initializes the I2C display (lcd.init(), lcd.backlight()) and draws the initial screen layout.

Movement Timer: Initializes the movement timer (lastMoveTime = millis()) to prepare the custom smooth movement routine.
