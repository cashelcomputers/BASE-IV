/* 
 * This file is part of the BASE-4 distribution (website).
 * Copyright (c) 2018 Tim Buchanan.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <math.h>
#include <avr/interrupt.h>
#include "libbase4.h"
#include "globals.h"
#include "libad9833.h"
#include "libadc.h"
#include "libmax7221.h"
#include "librotaryencoder.h"

uint16_t _control_reg;
volatile uint8_t rot_enc_dir;
volatile uint8_t rot_enc_pb = 0;
uint32_t frequency = DEFAULT_FREQ;
uint16_t phase = DEFAULT_PHASE;
uint8_t func_select_state = FUNC_SINE;
uint8_t disp_select_state = DISP_FREQ;
uint16_t sweep_interval_counter;
uint8_t selected_digit = 1;
uint32_t sweep_start_freq = SWEEP_START_DEFAULT;
uint32_t sweep_stop_freq = SWEEP_STOP_DEFAULT;
uint32_t sweep_interval = SWEEP_TIME_DEFAULT;
uint32_t sweep_delta;
uint32_t saved_frequency;
volatile uint16_t disp_select_value;
volatile uint16_t func_select_value;
volatile uint16_t adc_reading;
uint8_t is_sweep_started = 0;


uint16_t sweep_times[] = {50, 100, 250, 500, 1000, 2000};
uint32_t selected_digit_multiplier[] = {1, 10, 100, 1000, 10000, 100000, 1000000};


volatile uint8_t rot_enc_cw = 0;
volatile uint8_t rot_enc_ccw = 0;

volatile uint8_t tick_flag = 0;
uint8_t freq_reg_select = 0;

uint8_t digit_flash_counter = 0;            // counts how many times we have flashed the digit
uint16_t digit_flash_tick_counter = 0;      // counts the system ticks
uint8_t is_digit_flashing = 0;

void initial_setup(void)
{
    /*
    This function sets initial frequency and phase, and updates the display.
    Only used on startup.
    */

    AD9833_set_freq(frequency, 0);
    AD9833_set_phase(phase);
    update_display();
}

void check_digit_flash(void)
{
    /*
    This function checks if we need to flash a digit. If so, performs the blink.
    */

    if (is_digit_flashing)
            {
                digit_flash_tick_counter += 1;
            }

            if (digit_flash_tick_counter == 25)     // approx 0.75 seconds, blank digit
            { 
                max7221_putc(selected_digit, ' ');
            }
            if (digit_flash_tick_counter == 34)     // turn digit back on
            {
                update_display();
                digit_flash_counter += 1;
                digit_flash_tick_counter = 0;
            }
            if (digit_flash_counter > 4)            // stop flashing
            {
                is_digit_flashing = 0;
                digit_flash_counter = 0;
                digit_flash_tick_counter = 0;
            }
}

void spi_init(void)
{
    /*
    This function initialises the SPI bus.
    */

    // set output pins. NOTE: SS Must be set as output even though not used!!!
    SPI_DDR |= (1 << SPIE) | (1 << SPI_SCK) | (1 << SPI_MOSI) | (1 << MAX7221_CS) | (1 << SPI_SS);

    // set outputs
    SPI_PORT &= ~(1 << SPI_SS);
    SPI_PORT |= (1 << MAX7221_CS) | (1 << SPI_SCK);
    AD9833_DDR |= (1 << AD9833_CS);
    AD9833_PORT |= (1 << AD9833_CS);

    // enable SPI, master mode CPOL = 1, frequency = 8 MHz
    SPCR |= (1 << SPE) | (1 << MSTR) | (1 << CPOL);
    SPSR |= (1 << SPI2X);
}

void check_rot_enc_pb(void)
{
    /*
    This function checks if the rotary encoder pushbutton has been pressed. This will 
    be triggered from interrupt.
    */

    uint8_t current_digit = selected_digit;

    if (rot_enc_pb)
    {
        current_digit = current_digit + 1;

        if (current_digit > 7) 
        {

            current_digit = 1;
        }
        
        selected_digit = current_digit;
        rot_enc_pb = 0;
        is_digit_flashing = 1;
    }
}

