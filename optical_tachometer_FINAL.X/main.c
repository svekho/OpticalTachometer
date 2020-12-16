/*
 * File:   main.c
 * Author: Sandra Ekholm, Amy Nymalm and Nea Kontturi
 *
 * Created on 02 December 2020, 13:32
 */
#define F_CPU 3333333
#define USART0_BAUD_RATE(BAUD_RATE) \
    ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
#define MIN_VOLT_DIFF (30)
#define LWR_THRESH (5)

#include <xc.h>
#include "update_lcd.h"
#include "update_spin.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <util/delay.h>

// Function declarations
static void USART0_sendChar(char c);
static int USART0_printChar(char c, FILE *stream);
static void USART0_init(void);
void RTC_init(void);
void ADC_init(void);
void LCD_init(void);
void calibrate_threshold(void);
void TCB_init(void);

// Global variable to store adc result
volatile uint16_t adcValue;
// Variable to count rotations
volatile uint16_t rotations;
// int to check if the propeller is in front of the LDR, and makes sure, the 
// propeller is only counted once
volatile uint8_t isPropOn;
// Checking whether should update LCD or calculate rotations from adc
volatile uint8_t lcdUpdate;
// Value for storing the voltage level used as indicator whether there is a 
// propellor in front of the LDR or not.
uint16_t voltThreshold;
// Indicates potentiometer value user wants to give for spinning speed
uint8_t userVoltage;
// Indicates whether we are going to measure LDR or potentiometer, every 100th
// time we need to read potentiometer
volatile uint8_t potentRead;

// Fuction for sending text to computer terminal/putty
static void USART0_sendChar(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;
    }
    USART0.TXDATAL = c;
}

static int USART0_printChar(char c, FILE *stream)
{ 
    USART0_sendChar(c);
    return 0; 
}

static FILE USART_stream =  \
    FDEV_SETUP_STREAM(USART0_printChar, NULL, _FDEV_SETUP_WRITE);

// Intitialising connection to  computer terminal/putty
static void USART0_init(void)
{
    PORTA.DIR |= PIN0_bm;
    
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600); 
    
    USART0.CTRLB |= USART_TXEN_bm;  
    
    stdout = &USART_stream;
}

