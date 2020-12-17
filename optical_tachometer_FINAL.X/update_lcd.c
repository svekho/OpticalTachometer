/**
 * File: update_lcd.c 
 * 
 * DESCRIPTION
 *      This is a single source file for implementation of LCD update process.
 *      It receives a parameter rpm which is calculated in main.c. The parameter 
 *      is stored into an array from which the numbers are sent to LCD display 
 *      one by one. After whole value has been sent, also string "RPM" will be
 *      sent similarly.
 */

#define F_CPU 3333333

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

void update_lcd(int rpm)

{
    // Placeholder for calculated rpm
    int rpmOriginal = rpm;

    // This will be the number of elements in an array, initialized as 0 in the
    // beginning
    int num = 0; 
    // Counting how many chars will be in array, dividing with 10 to move
    // towards the final digit of rpm (+1 for each round to num because one 
    // digit has been passed)
    do
    {
        rpm = rpm/10;
        num++;
    }
    while(rpm != 0);

    // String for rpm value to be printed in LCD (+1 for null)
    char rpmValue[num+1];
    
    // Setting the rpm value to the array (reversed because we are printing
    // them to LCD next in ascending order)
    for (int i=num-1; i>=0; i--)
    {
        // + '0' to change integer into char
        rpmValue[i] = rpmOriginal%10 + '0';
        rpmOriginal = rpmOriginal/10;
    }

    // String to be printed after rpm value
    char rpmString[4] = "RPM";
    
    // Using VPORTs to make operations faster
    
    // Clear display
    VPORTD.OUT = 0x01;
    VPORTB.OUT &= ~PIN4_bm;
    VPORTB.OUT |= PIN3_bm;
    VPORTB.OUT &= ~PIN3_bm;
    // Settling time for LCD display to operate on command
    _delay_ms(1);
    
    // Set cursor back at home position
    VPORTD.OUT = 0x80;
    VPORTB.OUT &= ~PIN4_bm;
    VPORTB.OUT |= PIN3_bm;
    VPORTB.OUT &= ~PIN3_bm;
    // Settling time for LCD display to operate on command
    _delay_ms(1);
    
    // Looping through rpmValue to print each number of rpm
    for (int k = 0; k<num ;k++)
    {
        VPORTD.OUT = rpmValue[k];
        VPORTB.OUT |= PIN4_bm;
        VPORTB.OUT |= PIN3_bm;
        VPORTB.OUT &= ~PIN3_bm;
        // Settling time for LCD to operate on displaying the data received
        _delay_ms(1);
    }
    
    // Setting 'space' between numbers and "RPM"
    VPORTD.OUT = 0x20;
    VPORTB.OUT |= PIN4_bm;
    VPORTB.OUT |= PIN3_bm;
    VPORTB.OUT &= ~PIN3_bm;
    // Settling time for LCD to operate on displaying the data received
    _delay_ms(1);
    
    // Looping through the characters of rpmString and printing them to LCD
    for (int j=0; j<3; j++)
    {
        VPORTD.OUT = rpmString[j];
        VPORTB.OUT |= PIN4_bm;
        VPORTB.OUT |= PIN3_bm;
        VPORTB.OUT &= ~PIN3_bm;
        // Settling time for LCD to operate on displaying the data received
        _delay_ms(1);
    }
    
}