void set_frequency(void)
{
    /*
    This helper function sets the frequency and updates the display. This is
    the preferred function to set frequency.
    */
    
    // bounds checks
    if ((func_select_state == FUNC_SINE) && (frequency > MAX_FREQ))
    {
        frequency = MAX_FREQ;
    }
    else if ((func_select_state == FUNC_TRI) && (frequency > MAX_TRI_SQ_FREQ))
    {
        frequency = MAX_TRI_SQ_FREQ;
    }
    else if ((func_select_state == FUNC_SQUARE) && (frequency > MAX_TRI_SQ_FREQ))
    {
        frequency = MAX_TRI_SQ_FREQ;
    }
    else if (frequency < 1)
    {
        frequency = 1;
    }

    AD9833_set_freq(frequency, 0);
    max7221_display_int(frequency);
}

uint8_t read_disp_sel(void)
{
    /*
    This function reads the display select control.
    */
    
    // read the ADC
    uint16_t adc_reading = read_adc(DISP_SEL_CH);

    if (adc_reading > 600)
    {
        return DISP_FREQ;      // display fixed frequency
    }
    else if (adc_reading < 550 && adc_reading > 480)
    {

        return DISP_PHASE;      // display phase
    }
    else if (adc_reading < 370 && adc_reading > 300)
    {
        return DISP_SWEEP_START;      // display sweep start frequency
    }
    else if (adc_reading < 280 && adc_reading > 220)
    {
        return DISP_SWEEP_STOP;      // display sweep end frequency
    }
    else if (adc_reading < 219 && adc_reading > 185)
    {
        return DISP_SWEEP_TIME;      // display sweep time
    }
    else if (adc_reading < 184)
    {
        return 5;      // not currently used
    }

    return 6;               // error if we return from here
}

void check_disp_sel(void)
{
    /*
    This function checks if the display select control has changed. If so, 
    update display.
    */
    
    uint8_t new_disp_sel_state = read_disp_sel();
    
    if (new_disp_sel_state != disp_select_state)
    {
        disp_select_state = new_disp_sel_state;
        update_display();
    }
}

uint8_t read_func_sel(void)
{
    /*
    This function reads the function select control.
    */
    
    // read the ADC
    uint16_t adc_reading = read_adc(FUNC_SEL_CH);

    if (adc_reading > 600)
    {
        return FUNC_SINE;                               // sine wave selected
    }
    else if (adc_reading < 550 && adc_reading > 480)
    {
        
        return FUNC_TRI;                                // triangle wave selected
    }
    else if (adc_reading < 370 && adc_reading > 300)
    {
        return FUNC_SQUARE;                             // square wave selected
    }
    else if (adc_reading < 280 && adc_reading > 220)
    {
        return FUNC_LIN_SWEEP;                          // linear sweep selected
    }
    else if (adc_reading < 219 && adc_reading > 185)
    {
        return 4;                                       // log sweep selected (NOT IMPLEMENTED)
    }
    else if (adc_reading < 184)
    {
        return 5;                                       // not currently used
    }
    return 6;                                           // would be an error to return from here
}

void set_initial_func_sel_state(void)
{
    /*
    This function reads the function control on initial startup.
    */

    func_select_state = read_func_sel();
}
void set_initial_disp_sel_state(void)
{
    /*
    This function reads the display control on initial startup.
    */
   
    disp_select_state = read_disp_sel();
}

