# Optical Tachometer

This project has been created for a course DTEK0068 - "Embedded MicroProcessor Systems" held at the University of Turku. The project was started at the end of November 2020, and it was completed in mid-December 2020. The team which accomplished the project consisted of three participants. 

**This project was graded as the top of the class.** Assignment and grading was given by university teacher Jani Tammi (https://github.com/Jasata), who also consulted in making my initial portfolio.

## Technical details

The microcontroller that was used as project platform is ATmega4809 Curiosity Nano. Other components used are the breadboard, 7-segment LED display, DC motor and propellor, 10k potentiometer, L9110 motor driver, 16x2 LCD display (HD44780 compatible), LDR, appropriate resistors, LEDs and dupont wires. Component kit was provided by university for the duration of the work.

Optical Tachometer uses visible light to detect and calculate RPM of spinning propellor. An optical fork is created from discrete LDR (Light Dependent Resistor, in a voltage divider circuit) and LED. DC motor starts to rotate the propellor with a speed based on the value of potentiometer. The user shall define the spinning speed with potentiometer. LDR measures the light that will be different when the propeller is in front of the LED or whether the LDR receives only light from LED. Depending on the challenge level chosen, the implementation displayed either the most significant RPM digit in a single -segment display element or a parallel 16x2 LCD was driven to produce more verbose readout. This project implemented both options. 

Basic version introduces tachometer usage with 7-segment display which updates twice a second. DC motor control with a potentiometer is attached in version bonus1. Bonus2 version displays RPM with LCD display which updates once a second. 

In version FINAL, all versions are combined as one. The code in the FINAL repository has been parsed to project's closure form.

This project followed [BARR-C:2018 embedded C programming standard](https://barrgroup.com/embedded-systems/books/embedded-c-coding-standard), or more specifically the specific subset of the rules defined by the project requirements in the course.

## Commit history

Commit history for project can be found at: https://github.com/AmyNyma/Optical_Tachometer . This will be available until team member AmyNyma decides to delete the project. 

## Teacher's note

*This text has been requested to be added into portfolio project README by the undersigned, teacher of DTEK0068. Project work for DTEK0068 (2020) requires a demonstration of acquired knowledge in a form of a functional prototype device. My course covers wide range of topics, but the most important items required from the project submission are:*

 

  - *Understanding and correct application of shared resources (volatility, atomic access)*
  - *Correct use of interrupts to keep applications responsive*
  - *Source code discipline, by following the given coding standard*

 

*One of the driving goals in my course have been to introduce and enforce practices and concepts of quality that I myself have learned in my own professional life, before starting my career as a teacher.*

*Regards,
Jani Tammi
Teacher of DTEK0068*