// Configuring RTC
void RTC_init(void)
{
    // Helping variable
    uint8_t temp;
    
    /* Initialize 32.768kHz Oscillator: */
    /* Disable oscillator: */
    // Set temp as the value of register at that moment
    temp = CLKCTRL.XOSC32KCTRLA;
    // Set the final bit of temp into zero (to get access to SEL and CSUT)
    temp &= ~CLKCTRL_ENABLE_bm;
    // Writing to protected register
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    // Waiting until status bit becomes 0
    while(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
    {
        ;
    }
    
    // Set temp as the value of register at the moment
    temp = CLKCTRL.XOSC32KCTRLA;
    // Using external crystal: SEL = 0
    temp &= ~CLKCTRL_SEL_bm;
    // Writing to protected register
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    /* Enable oscillator: */
    temp = CLKCTRL.XOSC32KCTRLA;
    // Setting the oscillator enable bit
    temp |= CLKCTRL_ENABLE_bm;
    // Writing to protected register
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    /* Initialize RTC: */
    // Waiting all registers to be synchronized
    while (RTC.STATUS > 0)
    {
        ;
    }
    
    // External Clock Oscillator clock is selected
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;
    // Allow RTC running in debug mode
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    // Enable periodic interrupt
    RTC.PITINTCTRL = RTC_PI_bm;
    // Selecting number of RTC clock cycles (32768) because we want interrupt
    // once a second and enable periodic interrupt timer
    RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc | RTC_PITEN_bm;
}

// Configuring ADC
void ADC_init(void)
{
    // Set port E pin 0 as input for LDR and port F pin 4 input 
    // for potentiometer
    PORTE.DIRCLR = PIN0_bm;
    PORTF.DIRCLR = PIN4_bm;
    
    // Disable input buffers, pull-up resistor disabled by default 
    PORTE.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTF.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    
    // Voltage reference is 1,5V at first (internal reference voltage already
    // defined) and prescaler of 16
    ADC0.CTRLC |= ADC_REFSEL_INTREF_gc | ADC_PRESC_DIV16_gc;
    
    // Resolution 10 bits
    ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
    
    // Enable ADC
    ADC0.CTRLA |= ADC_ENABLE_bm;
    
    // Selecting AN8 (PE0) to be connected to ADC (LDR will be connected first)
    ADC0.MUXPOS =  ADC_MUXPOS_AIN8_gc;
    
    // Freerun mode enabled (next conversion starts automatically)
    ADC0.CTRLA |= ADC_FREERUN_bm;
    
    // Enable interrupts
    ADC0.INTCTRL |= ADC_RESRDY_bm;
    
}

// Initialize LCD
void LCD_init(void)
{
    // Configuring pins LCD uses as outputs
    VPORTB.DIR |= PIN3_bm| PIN4_bm | PIN5_bm;
    //VPORTB.DIR |= PIN4_bm;
    //VPORTB.DIR |= PIN5_bm;
    VPORTD.DIR |= PIN0_bm|PIN1_bm|PIN2_bm|PIN3_bm|PIN4_bm|PIN5_bm|PIN6_bm;
    VPORTD.DIR |= PIN7_bm;
    
    // Enable backlight
    VPORTB.OUT |= PIN5_bm;
}

// Initialize TCB3 PWM mode and output pin (to motor) for PWM signals
void TCB_init(void)
{
    // Configure correct pin as output to DC motor, and first as low
    VPORTA.DIR |= PIN2_bm;
    VPORTA.OUT |= PIN2_bm;

    // Duty cycle 50 % first (CCMPH = 0x80), PWM signal period 1 sec 
    // (CCMPL = 0xFF)
    TCB0.CCMP = 0x80FF;
    // Enable TCB, and divide clock with 2
    TCB0.CTRLA |= TCB_ENABLE_bm | TCB_CLKSEL_CLKDIV2_gc;
    // Enable output signal of Compare/Capture, and TCB configured in 
    // 8-bit PWM mode
    TCB0.CTRLB |= TCB_CCMPEN_bm | TCB_CNTMODE_PWM8_gc;
    // Also runs in sleep mode
    TCB0.CTRLA |= TCB_RUNSTDBY_bm;
}

// Calibrates the threshold for light vs dark, depending on current lighting.
void calibrate_threshold(void)
{
    // temporary values for calibration (array)
    int calibTab [4] = {0};
    
    printf("Calibrating lighting, one moment...\r\n");
    
    // Measuring current light conditions in ldr environment 100 times
    for (int i = 0; i<=9999; i++)
    {
        // Waiting adc result to be ready
        while (!(ADC0.INTFLAGS & ADC_RESRDY_bm))
        {
            ;
        }
        // Setting every 33th measured value to the array
        if ((i % 3333) == 0)
        {
            calibTab[i/3333 - 1] = ADC0.RES;
            printf("%d\r\n", calibTab[i/3333 - 1]);
        }
        // Allowing next adc conversion begin
        ADC0.INTFLAGS = ADC_RESRDY_bm;
    }

    //set voltThreshold a little bit above the average of the calibration values
    voltThreshold = (calibTab[0]+calibTab[1]+calibTab[2])/3 + MIN_VOLT_DIFF;
    printf("Calibration complete!\r\n");
    printf("Threshold voltage: %i\r\n\n", voltThreshold);
    return;
}

// RTC interrupt
ISR(RTC_PIT_vect)
{
    // Clearing interrupt flag
    RTC.PITINTFLAGS = RTC_PI_bm;
    // LCD needs to be updated
    lcdUpdate = 1;
}

// ADC interrupt, ADC conversion is done
ISR(ADC0_RESRDY_vect)
{
    // Clearing interrupt flag
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    // Setting the value adc measured
    adcValue = ADC0.RES;
    
    if (potentRead == 100)
    {
        // Updating the propellor
        lcdUpdate = 3;
    }
    else
    {
        //AdcValue: propeller is in front of LDR, calibrated in the beginning
        if (adcValue>voltThreshold)
        {
            //makes sure the rotations are only updated once per rotation
            if(isPropOn == 0)
            {
                isPropOn=1;
            }
        }
        else if (adcValue<(voltThreshold - LWR_THRESH))
        {
            isPropOn = 0;
        }
        // No segment updating, LDR updating
        lcdUpdate = 2;
    }
}


int main(void) 
{
    // In the beginning the value of parameters is 0
    adcValue = 0;
    rotations = 0;
    // Variable to store the rpm calculated from rotations
    uint16_t rpm = 0;
    isPropOn = 0;
    lcdUpdate = 0;
    userVoltage = 0;
    potentRead = 0;
    // Setting internal reference voltage to 1.5V
    VREF.CTRLA = VREF_ADC0REFSEL_1V5_gc;
    // Initialize output to putty
    USART0_init();
    // Initialize LCD display
    LCD_init();
    // Initialize ADC and its input pin
    ADC_init();
    // Initialize RTC
    RTC_init();
    // Initialize TCB to 8-bit PWM mode(and its output pin)
    TCB_init();
   
    // Setting IDLE as sleep mode
    set_sleep_mode(SLPCTRL_SMODE_IDLE_gc);
    // Start ADC conversion
    ADC0.COMMAND = ADC_STCONV_bm;
    
    // Calibrates current lighting without anything in front of the LDR
    calibrate_threshold();
    
    // Enable global interrupts
    sei();
    
    while(1)
    {
        // Entering sleep mode every time after wake up
        sleep_mode();
        // Checking whether the interrupt was about LCD updating
        if (lcdUpdate == 1)
        {
            // Disable interrupts for segment updating
            cli();
            // rpm calculated from rotations 
            //(60 because observation interval is 1,0s)
            rpm = rotations*60;
            // Updating display to RPM
            update_lcd(rpm);
            // Resetting rpm and rotations for next round
            rpm = 0;
            rotations=0;
            // Enable interrupts again
            sei();
        }
        // Checking whether interrupt was from result-ready adc (LDR))
        else if (lcdUpdate == 2)
        {
            //when the propeller is not in front of the LDR, the
            // rotations value is updated.
            if (isPropOn == 1)
            {
                // Disable global interrupts to update propeller
                cli();
                rotations++;
                isPropOn = 2;
                // Enable global interrupts
                sei();
            }
            // Checking if next conversion shall be taken from potentiometer
            if (potentRead == 99)
            {
                cli();
                // Switching ADC channel to potentiometer
                ADC0.MUXPOS = ADC_MUXPOS_AIN14_gc;
                // Settling time for ADC to switch the channel
                _delay_us(10);
                // Setting new reference voltage (VDD)
                ADC0.CTRLC |= ADC_REFSEL_VDDREF_gc;
                potentRead++;
                sei();
            }
            else
            {
            cli();
            potentRead++;
            sei();
            }
        }
        else if (lcdUpdate == 3)
        {
            cli();
            // Potentiometer value changed from 10 bits to 8
            userVoltage = adcValue>>2;
            // Updates motor speed
            update_spin(userVoltage);
            // Switching ADC channel back to LDR
            ADC0.MUXPOS =  ADC_MUXPOS_AIN8_gc;
            // Settling time for ADC to switch the channel
            _delay_us(10);
            // Setting reference voltage back 1.5 V for LDR
            ADC0.CTRLC |= ADC_REFSEL_INTREF_gc;
            potentRead = 0;
            sei();
        }
    }
    return 0;
}
