/**
 * File: update_spin.c
 * 
 * DESCRIPTION
 *      This is a single source file for updating the spinning speed of DC 
 *      motor. It receives a parameter userVoltage which is the value received 
 *      from potentiometer. The TCB will be disabled for the process of changing
 *      the motor speed. Motor speed is updated by changing the duty cycle of
 *      PWM that is controlled by TCB.
 */

#include <avr/io.h>

void update_spin(uint8_t userVoltage)
{
    // Variable to store hexadecimal value of given userVoltage
    //uint8_t userHex;
    
    // Disable the peripheral
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
    
    // Writing Capture/Compare register with new duty cycle based on
    // potentiometer
    TCB0.CCMPH = userVoltage;
    
    // Reset count register
    TCB0.CNT = 0x0000;
    
    // Re-enable the module
    TCB0.CTRLA |= TCB_ENABLE_bm;
}
