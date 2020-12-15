#include <avr/io.h>

void update_spin(int userVoltage)
{
    // Disable the peripheral
    TCB3.CTRLA &= ~TCB_ENABLE_bm;
    // Writing Capture/Compare register with new duty cycle based on
    // potentiometer
    // Reset count register
    // Re-enable the module
    TCB3.CTRLA |= TCB_ENABLE_bm;
}
