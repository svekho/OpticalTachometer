#include <avr/io.h>

void update_spin(int userVoltage)
{
<<<<<<< HEAD
    // Disable the peripheral
    // Writing Capture/Compare register with new duty cycle based on
    // potentiometer
    // Reset count register
    // Re-enable the module
=======
    if(userVoltage > 100)
    {
        VPORTA.OUT |= PIN2_bm;
    }
    else
    {
        VPORTA.OUT &= ~PIN2_bm;
    }
    
>>>>>>> bb4a5abb5fc67c6cece5486edd6147dde0ec48da
}
