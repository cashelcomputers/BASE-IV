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
#include <math.h>
#include "libad9833.h"
#include "globals.h"

void _ad9833_send_16(uint16_t data)
{
    /*
    This function sends data to the AD9833 over the SPI bus. External functions
    should not call this, instead use appropriate helper functions.
    */

    if (data > 4194303)                 // 2^16 - 1
    {
        // data too big
        return;
    }

    uint8_t msb = (data >> 8);
    uint8_t lsb = (data & 0xFF);

    AD9833_PORT &= ~(1 << AD9833_CS);   // assert AD9833 chip select

    // send 2 bytes
    SPDR = msb;
    while(!(SPSR & (1<<SPIF)));
    SPDR = lsb;
    while(!(SPSR & (1<<SPIF)));


    AD9833_PORT |= (1 << AD9833_CS);
}

void AD9833_set_freq(uint32_t new_freq, uint8_t freq_reg)
{
    /*
    This function sets the desired frequency into the appropriate
    AD9833 frequency register.
    */

    uint32_t freq;
    uint16_t msb;
    uint16_t lsb;

    // test to see if requested frequency is within bounds
    if (new_freq < 1)
    {
        new_freq = 1;
    }
    else if (new_freq > MAX_FREQ)
    {
        new_freq = MAX_FREQ;
    }

    freq = (new_freq * pow(2, 28)) / AD9833_CLOCK;
    
    // swap frequency registers if we are sweeping
    if (freq_reg)
    {
        msb = (int)(freq >> 14) | AD9833_FREQ1_REG;
        lsb = (int)(freq & 0x3FFF) | AD9833_FREQ1_REG;
    }
    else
    {
        msb = (int)(freq >> 14) | AD9833_FREQ0_REG;
        lsb = (int)(freq & 0x3FFF) | AD9833_FREQ0_REG;
    }

    // send the data over SPI
    _ad9833_send_16(lsb);
    _ad9833_send_16(msb);
}

void AD9833_set_waveform(uint8_t waveform)
{
    /*
    This function sets the desired output waveform (sine, triangle or square).
    */

    uint16_t ctrl_reg_value;

    if (waveform == FUNC_SINE)                          // sine
    {
        ctrl_reg_value = 0x00;
    }
    else if (waveform == FUNC_TRI)                      // triangle
    {
        ctrl_reg_value = (1 << MODE);
    }
    else if (waveform == FUNC_SQUARE)                   // square
    {
        ctrl_reg_value = (1 << OPBITEN) | (1 << DIV2);
    }

    AD9833_set_ctrl_reg(ctrl_reg_value);
    
}
void AD9833_set_ctrl_reg(uint16_t data)
{
    /*
    This function sets the AD9833 control register correctly. It should be used for all
    writes to control register.
    */

    uint16_t new_value = data | (1 << B28);
    _ad9833_send_16(new_value);
}

void AD9833_set_phase(uint16_t phase)
{
    /*
    This function sets the phase of the output. Currently, in 2pi/4096
    */

    // bounds checks
    if (phase < 0)
    {
        phase = 0;
    }
    else if (phase > MAX_PHASE)
    {
        phase = MAX_PHASE;
    }
    
    phase |= AD9833_PHASE0_REG;
    _ad9833_send_16(phase);
}

void AD9833_reset(uint8_t reset)
{
    /*
    This function configures the AD9833 reset.
    */

    if (reset == 0)             // disable reset
    {
        AD9833_set_ctrl_reg(0x00);
    }
    else if (reset == 1)        // enable reset
    {
        AD9833_set_ctrl_reg(1 << AD9833_RESET);
    }
}
void AD9833_sleep(uint8_t sleep_mode)
{
    /*
    This function enables or disables AD9833 sleep modes.
    */

    if (sleep_mode == 0)            // normal mode
    {
        AD9833_set_ctrl_reg(0x00);
    }
    else if (sleep_mode == 1)       // sleep mode
    {
        AD9833_set_ctrl_reg((1 << SLEEP1) | (1 << SLEEP12));
    }
}