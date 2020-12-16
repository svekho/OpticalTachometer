#include <avr/io.h>
/**
 * This function updates the TCB PWM duty cycle based on the parameter received
 * from main.c. 
 * @param userVoltage
 */
void update_spin(uint8_t userVoltage)
{
    // Variable to store hexadecimal value of given userVoltage
    //uint8_t userHex ;
    
    // Disable the peripheral
    TCB0.CTRLA &= ~TCB_ENABLE_bm;
    TCB0.CTRLB &= ~TCB_CCMPEN_bm;
    
    // Writing Capture/Compare register with new duty cycle based on
    // potentiometer
    TCB0.CCMPH = userVoltage;
    
    // Reset count register
    TCB0.CNT = 0x0000;
    
    // Re-enable the module
    TCB0.CTRLA |= TCB_ENABLE_bm;
    TCB0.CTRLB |= TCB_CCMPEN_bm;
}