void check_func_sel(void)
{
    /*
    This function checks if the function select control has changed. If so,
    select new function.
    */

    uint8_t new_func_sel_state = read_func_sel();
    
    if (new_func_sel_state != func_select_state)
    {
        // if the selected function is non-sweep:
        if ((new_func_sel_state == FUNC_SINE) || (new_func_sel_state == FUNC_TRI) || (new_func_sel_state == FUNC_SQUARE))
        {
            AD9833_set_waveform(new_func_sel_state);

            // if sweep timer is running, stop it and restore frequency
            if (TCCR0B & (1 << CS01))
            {
                stop_sweep();
            }
        }
        else if (new_func_sel_state == FUNC_LIN_SWEEP)
        {
            calculate_sweep_delta();
            start_sweep();
        }
    }
    func_select_state = new_func_sel_state;
}

void init_sweep_timer(void)
{
    /*
    This function initialises the frequency sweep timer, TIMER0.
    */

    TCCR0A = (1 << WGM01);          // set CTC mode
    OCR0A = SWEEP_TIMER_OVF;        // set overflow value
    TCNT0 = 0x00;                   // ensure timer is reset to 0
    cli();
    TIMSK0 |= (1 << OCIE0A);        // enable compare match interrupt
    sei();
    

}

void start_sweep(void)
{
    /*
    This function starts the linear sweep.
    */

    AD9833_set_waveform(FUNC_SINE);

    // save previous frequency
    saved_frequency = frequency;
    TCCR0B |= (1 << CS01);           // set clk/8 prescaler and start timer
    is_sweep_started = 1;
}

void stop_sweep(void)
{
    /*
    This function stops the linear sweep.
    */

    TCCR0B &= ~(1 << CS01);         // fin
    frequency = saved_frequency;    // restore last frequency
    AD9833_set_freq(frequency, 0);
    check_func_sel();
    check_disp_sel();
    TCNT0 = 0x00;
    is_sweep_started = 0;
}

void init_tick_timer(void)
{
    /*
    This function initialises the system tick timer, TIMER1 (16 bit counter).
    It should be called just before main loop is entered.
    */

    OCR1A = TICK_TIMER_OVF;                         // set overflow value for OC1A

    cli();
    TCNT1 = 0x00;                                   // ensure counter is reset
    TIMSK1 = (1 << OCIE1A);                         // enable compare match interrupt
    TCCR1B = (1 << WGM12) | (1 << CS12);            // set CTC mode and clk/256 prescaler, start the timer
    sei();
}

void toggle_debug_pin(void)
{
    /*
    This function was used for debugging.
    */

    PORTD ^= (1 << PD5);            // pin 5
}

void check_rotary_encoder(void)
{
    /*
    This function checks if the rotary encoder has moved. If so, update display.
    */

    int8_t delta;

    // check if rotary encoder has moved
    if (rot_enc_cw)
        {
            delta = 1;
            rot_enc_cw = 0;
        }

    else if (rot_enc_ccw)
        {
            delta = -1;
            rot_enc_ccw = 0;
        }
    else
    {
        delta = 0;
    }
    
    // if rotary encoder has moved, update relevant variable and display
    if (delta)
    {
        if (disp_select_state == DISP_FREQ)
        {
            frequency += delta * selected_digit_multiplier[selected_digit - 1];
            set_frequency();
        }
        else if (disp_select_state == DISP_PHASE)
        {
            // phase can only have 4 digits, set to 4 if out of bounds
            if (selected_digit > 4) selected_digit = 4;

            phase += delta * selected_digit_multiplier[selected_digit - 1];
            set_phase(phase);
        }
        else if (disp_select_state == DISP_SWEEP_START)
        {
            sweep_start_freq += delta * selected_digit_multiplier[selected_digit - 1];
            max7221_display_int(sweep_start_freq);
        }
        else if (disp_select_state == DISP_SWEEP_STOP)
        {
            sweep_stop_freq += delta * selected_digit_multiplier[selected_digit - 1];
            max7221_display_int(sweep_stop_freq);
        }
        else if (disp_select_state == DISP_SWEEP_TIME)
        {
            // bounds checking
            int8_t new_sweep_interval = sweep_interval + delta;
            if (new_sweep_interval > SWEEP_2000MS)
            {
                sweep_interval = SWEEP_2000MS;
            }
            else if (new_sweep_interval < SWEEP_50MS)
            {
                sweep_interval = SWEEP_50MS;
            }
            else
            { 
                sweep_interval += delta;
            }

            update_display();
        }
    }
}

