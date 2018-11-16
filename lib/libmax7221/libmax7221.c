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
#include <string.h>
#include <stdlib.h>
#include "libmax7221.h"
#include "globals.h"

void max7221_init(void)
{
    /*
    This function configures the MAX7221 display driver IC for use.
    */

    // set scan limit to all 8 digits
    max7221_write(SCAN_LIMIT, 0x07);

    // set no decode mode on all bits
    max7221_write(DECODE_MODE, 0x00);

    // set intensity to FULL POWER
    max7221_set_intensity(15);

    // kick it in the guts
    max7221_powerup();

    // ensure display is blank
    max7221_blank_display();

}

void max7221_write(uint8_t address, uint8_t data)
{
    SPCR &= ~(1 << CPOL);           // set clock polarity
    SPI_PORT &= ~(1 << MAX7221_CS); // assert MAX7221 chip select
    //_delay_us(5);
    SPDR = (address & 0x0F);        // only send lower nibble of address
    while(!(SPSR & (1<<SPIF)));     // wait for send completion
    SPDR = data;
    while(!(SPSR & (1<<SPIF)));
    SPI_PORT |= (1 << MAX7221_CS);
    SPCR |= (1 << CPOL);            // reset clock polarity


}

void display_test(uint8_t mode)
{
    if (mode)
    {
        max7221_write(TEST, 0x01);
    }
    else
    {
        max7221_write(TEST, 0x00);
    }
}

void max7221_set_intensity(uint8_t intensity_value)
{
    if (intensity_value > 15) intensity_value = 15;
    max7221_write(INTENSITY, intensity_value);
}

uint8_t max7221_putc(uint8_t digit, uint8_t data)
{
    //if ((digit < 1) || (digit > 8)) return -1;
    uint8_t out_char;
    switch (data)
    {
        case '0':
            out_char = CHAR_0;
            break;
        case '1':
            out_char = CHAR_1;
            break;
        case '2':
            out_char = CHAR_2;
            break;
        case '3':
            out_char = CHAR_3;
            break;
        case '4':
            out_char = CHAR_4;
            break;
        case '5':
            out_char = CHAR_5;
            break;
        case '6':
            out_char = CHAR_6;
            break;
        case '7':
            out_char = CHAR_7;
            break;
        case '8':
            out_char = CHAR_8;
            break;
        case '9':
            out_char = CHAR_9;
            break;
        case ' ':
            out_char = CHAR_BLANK;
            break;
        case 'A':
            out_char = CHAR_A;
            break;
        case 'B':
            out_char = CHAR_B;
            break;
        case 'C':
            out_char = CHAR_C;
            break;
        case 'D':
            out_char = CHAR_D;
            break;
        case 'E':
            out_char = CHAR_E;
            break;
        case 'F':
            out_char = CHAR_F;
            break;
        case 'H':
            out_char = CHAR_H;
            break;
        case 'I':
            out_char = CHAR_I;
            break;
        case 'J':
            out_char = CHAR_J;
            break;
        case 'L':
            out_char = CHAR_L;
            break;
        case 'P':
            out_char = CHAR_P;
            break;
        case 'S':
            out_char = CHAR_S;
            break;
        case 'T':
            out_char = CHAR_T;
            break;
        case 'U':
            out_char = CHAR_U;
            break;
        case 'Y':
            out_char = CHAR_Y;
            break;
        case '-':
            out_char = CHAR_DASH;
            break;
    }
    
    max7221_write(digit, out_char);
    return 0;
}

uint8_t max7221_puts(uint8_t data[])
{
    
    uint8_t i;
    //uint8_t data_length = sizeof(data) / sizeof(int);
    for (i = 1; i < 8; i++)
    {
        max7221_putc(i, data[i - 1]);
    }
    return 0;
}

void max7221_powerup(void)
{
    max7221_write(SHUTDOWN, 0x01);
}

void max7221_powerdown(void)
{
    max7221_write(SHUTDOWN, 0x00);
}

void max7221_blank_display(void)
{
    
    // blanks the entire display.

    for (uint8_t i = 1; i < 9; i++)
    {
        max7221_write(i, CHAR_BLANK);
    }
}

void max7221_display_int(uint32_t value)
{
    uint8_t int_length = floor(log10(abs(value))) + 1;              // determine number of digits
    char str[int_length];                                       
    //uint8_t blank_spaces = (8 - int_length);                          // calculate how many blank digits we need to pad with
    //char blank_str[] = {blank_spaces * ' '};

    
    ltoa(value, str, 10);
    strrev(str);
    //strcat(str, blank_str);
    max7221_blank_display();
    for (uint8_t i = 0; i < strlen(str); i++)
    {
        max7221_putc((i + 1), str[i]);
    }
}

void max7221_splash(void)
{
    max7221_putc(D7, ' ');
    max7221_putc(D6, 'B');
    max7221_putc(D5, 'A');
    max7221_putc(D4, 'S');
    max7221_putc(D3, 'E');
    max7221_putc(D2, '-');
    max7221_putc(D1, '4');
    max7221_putc(D0, ' ');
    
    /*uint8_t data[] = {CHAR_BLANK, CHAR_B, CHAR_A};
    max7221_puts(data);
    */
}