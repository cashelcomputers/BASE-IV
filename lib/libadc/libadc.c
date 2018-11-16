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

#include <avr/io.h>
#include <avr/interrupt.h>
#include "libadc.h"
#include "globals.h"

//volatile uint16_t disp_select_value;
//volatile uint16_t func_select_value;
volatile uint8_t adc_channel = DISP_SEL_CH;

void adc_init(void)
{
    /*
    This function configures the ADC for use.
    */

    // set ADC ref = Vcc
    ADMUX |= (1 << REFS0);

    // enable ADC, start conversion, set prescalar
    ADCSRA |= (1 << ADSC) | (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t read_adc(uint8_t channel)
{
    ADMUX = (ADMUX & 0xF8) | channel;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}
/*ISR(ADC_vect)
{
    uint8_t admux = ADMUX;
    if (adc_channel == DISP_SEL_CH)
    {
        disp_select_value = ADC;
        admux ^= 1;
        adc_channel = FUNC_SEL_CH;
        PORTD ^= (1 << PD7);
        
        

    }
    else
    {
        func_select_value = ADC;
        admux ^= 1;
        adc_channel = DISP_SEL_CH;
        PORTD ^= (1 << PD5);
    }
    ADMUX = admux;
    ADCSRA |= (1 << ADSC);

    
    
}
*/