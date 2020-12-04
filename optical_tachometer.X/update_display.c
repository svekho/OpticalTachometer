#include <avr/io.h>

void update_display(int rpm)
{
    // Checking which number to play on 7-segment display
    switch(rpm)
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
