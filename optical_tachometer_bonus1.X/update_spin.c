#include <avr/io.h>

void update_spin(int userVoltage)
{
    if(userVoltage > 100)
    {
        VPORTA.OUT |= PIN2_bm;
    }
    else
    {
        VPORTA.OUT &= ~PIN2_bm;
    }
    
}
