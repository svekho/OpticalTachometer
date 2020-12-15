#include <avr/io.h>

void update_spin(int userVoltage)
{
    // Disable the peripheral
    // Writing Capture/Compare register with new duty cycle based on
    // potentiometer
    // Reset count register
    // Re-enable the module
}
