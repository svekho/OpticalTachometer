#include <avr/io.h>
/**
 * This function gets calculated most significant digit from main.c, and sets 
 * high those pins needed to present that digit on the 7-segment display. 
 * Those pins that are not envolved with the digit will be set low.
 * @param msd
 */
void update_display(int msd)
{
    // Checking which number to play on 7-segment display
    switch(msd)
    {
        case 0 :
            VPORTC.OUT = (VPORTC.OUT|PIN7_bm|PIN0_bm|PIN1_bm|PIN2_bm|PIN3_bm|
                          PIN4_bm|PIN5_bm) & ~(PIN6_bm);
            break;
        case 1 :
            VPORTC.OUT = (VPORTC.OUT|PIN7_bm|PIN1_bm|PIN2_bm) & ~(PIN0_bm|
                          PIN3_bm|PIN4_bm|PIN5_bm|PIN6_bm);
            break;
        case 2 :
            VPORTC.OUT = (VPORTC.OUT|PIN7_bm|PIN0_bm|PIN1_bm|PIN3_bm|PIN4_bm|
                          PIN6_bm) & ~(PIN2_bm|PIN5_bm);
            break;
        case 3:
            VPORTC.OUT = (VPORTC.OUT|PIN7_bm|PIN0_bm|PIN1_bm|PIN3_bm|PIN2_bm|
                          PIN6_bm) & ~(PIN4_bm|PIN5_bm);
            break;
        case 4 :
            VPORTC.OUT = (VPORTC.OUT|PIN7_bm|PIN1_bm|PIN2_bm|PIN5_bm|PIN6_bm) &
                          ~(PIN0_bm|PIN3_bm|PIN4_bm);
            break;
        case 5 :
            VPORTC.OUT = (VPORTC.OUT|PIN7_bm|PIN0_bm|PIN2_bm|PIN3_bm|PIN5_bm|
                          PIN6_bm) & ~(PIN1_bm|PIN4_bm);
            break;
        case 6 :
            VPORTC.OUT = (VPORTC.OUT|PIN7_bm|PIN0_bm|PIN2_bm|PIN3_bm|PIN4_bm|
                          PIN5_bm|PIN6_bm) & ~(PIN1_bm);
            break;
        case 7 :
            VPORTC.OUT = (VPORTC.OUT|PIN7_bm|PIN0_bm|PIN1_bm|PIN2_bm) &~(PIN3_bm
                          |PIN4_bm|PIN5_bm|PIN6_bm);
                          
            break;
        case 8 :
            VPORTC.OUT = (VPORTC.OUT|PIN7_bm|PIN0_bm|PIN1_bm|PIN3_bm|PIN2_bm|
                          PIN4_bm|PIN5_bm|PIN6_bm);
            break;
        case 9 :
            VPORTC.OUT = (VPORTC.OUT|PIN7_bm|PIN0_bm|PIN1_bm|PIN3_bm|PIN2_bm|
                          PIN5_bm|PIN6_bm) & ~(PIN4_bm);
            break;
    }
}
