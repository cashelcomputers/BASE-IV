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

// SPI defs
#define SPI_DDR                 DDRB
#define SPI_PORT                PORTB
#define SPI_MOSI                PB3
#define SPI_SCK                 PB5
#define SPI_SS                  PB2
#define MAX7221_CS              1
#define AD9833_DDR              DDRB
#define AD9833_PORT             PORTB
#define AD9833_CS               PB0                 // NOTE! Was changed to PB0, need to bodge PCB

// MAX7221 addresses
#define NOP                     0x00
#define D0                      0x01
#define D1                      0x02
#define D2                      0x03
#define D3                      0x04
#define D4                      0x05
#define D5                      0x06
#define D6                      0x07
#define D7                      0x08
#define DECODE_MODE             0x09
#define INTENSITY               0x0A
#define SCAN_LIMIT              0x0B
#define SHUTDOWN                0x0C
#define TEST                    0x0F

// 7 segment character definitions
#define CHAR_0                  0x7E
#define CHAR_1                  0x30
#define CHAR_2                  0x6D
#define CHAR_3                  0x79
#define CHAR_4                  0x33
#define CHAR_5                  0x5B
#define CHAR_6                  0x5F
#define CHAR_7                  0x70
#define CHAR_8                  0x7F
#define CHAR_9                  0x7B
#define CHAR_A                  0x77
#define CHAR_B                  0x1F
#define CHAR_C                  0x4E
#define CHAR_D                  0x3D
#define CHAR_E                  0x4F
#define CHAR_F                  0x47
#define CHAR_H                  0x37
#define CHAR_I                  0x06
#define CHAR_J                  0x3C
#define CHAR_L                  0x0E
#define CHAR_P                  0x67
#define CHAR_S                  0x5B
#define CHAR_T                  0x0F
#define CHAR_U                  0x3E
#define CHAR_Y                  0x3B
#define CHAR_BLANK              0x00
#define CHAR_DASH               0x01

// rotary encoder defines
#define ROT_ENC_DDR             DDRD
#define ROT_ENC_PORT            PORTD
#define ROT_ENC_PIN             PIND
#define ROT_ENC_D0              PD3         // INT1
#define ROT_ENC_D1              PD4
#define ROT_END_PB              PD2         // INT0

// switch defines
#define SW_DDR                  DDRC
#define SW_PORT                 PORTC
#define SW_PIN                  PINC
#define STANDBY_SW              PC0
#define OUTPUT_ENABLE_SW        PC1

// sweep defines. These are the number of steps for each time interval.




// program definitions
#define DEFAULT_FREQ            100000UL
#define DEFAULT_PHASE           50UL
#define MAX_FREQ                5000000UL
#define MAX_TRI_SQ_FREQ         500000UL
#define MAX_PHASE               4096UL
#define MIN_PHASE               0
#define SWEEP_START_DEFAULT     100000UL
#define SWEEP_STOP_DEFAULT      1000000UL
#define SWEEP_TIME_DEFAULT      SWEEP_1000MS
#define SWEEP_TIMER_OVF         200U         // changed from 64
#define TICK_TIMER_OVF          1875UL      // OC1A
#define ADC_SC_OVF              487UL       // OC1B

// ADC defines
#define FUNC_SEL_CH             7
#define DISP_SEL_CH             6

// AD9833 control word bit definitions
#define MODE                    1
#define DIV2                    3
#define OPBITEN                 5
#define SLEEP12                 6
#define SLEEP1                  7
#define AD9833_RESET            8
#define PSELECT                 10
#define FSELECT                 11
#define HLB                     12
#define B28                     13

//AD9833 address definitions
#define AD9833_ADDRESS_13       13
#define AD9833_ADDRESS_14       14
#define AD9833_ADDRESS_15       15

// misc AD9833 defines
#define AD9833_CLOCK            25000000UL

// output function definitions
#define FUNC_SINE               0
#define FUNC_TRI                1
#define FUNC_SQUARE             2
#define FUNC_LIN_SWEEP          3
#define FUNC_LOG_SWEEP          4

// display defines
#define DISP_FREQ               1
#define DISP_PHASE              2
#define DISP_SWEEP_START        3
#define DISP_SWEEP_STOP         4
#define DISP_SWEEP_TIME         5

// register addresses
#define AD9833_CTRL_REG         0x00
#define AD9833_FREQ0_REG        0x4000UL
#define AD9833_FREQ1_REG        0x8000UL
#define AD9833_PHASE0_REG       0xC000UL
#define AD9833_PHASE1_REG       0xE000UL


extern volatile uint16_t func_select_value;
extern volatile uint16_t disp_select_value;
extern volatile uint8_t sweep_increment_trig;
extern uint8_t is_sweep_started;
