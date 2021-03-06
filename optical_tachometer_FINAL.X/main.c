/*
 * File:   main.c - Optical Tachometer / DTE0068 (2020) Course Project
 * Authors: Sandra Ekholm <svekho@utu.fi> 
 *          Amy Nymalm <aanyma@utu.fi> 
 *          Nea Kontturi <nckont@utu.fi>
 * 
 * DESCRIPTION
 *      This is the implementation of project Optical Tachometer. The code is 
 *      divided into three source files. Two other files include updates of 
 *      two components that are used. With ADC the microcontroller reads LDR's
 *      values that will change when the DC motor is spinning between LDR and 
 *      LED. Based on values LDR gives, the RPM is calculated and sent to 
 *      connected LCD display, which updates once a minute. The spinning of DC 
 *      motor can be changed with potentiometer that is also connected to ADC.
 *      
 *
 * Created on 02 December 2020, 13:32
 * 2020-12-02 RTC initialized succesfully.
 * 2020-12-04 Connection to Putty created.
 * 2020-12-07 ADC initialized succesfully.
 * 2020-12-08 RPM calculation added and interrupts for RTC and ADC created.
 *            Calibration of LDR light added.
 * 2020-12-11 LCD display initialized and configurated to work successfully.
 * 2020-12-14 DC motor connected.
 * 2020-12-15 TCB initialized and configurated. Spinning updating for motor
 *            created.
 * 2020-12-16 Delays added for ADC channel changing.
 * 2020-12-17 Final implementation of project.
 * 2020-12-18 Final corrected version of project.
 * 
 */

#define F_CPU 3333333
#define USART0_BAUD_RATE(BAUD_RATE) \
    ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

// A boundary value which will be added to calculated voltThreshold, the ADC 
// measurements will be more stable and reliable when deciding whether propellor
// is in front of LDR or not. Through testing value 30 has been estimated as
// suitable value
#define MIN_VOLT_DIFF (30)

// Value that will be subtracted from voltThreshold to make sure LDR measurement
// doesn't cause unstable conditions when calculating rpm
#define LWR_THRESH (5)

#include "lcd.h"
#include "spin.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <util/delay.h>

// Function declarations
static void usart0_send_char(char c);
static int usart0_print_char(char c, FILE *stream);
static void usart0_init(void);
void rtc_init(void);
void adc_init(void);
void ldr_threshold_calibrate(void);
void tcb_init(void);

// Global variable to store ADC result
volatile uint16_t adcValue;

// Unsigned integer to indicate if the propeller is in front of the LDR, and  
// this makes sure, the propeller is only counted once
volatile uint8_t isPropOn;

// Checking whether should update LCD (=1), calculate rotations (=2) from ADC or  
// measure potentiometer (=3) from ADC
volatile uint8_t lcdUpdate;

// Value for storing the voltage level used as boundary indicating whether there
// is a propellor in front of the LDR or not
uint16_t voltThreshold;

// Indicates whether we are going to measure LDR or potentiometer, every 100th
// time we need to read potentiometer, otherwise LDR
volatile uint8_t potentRead;

// Function for sending text to computer terminal/putty
static void usart0_send_char(char c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
    {
        ;
    }
    USART0.TXDATAL = c;
}

static int usart0_print_char(char c, FILE *stream)
{ 
    usart0_send_char(c);
    return 0; 
}

static FILE USART_stream =  \
    FDEV_SETUP_STREAM(usart0_print_char, NULL, _FDEV_SETUP_WRITE);

// Intitializing connection to computer terminal/putty
static void usart0_init(void)
{
    PORTA.DIR |= PIN0_bm;
    
    // Value 9600 is provided by Microchip
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600); 
    
    USART0.CTRLB |= USART_TXEN_bm;  
    
    stdout = &USART_stream;
}

// Configuring RTC
void rtc_init(void)
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
    // RTC runs on STDBY sleep mode
    RTC.CTRLA |=  RTC_RUNSTDBY_bm;
    // Enable periodic interrupt
    RTC.PITINTCTRL = RTC_PI_bm;
    // Selecting number of RTC clock cycles (32768) because we want interrupt
    // once a second and enable periodic interrupt timer
    RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc | RTC_PITEN_bm;
}

