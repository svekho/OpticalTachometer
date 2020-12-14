#define F_CPU 3333333

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

void update_lcd(int rpm)

{
    // Placeholder for calculated rpm
    int rpmOriginal = rpm;
    // This will be the number of elements in an array
    int num = 0; 
    // Counting how many chars will be in array, dividing with 10 to move
    // towards the final digit of rpm (+1 for each round to num)
    do
    {
        rpm = rpm/10;
        num++;
    }
    while(rpm != 0);
<<<<<<< HEAD
    // String for rpm value to be printed in LCD (+1 for null)
    unsigned char rpmValue[num+1];
    // Setting the rpm value to the array (reversed because we are printing)
    // them to LCD next in ascending order
=======

    // String for rpm value to be printed in LCD (+1 for null)
    char rpmValue[num+1];
    
    // Setting the rpm value to the array (reversed because we are printing)
    // them to LCD next in descending order
>>>>>>> 7f86f4e9acf98f7e5c408d730ec78ab0dd005aa9
    for (int i=num-1; i>=0; i--)
    {
        // + '0' to change integer into char
        rpmValue[i] = rpmOriginal%10 + '0';
        rpmOriginal = rpmOriginal/10;
    }

    // String to be printed after rpm value
    unsigned char rpmString[4] = "RPM";
    
    // Clear display
    PORTD.OUT = 0x01;
    PORTB.OUT &= ~PIN4_bm;
    PORTB.OUT |= PIN3_bm;
    PORTB.OUT &= ~PIN3_bm;
    _delay_ms(20);
    
    // Set cursor back at home position
    PORTD.OUT = 0x80;
    PORTB.OUT &= ~PIN4_bm;
    PORTB.OUT |= PIN3_bm;
    PORTB.OUT &= ~PIN3_bm;
    _delay_ms(20);
    
    // Looping through rpmValue to print each number of rpm
    for (int k = 0; k<num ;k++)
    {
        PORTD.OUT = rpmValue[k];
        PORTB.OUT |= PIN4_bm;
        PORTB.OUT |= PIN3_bm;
        _delay_us(1);
        PORTB.OUT &= ~PIN3_bm;
        _delay_ms(20);
    }
    
    // Setting 'space' between numbers and "RPM"
    PORTD.OUT = 0x20;
    PORTB.OUT |= PIN4_bm;
    PORTB.OUT |= PIN3_bm;
    PORTB.OUT &= ~PIN3_bm;
    _delay_ms(20);
    
    // Looping through the characters of rpmString and printing them to LCD
    for (int j=0; j<3; j++)
    {
        PORTD.OUT = rpmString[j];
        PORTB.OUT |= PIN4_bm;
        PORTB.OUT |= PIN3_bm;
        _delay_us(1);
        PORTB.OUT &= ~PIN3_bm;
        _delay_ms(20);
    }
    
}
