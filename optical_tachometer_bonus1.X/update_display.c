#include <avr/io.h>

void update_display(int msd)
{
    // Checking which number to play on 7-segment display
    // HUOM KUN SIIVOTAAN NII pinie samanaikaisuutee t‰ytyy kiinnitt‰‰ huomioo
    // esimerkki case 1 kohdal!!
    switch(msd)
    {
        case 0 :
            VPORTC.OUT |= PIN7_bm;
            VPORTC.OUT |= PIN0_bm;
            VPORTC.OUT |= PIN1_bm;
            VPORTC.OUT |= PIN2_bm;
            VPORTC.OUT |= PIN3_bm;
            VPORTC.OUT |= PIN4_bm;
            VPORTC.OUT |= PIN5_bm;
            VPORTC.OUT &= ~PIN6_bm;
            break;
        case 1 :
            VPORTC.OUT |= PIN7_bm;
            VPORTC.OUT &= ~PIN0_bm;
            VPORTC.OUT |= PIN1_bm;
            VPORTC.OUT |= PIN2_bm;
            VPORTC.OUT &= ~PIN3_bm;
            VPORTC.OUT &= ~PIN4_bm;
            VPORTC.OUT &= ~PIN5_bm;
            VPORTC.OUT &= ~PIN6_bm;
            // VPORTC.OUT = (VPORTC.OUT | PIN7_bm | PIN1_bm | PIN2_bm) & ~(PIN0_bm | PIN3_bm | pin4_bm | pin5_bm | pin6_bm);
            break;
        case 2 :
            VPORTC.OUT |= PIN7_bm;
            VPORTC.OUT |= PIN0_bm;
            VPORTC.OUT |= PIN1_bm;
            VPORTC.OUT &= ~PIN2_bm;
            VPORTC.OUT |= PIN6_bm;
            VPORTC.OUT |= PIN3_bm;
            VPORTC.OUT |= PIN4_bm;
            VPORTC.OUT &= ~PIN5_bm;
            break;
        case 3:
            VPORTC.OUT |= PIN7_bm;
            VPORTC.OUT |= PIN0_bm;
            VPORTC.OUT |= PIN1_bm;
            VPORTC.OUT |= PIN2_bm;
            VPORTC.OUT |= PIN3_bm;
            VPORTC.OUT &= ~PIN4_bm;
            VPORTC.OUT &= ~PIN5_bm;
            VPORTC.OUT |= PIN6_bm;
            break;
        case 4 :
            VPORTC.OUT |= PIN7_bm;
            VPORTC.OUT &= ~PIN0_bm;
            VPORTC.OUT |= PIN1_bm;
            VPORTC.OUT |= PIN2_bm;
            VPORTC.OUT &= ~PIN3_bm;
            VPORTC.OUT &= ~PIN4_bm;
            VPORTC.OUT |= PIN5_bm;
            VPORTC.OUT |= PIN6_bm;
            break;
        case 5 :
            VPORTC.OUT |= PIN7_bm;
            VPORTC.OUT |= PIN0_bm;
            VPORTC.OUT |= PIN5_bm;
            VPORTC.OUT |= PIN6_bm;
            VPORTC.OUT |= PIN2_bm;
            VPORTC.OUT |= PIN3_bm;
            VPORTC.OUT &= ~PIN1_bm;
            VPORTC.OUT &= ~PIN4_bm;
            break;
        case 6 :
            VPORTC.OUT |= PIN7_bm;
            VPORTC.OUT |= PIN0_bm;
            VPORTC.OUT &= ~PIN1_bm;
            VPORTC.OUT |= PIN2_bm;
            VPORTC.OUT |= PIN3_bm;
            VPORTC.OUT |= PIN4_bm;
            VPORTC.OUT |= PIN5_bm;
            VPORTC.OUT |= PIN6_bm;
            break;
        case 7 :
            VPORTC.OUT |= PIN7_bm;
            VPORTC.OUT |= PIN0_bm;
            VPORTC.OUT |= PIN1_bm;
            VPORTC.OUT |= PIN2_bm;
            VPORTC.OUT &= ~PIN3_bm;
            VPORTC.OUT &= ~PIN4_bm;
            VPORTC.OUT &= ~PIN5_bm;
            VPORTC.OUT &= ~PIN6_bm;
            break;
        case 8 :
            VPORTC.OUT |= PIN7_bm;
            VPORTC.OUT |= PIN0_bm;
            VPORTC.OUT |= PIN1_bm;
            VPORTC.OUT |= PIN2_bm;
            VPORTC.OUT |= PIN3_bm;
            VPORTC.OUT |= PIN4_bm;
            VPORTC.OUT |= PIN5_bm;
            VPORTC.OUT |= PIN6_bm;
            break;
        case 9 :
            VPORTC.OUT |= PIN7_bm;
            VPORTC.OUT |= PIN0_bm;
            VPORTC.OUT |= PIN1_bm;
            VPORTC.OUT |= PIN2_bm;
            VPORTC.OUT |= PIN3_bm;
            VPORTC.OUT &= ~PIN4_bm;
            VPORTC.OUT |= PIN5_bm;
            VPORTC.OUT |= PIN6_bm;
            break;
    }
}