// Configuring ADC
void adc_init(void)
{
    // Set port E pin 0 as input for LDR and port F pin 4 input 
    // for potentiometer
    PORTE.DIRCLR = PIN0_bm;
    PORTF.DIRCLR = PIN4_bm;
    
    // Disable input buffers, pull-up resistor disabled by default 
    PORTE.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    PORTF.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    
    // Voltage reference is 1,5V at first when starting measurements of LDR
    // (internal reference voltage already defined in main) and prescaler of 16
    ADC0.CTRLC |= ADC_REFSEL_INTREF_gc | ADC_PRESC_DIV16_gc;
    
    // Resolution 10 bits, enable ADC and running also on stdy sleep mode
    ADC0.CTRLA |= ADC_RESSEL_10BIT_gc | ADC_ENABLE_bm | ADC_RUNSTBY_bm;
    
    // Selecting AN8 (PE0) to be connected to ADC (LDR will be connected first)
    ADC0.MUXPOS =  ADC_MUXPOS_AIN8_gc;
    
    // Freerun mode enabled (next conversion starts automatically)
    ADC0.CTRLA |= ADC_FREERUN_bm;
    
    // Enable interrupts
    ADC0.INTCTRL |= ADC_RESRDY_bm;
}

// Initialize TCB0 in PWM mode and output pin (to motor) for PWM signals
void tcb_init(void)
{
    // Configure correct pin as output to DC motor, and first as high
    VPORTA.DIR |= PIN2_bm;
    VPORTA.OUT |= PIN2_bm;

    // Duty cycle 50 % set at first (CCMPH = 0x80), PWM signal period 1 sec 
    // (CCMPL = 0xFF)
    TCB0.CCMP = 0x80FF;
    // Enable TCB, and divide clock with 2, also setting run in stdby sleep mode
    TCB0.CTRLA |= TCB_ENABLE_bm | TCB_CLKSEL_CLKDIV2_gc | TCB_RUNSTDBY_bm;
    // Enable output signal of Compare/Capture, and TCB configured in 
    // 8-bit PWM mode
    TCB0.CTRLB |= TCB_CCMPEN_bm | TCB_CNTMODE_PWM8_gc;
    
}

// Calibrates the threshold for light vs dark, depending on current lighting
void ldr_threshold_calibrate(void)
{
    // Three temporary values of calibration will be set into an array
    int calibTab [4] = {0};
    
    // Informing user via Putty
    printf("Calibrating lighting, one moment...\r\n");
    
    // Measuring current light conditions in LDR's environment 10000 times
    for (int i = 0; i<=9999; i++)
    {
        // Waiting adc result to be ready
        while (!(ADC0.INTFLAGS & ADC_RESRDY_bm))
        {
            ;
        }
        // Setting every 333th measured value to the array
        if ((i % 3333) == 0)
        {
            // Capturing current ADC value
            calibTab[i/3333 - 1] = ADC0.RES;
            // Prints measured value to Putty
            printf("%d\r\n", calibTab[i/3333 - 1]);
        }
        // Allowing next ADC conversion begin
        ADC0.INTFLAGS = ADC_RESRDY_bm;
    }

    // Set voltThreshold a little bit above the average of the calibration 
    // values with defined macro
    voltThreshold = (calibTab[0]+calibTab[1]+calibTab[2])/3 + MIN_VOLT_DIFF;
    // Informing user when calibration is complete and what is the boundary 
    // value
    printf("Calibration complete!\r\n");
    printf("Threshold voltage: %i\r\n", voltThreshold);
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
    // Setting the value ADC measured
    adcValue = ADC0.RES;
    
    // Checking whether measured value is from potentiometer, time to update the
    // propellor spinning speed
    if (potentRead == 100)
    {
        // Setting indicator to point out the need for updating spinning
        lcdUpdate = 3;
    }
    
    // Measure was from LDR
    else
    {
        // Based on adcValue: propeller is in front of LDR, threshold was 
        // calibrated in the beginning
        if (adcValue>voltThreshold)
        {
            // Propellor was not in front of LDR last time ADC measured value
            if(isPropOn == 0)
            {
                // Setting propellor state to be in front of LDR
                isPropOn=1;
            }
        }
        // Based on adcValue: propellor is not in front of LDR, LWR_THRESH is 
        // subtracted from boundary value to make sure light conditions are not 
        // unstable
        else if (adcValue<(voltThreshold - LWR_THRESH))
        {
            // Setting propellor state to indicate it is not in front of LDR
            isPropOn = 0;
        }
        // Setting indicator to point out the need for LDR updating
        lcdUpdate = 2;
    }
}

