/**
 * File: lcd.c 
 * 
 * DESCRIPTION
 *      This is a single source file for implementation of LCD update process
 *      and LCD initialize process. Lcd_init() initializes output pins to
 *      display, another function lcd_update updates display when requested in
 *      main.c. It receives a parameter rpm which is calculated in main.c. The 
 *      parameter is stored into an array from which the numbers are sent to LCD 
 *      display one by one. After whole value has been sent, also string "RPM"
 *      will be sent similarly.
 */

#define F_CPU 3333333

// Value which is used to divide rpm value, aims to get access on each digit of 
// rpm value or divide rpm further to get access on next digit
#define RPM_VALUE_DIV (10)

// Values to operate on LCD display
#define LCD_CLEAR 0x01
#define LCD_CURSOR_HOME 0x80
#define LCD_SET_SPACE 0x20

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

// Initialize LCD
void lcd_init(void)
{
    // Configuring pins LCD uses as outputs, operations don't need to be atomic  
    // (because no global interrupts are enabled) but operations shall be fast
    // (using VPORTS)
    PORTB.DIRSET = PIN3_bm | PIN4_bm | PIN5_bm;
    VPORTD.DIR |= PIN0_bm|PIN1_bm|PIN2_bm|PIN3_bm|PIN4_bm|PIN5_bm|PIN6_bm;
    VPORTD.DIR |= PIN7_bm;
    
    // Enable LCD backlight
    VPORTB.OUT |= PIN5_bm;
}

// Update LCD display
void lcd_update(uint16_t rpm)
{
    // Placeholder for received rpm
    uint16_t rpmOriginal = rpm;

    // Variable "num" will be the number of elements in an array, initialized as
    // 0 in the beginning
    uint8_t num = 0; 
    
    // Counting how many chars will be in string, dividing rpm with 10 to go
    // over each digit of rpm (+1 for each round to num because one digit has
    // been passed)
    do
    {
        rpm = rpm/RPM_VALUE_DIV;
        num++;
    }
    while(rpm != 0);

    // String for rpm value, which will be printed in LCD (string size num and 
    // +1 for null character)
    char rpmValue[num+1];
    
    // Setting the rpm value to the array, looping through each element of
    // string and setting a digit into it (reversed order because we are 
    // printing them to LCD next in ascending order)
    for (int i=num-1; i>=0; i--)
    {
        // + '0' to change integer into char
        rpmValue[i] = rpmOriginal%RPM_VALUE_DIV + '0';
        // Dividing with 10 to get access into next digit of rpm
        rpmOriginal = rpmOriginal/RPM_VALUE_DIV;
    }

    // String to be printed after rpm value
    char rpmString[4] = "RPM";
    
    // Using VPORTs to make operations faster
    
    // Clear display
    VPORTD.OUT = LCD_CLEAR;  // Instruction that will be sent to LCD
    VPORTB.OUT &= ~PIN4_bm; // Register select: instruction input for LCD   
    VPORTB.OUT |= PIN3_bm;  // Enable signal into LCD 
    VPORTB.OUT &= ~PIN3_bm; // Disable signal to LCD
    // Settling time for LCD display to operate on command
    _delay_ms(1);
    
    // Set cursor back at home position
    VPORTD.OUT = LCD_CURSOR_HOME;
    VPORTB.OUT &= ~PIN4_bm;
    VPORTB.OUT |= PIN3_bm;
    VPORTB.OUT &= ~PIN3_bm;
    // Settling time for LCD display to operate on command
    _delay_ms(1);
    
    // Looping through rpmValue to send each digit of rpm to LCD
    for (int k = 0; k<num ;k++)
    {
        VPORTD.OUT = rpmValue[k];   // Data that will be sent to LCD
        VPORTB.OUT |= PIN4_bm;  // Register select: data input for LCD
        VPORTB.OUT |= PIN3_bm;  // Enable signal into LCD
        VPORTB.OUT &= ~PIN3_bm; // Disable signal to LCD
        // Settling time for LCD to operate on displaying the data received
        _delay_ms(1);
    }
    
    // Setting 'space' between rpm digits and string "RPM"
    VPORTD.OUT = LCD_SET_SPACE;
    VPORTB.OUT |= PIN4_bm;
    VPORTB.OUT |= PIN3_bm;
    VPORTB.OUT &= ~PIN3_bm;
    // Settling time for LCD to operate on displaying the data received
    _delay_ms(1);
    
    // Looping through the characters of rpmString and sending them to LCD
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
