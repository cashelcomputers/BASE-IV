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

#ifndef LIBBASE4_H
#define LIBBASE4_H

// the following sets the number of steps for each sweep time interval

#define SWEEP_STEPS_50MS        500UL
#define SWEEP_STEPS_100MS       1000UL
#define SWEEP_STEPS_250MS       2500UL
#define SWEEP_STEPS_500MS       5000UL
#define SWEEP_STEPS_1000MS      10000UL
#define SWEEP_STEPS_2000MS      20000UL

#define SWEEP_50MS              0
#define SWEEP_100MS             1
#define SWEEP_250MS             2
#define SWEEP_500MS             3
#define SWEEP_1000MS            4
#define SWEEP_2000MS            5

extern uint32_t frequency;
extern uint16_t phase;
extern uint8_t func_select_state;
extern uint8_t disp_select_state;
extern volatile uint8_t rot_enc_cw;
extern volatile uint8_t rot_enc_ccw;
extern uint32_t sweep_start_freq;
extern uint32_t sweep_stop_freq;
extern uint32_t sweep_interval;              // from 0 to 9 (the index to the array of possible sweep time intervals)
uint8_t selected_digit;              // from 1 to 7
extern volatile uint8_t tick_flag;
extern volatile uint8_t rot_enc_pb;
//uint32_t selected_digit_multiplier[8];

// prototypes

void spi_init(void);
void initial_setup(void);

void set_frequency(void);

void check_func_sel(void);
void check_disp_sel(void);

void init_sweep_timer(void);
void init_tick_timer(void);

void start_sweep();
void stop_sweep();

void toggle_debug_pin(void);

void update_display(void);
void set_phase(uint16_t new_phase);
void init_debug_pin(void);

uint8_t read_func_sel(void);
uint8_t read_disp_sel(void);
void set_initial_func_sel_state(void);
void update_display(void);
void check_rotary_encoder(void);

void calculate_sweep_delta(void);
void sweep_increment(void);

void check_rot_enc_pb(void);
void check_digit_flash(void);

#endif