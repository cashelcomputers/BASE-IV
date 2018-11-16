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


void _ad9833_send_16(uint16_t data);
void AD9833_set_freq(uint32_t new_freq, uint8_t freq_reg);
void AD9833_set_waveform(uint8_t waveform);
void AD9833_set_ctrl_reg(uint16_t data);
void AD9833_set_phase(uint16_t phase);
void AD9833_reset(uint8_t reset);
void AD9833_sleep(uint8_t sleep_mode);