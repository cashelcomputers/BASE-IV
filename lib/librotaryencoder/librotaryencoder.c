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
#include "librotaryencoder.h"
#include "globals.h"

void rotary_encoder_init(void)
{
    /*
    This function initialises the rotary encoder and it's pushbutton,
    as well as interrupts.
    */

    ROT_ENC_DDR &= ~(1 << ROT_ENC_D0) | (1 << ROT_ENC_D1);
    
    cli();
    EIMSK = (1 << INT1) | (1 << INT0);          // enable INT0 and INT1
    EICRA = (1 << ISC11) | (1 << ISC01);        // interrupt on falling edge
    sei();
}