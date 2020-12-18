/**
 * File: spin.c
 * 
 * DESCRIPTION
 *      This is a single source file for updating the spinning speed of DC 
 *      motor. It receives a parameter userVoltage which is the value received 
 *      from potentiometer. The TCB will be disabled for the process of changing
 *      the motor speed. Motor speed is updated by changing the duty cycle of
 *      PWM that is controlled by TCB.
 */

#include <avr/io.h>

void spin_update(uint8_t userVoltage)
{    
    // Disable the peripheral to update TCB correctly without corrupts
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
    
    // Writing Capture/Compare register with new duty cycle based on
    // potentiometer
    TCB0.CCMPH = userVoltage;
    
    // Re-enable the module
    TCB0.CTRLA |= TCB_ENABLE_bm;
}