void set_phase(uint16_t new_phase)
{
    /*
    This helper function sets the AD9833 phase. This is the preferred
    function to call to set phase.
    */

    if (new_phase > MAX_PHASE) 
    {
        new_phase = MIN_PHASE;
        phase = MIN_PHASE;
    }
    else if (new_phase < MIN_PHASE)
    {
        new_phase = MAX_PHASE;
        phase = MAX_PHASE;
    }
    AD9833_set_phase(new_phase);
    max7221_display_int(new_phase);

}

void init_debug_pin(void)
{
    /*
    Used for debugging.
    */

    DDRD |= (1 << PD5);
    PORTD &= ~(1 << PD5);
}

void update_display(void)
{
    /*
    This function updates the display.
    */

    if (disp_select_state == DISP_FREQ)
    {
        max7221_display_int(frequency);
    }

    else if (disp_select_state == DISP_PHASE)
    {
        max7221_display_int(phase);
    }

    else if (disp_select_state == DISP_SWEEP_START)
    {
        max7221_display_int(sweep_start_freq);
    }

    else if (disp_select_state == DISP_SWEEP_STOP)
    {
        max7221_display_int(sweep_stop_freq);
    }

    else if (disp_select_state == DISP_SWEEP_TIME)
    {
        max7221_display_int(sweep_times[sweep_interval]);
    }
}

void calculate_sweep_delta(void)
{
    /*
    This function calculated the step increase for linear sweep.
    */

    uint32_t sweep_step;
    
    if (sweep_interval == SWEEP_50MS)
    {
        sweep_step = SWEEP_STEPS_50MS;
    }
    else if (sweep_interval == SWEEP_100MS)
    {
        sweep_step = SWEEP_STEPS_100MS;
    }
    else if (sweep_interval == SWEEP_250MS)
    {
        sweep_step = SWEEP_STEPS_250MS;
    }
    else if (sweep_interval == SWEEP_500MS)
    {
        sweep_step = SWEEP_STEPS_500MS;
    }
    else if (sweep_interval == SWEEP_1000MS)
    {
        sweep_step = SWEEP_STEPS_1000MS;
    }
    else if (sweep_interval == SWEEP_2000MS)
    {
        sweep_step = SWEEP_STEPS_2000MS;
    }
    sweep_delta = (sweep_stop_freq - sweep_start_freq) / sweep_step;
}

void sweep_increment(void)
{
    /*
    This function increments the linear sweep by the sweep delta.
    */

    frequency += sweep_delta;

    // check if we have hit the sweep stop frequency
    if (frequency > sweep_stop_freq)
    {
        frequency = sweep_start_freq;
    }

    AD9833_set_freq(frequency, freq_reg_select);

    if (freq_reg_select)
    {
        AD9833_set_ctrl_reg(FSELECT);
    }
    else
    {
        AD9833_set_ctrl_reg(0x00);
    }
}

ISR(INT0_vect)
{
    /*
    Rotary encoder pushbutton interrupt.
    */

    rot_enc_pb = 1;
    _delay_ms(20);
}

ISR(INT1_vect)
{
    /*
    rotary encoder interrupt.
    */

    if (ROT_ENC_PIN & (1 << ROT_ENC_D1))
    {
        rot_enc_cw = 1;
    }
    else
    {
        rot_enc_ccw = 1;
    }
    _delay_ms(20);
}

ISR(TIMER1_COMPA_vect)
{
    /*
    Main tick timer interrupt.
    */

    tick_flag = 1;
}

ISR(TIMER0_COMPA_vect)
{
    /*
    Sweep timer interrupt.
    */
   
    freq_reg_select ^= (1 << 0);
    sweep_increment_trig = 1;
}