int main(void) 
{
    // Variable to store the rpm calculated from rotations, initialized as 0
    uint16_t rpm = 0;
    
    // Indicates potentiometer value user has set for spinning speed,
    // initialized as 0
    uint8_t userVoltage = 0;
    
    // Variable to count rotations, initialized as 0
    volatile uint16_t rotations = 0;
    
    // In the beginning the value of global variables is set to 0
    adcValue = 0;
    isPropOn = 0;
    lcdUpdate = 0;
    potentRead = 0;
    
    // Setting internal reference voltage to 1.5V
    VREF.CTRLA = VREF_ADC0REFSEL_1V5_gc;
    
    // Initialize output to Putty
    usart0_init();
    // Initialize LCD display
    lcd_init();
    // Initialize ADC and its input pin
    adc_init();
    // Initialize RTC
    rtc_init();
    // Initialize TCB to 8-bit PWM mode (and its output pin)
    tcb_init();
   
    // Setting IDLE as sleep mode
    set_sleep_mode(SLPCTRL_SMODE_IDLE_gc);
    
    // Start ADC conversion
    ADC0.COMMAND = ADC_STCONV_bm;
    
    // Calibrates current lightning without anything in front of the LDR
    ldr_threshold_calibrate();
    
    // Enable global interrupts
    sei();
    
    while(1)
    {
        // Entering sleep mode every time after wake up (interrupt)
        sleep_mode();
        
        // Checking whether the interrupt was about LCD updating
        if (lcdUpdate == 1)
        {
            // Disable interrupts for segment updating, rpm must be calculated
            // and set to LCD without interrupts because it might lead to data
            // corruption or unstable state
            cli();
            // Rpm calculation from rotations 
            // (*60 because observation interval is 1,0s)
            rpm = rotations*60;
            // Updating display to calculated RPM
            lcd_update(rpm);
            // Resetting rotations for next round
            rotations=0;
            // Enable interrupts after LCD has been updated correctly
            sei();
        }
        
        // Checking whether the interrupt was from result-ready ADC (LDR)
        else if (lcdUpdate == 2)
        {
            // When the propeller is in front of the LDR, the rotations
            // value is updated
            if (isPropOn == 1)
            {
                // Disable global interrupts to update rotations without data
                // corruption, making process atomic
                cli();
                // Rotations value added with one
                rotations++;
                // isPropOn written to two to ensure LDR does not count same 
                // rotation twice
                isPropOn = 2;
                // Enable global interrupts
                sei();
            }
            // Checking if next conversion shall be taken from potentiometer,
            // next LDR conversion is 100th time
            if (potentRead == 99)
            {
                // Disable global interrupts to ensure proper ADC configuration
                // without corruption
                cli();
                // Switching ADC channel to potentiometer
                ADC0.MUXPOS = ADC_MUXPOS_AIN14_gc;
                // Settling time for ADC to switch the channel
                _delay_us(10);
                // Setting new reference voltage (VDD)
                ADC0.CTRLC |= ADC_REFSEL_VDDREF_gc;
                // Settling time for ADC to switch reference voltage
                _delay_us(10);
                // Increasing potentRead to 100
                potentRead++;
                // Enable global interrupts
                sei();
            }
            
            // Neither rotations nor ADC channel shall be updated
            else
            {
                // Disable interrupts to ensure atomic operation for RMW
                cli();
                // Increase potentRead value
                potentRead++;
                // Enable global interrupts
                sei();
            }
        }
        
        // Checking whether interrupt was from result-ready ADC (potentiometer) 
        else if (lcdUpdate == 3)
        {
            // Disable interrupts to ensure proper spinning update and ADC 
            // configuration without corruptions
            cli();
            // Potentiometer value changed from 10 bits to 8
            userVoltage = adcValue>>2;
            // Updates motor speed
            spin_update(userVoltage);
            // Switching ADC channel back to LDR
            ADC0.MUXPOS =  ADC_MUXPOS_AIN8_gc;
            // Settling time for ADC to switch the channel
            _delay_us(10);
            // Setting reference voltage back to 1.5 V for LDR
            ADC0.CTRLC |= ADC_REFSEL_INTREF_gc;
            // Settling time for ADC to switch reference voltage
            _delay_us(10);
            // Reset potentRead counter
            potentRead = 0;
            // Enable global interrupts
            sei();
        }
    }
    return 0;
}
