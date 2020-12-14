/*
 * File:   main.c
 * Author: Sandra Ekholm, Amy Nymalm and Nea Kontturi
 * BONUS 1!!!!!!!
 * Created on 02 December 2020, 13:32
 */
#define F_CPU 3333333
#define USART0_BAUD_RATE(BAUD_RATE) \
    ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)
#define MIN_VOLT_DIFF (30)
#define LWR_THRESH (5)
#include <xc.h>
#include "update_display.h"
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
void SEGMENT_init(void);
void PWM_CLOCK_init(void);
void TCB_init(void);
void calibrate_threshold(void);

// Global variable to store adc result
volatile uint16_t adcValue;
// Variable to count rotations
volatile uint16_t rotations;
// Global variable msd (indicates most significant digit
// value of rotating propeller rpm)
uint8_t msd;
// int to check if the propeller is in front of the LDR, and makes sure, the 
// propeller is only counted once
volatile uint8_t isPropOn;
// Checking whether should update display or calculate rotations from adc
volatile uint8_t segmentUpdate;
// Value for storing the voltage level used as indicator whether there is a 
// propellor in front of the LDR or not.
uint16_t voltThreshold;

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
    // Selecting number of RTC clock cycles (16384) because we want interrupt
    // twice a second and enable periodic interrupt timer
    RTC.PITCTRLA = RTC_PERIOD_CYC16384_gc | RTC_PITEN_bm;
}

// Configuring ADC
void ADC_init(void)
{
    // Set port E pin 0 as input
    PORTE.DIRCLR = PIN0_bm;
    
    // Disable input buffer, pull-up resistor disabled by default 
    PORTE.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    
    // Voltage reference is 1,5V (internal reference voltage already defined) 
    // and prescaler of 16
    ADC0.CTRLC |= ADC_REFSEL_INTREF_gc | ADC_PRESC_DIV16_gc;
    
    // Resolution 10 bits
    ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
    
    // Enable ADC
    ADC0.CTRLA |= ADC_ENABLE_bm;
    
    // Selecting AN8 (PE0) to be connected to ADC
    ADC0.MUXPOS =  ADC_MUXPOS_AIN8_gc;
    
    // Freerun mode enabled (next conversion starts automatically)
    ADC0.CTRLA |= ADC_FREERUN_bm;
    
    // Enable interrupts
    ADC0.INTCTRL |= ADC_RESRDY_bm;
    
}
// Initialize 7-segment display
void SEGMENT_init(void)
{
    // 7-segment display configurations
    
    // Port C configured as an output
    VPORTC.DIR |= (PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm);
    VPORTC.DIR |= (PIN6_bm | PIN7_bm);
}
// Initialize clock for TCB's PWM mode
void PWM_CLOCK_init(void)
{
    // Enable writing to protected register
    CPU_CCP = CCP_IOREG_gc;
    // Selecting highest prescaler division (64), and enable prescaler
    CLKCTRL.MCLKCTRLB = CLKCTRL_PDIV_64X_gc | CLKCTRL_PEN_bm;
    
    // Enable writing to protected register again
    CPU_CCP = CCP_IOREG_gc;
    // Selecting the source for main clock, ultra low power oscillator
    CLKCTRL.MCLKCTRLA = CLKCTRL_CLKSEL_OSCULP32K_gc;
    
    // Waiting for system oscillator change to finish, halting for waiting time
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm)
    {
        ;
    }
}

// Initialize TCB PWM mode and output pin (to motor) for PWM signals
void TCB_init(void)
{
    // Configure correct pin as output to DC motor, and first as low
    VPORTA.DIR |= PIN2_bm;
    VPORTA.OUT &= ~PIN2_bm;
    
    // Duty cycle 50 % (CCMPH = 0x80), PWM signal period 1 sec (CCMPL = 0xFF)
    TCB3.CCMP = 0x80FF;
    // Enable TCB, and divide clock more with 2
    TCB3.CTRLA |= TCB_ENABLE_bm | TCB_CLKSEL_CLKDIV2_gc;
    // Enable output signal of Compare/Capture, and TCB configured in 
    // 8-bit PWM mode
    TCB3.CTRLB |= TCB_CCMPEN_bm | TCB_CNTMODE_PWM8_gc;
}

// RTC interrupt
ISR(RTC_PIT_vect)
{
    // Clearing interrupt flag
    RTC.PITINTFLAGS = RTC_PI_bm;
    // Segment needs to be updated
    segmentUpdate = 1;
}

// ADC interrupt, ADC conversion is done
ISR(ADC0_RESRDY_vect)
{
    // Clearing interrupt flag
    ADC0.INTFLAGS = ADC_RESRDY_bm;
    // Setting the value adc measured
    adcValue = ADC0.RES;
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
    // No segment updating
    segmentUpdate = 2;
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

int main(void) 
{
    // In the beginning the value of parameters is 0
    adcValue = 0;
    rotations = 0;
    // Variable to store the rpm calculated from rotations
    uint16_t rpm = 0;
    msd = 0;
    isPropOn = 0;
    segmentUpdate = 0;
    // Setting internal reference voltage to 1.5V
    VREF.CTRLA = VREF_ADC0REFSEL_1V5_gc;
    // Initialize output to putty
    USART0_init();
    // Initialize 7-segment display
    SEGMENT_init();
    // Initialize ADC and its input pin
    ADC_init();
    // Initialize RTC
    RTC_init();
    // Initialize PWM clock
    PWM_CLOCK_init();
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
        // Checking whether the interrupt was about segment updating
        if (segmentUpdate == 1)
        {
            // Disable interrupts for segment updating
            cli();
            // rpm calculated from rotations 
            //(120 because observation interval is 0,5s)
            rpm = rotations*120;
            // testing
            printf("%i rpm\r\n", rpm);
            // Counting first digit from rpm
            while(rpm >=10)
            {
            // Dividing with 10 until only one number is left -> msd
            rpm = rpm / 10;
            }
            msd = rpm;
            // Updating display to msd
            update_display(msd);
            // Resetting rpm and rotations for next round
            rpm = 0;
            rotations=0;
            // Enable interrupts again
            sei();
        }
        // Checking whether interrupt was from result-ready adc
        else if (segmentUpdate == 2)
        {
            //when the propeller is not in front of the LDR, the
            // rotations value is updated.
            if (isPropOn == 1)
            {
                // Disable global interrupts to update propeller
                cli();
                rotations++;
                //printf("%i\r\n",rotations);
                //printf("%i\r\n",adcValue);
                isPropOn = 2;
                // Enable global interrupts
                sei();
            }
        }
    }
    return 0;
}

