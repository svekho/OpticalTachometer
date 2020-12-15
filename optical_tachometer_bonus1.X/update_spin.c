#include <avr/io.h>

void update_spin(int userVoltage)
{
    // Variable to store hexadecimal value of given userVoltage
    //uint8_t userHex;
    
    
    // Disable the peripheral
    TCB3.CTRLA &= ~TCB_ENABLE_bm;
    
    // Writing Capture/Compare register with new duty cycle based on
    // potentiometer
    TCB3.CCMPH = userVoltage;
    
    // Reset count register
    TCB3.CNT = 0x0000;
    
    // Re-enable the module
    TCB3.CTRLA |= TCB_ENABLE_bm;
}
