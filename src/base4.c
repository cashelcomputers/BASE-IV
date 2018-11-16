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

/************************************************************************
* FILENAME :        base4.c             
*
* DESCRIPTION :
*       The main file for the project. No functions are defined here,
*       this file only calls other functions. 
*
* PUBLIC FUNCTIONS :
*       int     main()
*
* NOTES :
*       These functions are a part of the BASE-4 project
*
* AUTHOR :    Tim Buchanan
*
* CHANGES :   All changes are tracked using Git.
* 
* PIN DEFINITIONS (Arduino Pro Mini pin names in brackets):
* ADC6 (A6):            Display select sense
* ADC7 (A7):            Function select sense
* PB3 (11):             SPI MOSI (note, PCB was designed to connect to PB4 (12), will
*                       need to be bodged. Thanks RobotDyn for the incorrect
*                       documentation!)
* PB5 (13):             SPI SCK
* PC0 (A0):             Standby switch input (DELETED)
* PC1 (A1):             Output enable switch
* PD3 (3/INT1):         Rotary encoder D0 input
* PD4 (4):              Rotary encoder D1 input
* PD2 (2/INT0):         Rotary encoder pushbutton
* PB1 (9):              MAX7221 chip select (SPI)
* PB0 (8):              AD9833 chip select (SPI) BODGE
*
* TIMERS:
* TIMER0:               Sweep timer
* TIMER1:               System tick timer (30ms) (output compare A)
*                       ADC start conversion (output compare B)
* 
************************************************************************/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "libbase4.h"
#include "base4.h"
#include "globals.h"
#include "libad9833.h"
#include "libadc.h"
#include "libmax7221.h"
#include "librotaryencoder.h"

volatile uint8_t tick_flag;
volatile uint8_t sweep_increment_trig = 0;

// digit flash variables



int main()
{
    uint8_t is_ad9833_asleep = 0;           // true if AD9833 asleep, false otherwise

    // pause to allow hardware to reset
    _delay_ms(500);

    // init hardware

    //init_debug_pin();
    spi_init();
    max7221_init();
    adc_init();
    rotary_encoder_init();

    // init sweep timer, don't start it yet
    init_sweep_timer();

    // read starting state of front panel
    set_initial_func_sel_state();

    // reset AD9833
    AD9833_reset(0);

    // display test and splash screen
    max7221_blank_display();
    display_test(1);
    _delay_ms(1000);
    display_test(0);
    max7221_splash();
    _delay_ms(3000);

    // set initial frequency and phase
    initial_setup();
    
    // init and start the tick timer (30ms)
    init_tick_timer();

    while (1)
    {
        
        

        if (sweep_increment_trig)
        {
            sweep_increment();
            sweep_increment_trig = 0;
        }
        

    
        if (tick_flag)
        {
            check_func_sel();

            // if we are sweeping, lock out display select and rotary encoder and output enable
            if (!(is_sweep_started))
            {
                check_disp_sel();
                check_rotary_encoder();
                check_rot_enc_pb();
                
                if (!(SW_PIN & (1 << OUTPUT_ENABLE_SW)) && !(is_ad9833_asleep))
                {
                    AD9833_sleep(1);
                    is_ad9833_asleep = 1;

                }
                else if (SW_PIN & (1 << OUTPUT_ENABLE_SW) && is_ad9833_asleep)
                {
                    AD9833_sleep(0);
                    AD9833_reset(0);
                    check_func_sel();
                    is_ad9833_asleep = 0;
                }
                
            }

            // digit flash 
            check_digit_flash();
            /*
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
            */

            
            
            
            tick_flag = 0;
            
        }
    }
}