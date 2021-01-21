# Optical Tachometer

This project has been executed as a part of course DTEK0068 - Embedded MicroProcessor Systems performance in the University of Turku. The project was started at the end of November 2020, and it was completed in mid-December 2020. The team which accomplished the project consisted of three participants. 

The score received from the project was the top of the class. Assignment and evaluation for this project was given by university teacher Jani Tammi (https://github.com/Jasata) who also validates this portfolio.

## Technical details

The microcontroller that was used as project platform is ATMEGA4809 Curiosity Nano. Other components used are the breadboard, 7-segment LED display, DC motor and propellor, potentiometer, L9110 motor driver, 16x2 LCD display, LDR, appropriate resistors, LEDs and dupont wires. Component kit was afforded by university.

Optical Tachometer uses visible light to detect and calculate RPM of spinning propellor. First, the propellor shall be placed between LED and LDR. DC motor starts to rotate the propellor with a speed based on the value of potentiometer. The user shall define the spinning speed with potentiometer. LDR measures the light that will be different when the propeller is in front of the LED or whether the LDR receives only light from LED. Depending, whether 7-segment display or LCD display is used, the RPM will be calculated and informed. With 7-segment display, only the most significant digit will be updated twice a second. When utilizing LCD display, the RPM will be displayed once a second. 

Basic version introduces tachometer usage with 7-segment display. DC motor control with a potentiometer is attached in version bonus1. Bonus2 version displays RPM with LCD display. 

In version FINAL, all versions are combined as one. The code in the FINAL repository has been parsed to project's closure form.

## Commit history

Commit history for project can be found at: https://github.com/AmyNyma/Optical_Tachometer . This will be available until team member Amy Nymalm decides to delete the project. 
