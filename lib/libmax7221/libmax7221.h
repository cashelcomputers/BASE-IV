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

// prototypes

void max7221_init(void);
void max7221_write(uint8_t address, uint8_t data);
void display_test(uint8_t mode);
void max7221_set_intensity(uint8_t intensity_value);
uint8_t max7221_putc(uint8_t digit, uint8_t data);
uint8_t max7221_puts(uint8_t data[]);
void max7221_powerup(void);
void max7221_powerdown(void);
void max7221_blank_display(void);
void max7221_splash(void);
void max7221_display_int(uint32_t value);