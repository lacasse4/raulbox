#ifndef __RAULBOX_PIC16F1718_H
#define __RAULBOX_PIC16F1718_H

#define SET                   1                                                 //Bit setting macro
#define CLR                   0                                                 //Bit clearing macro
#define _BAUDRATE             38400                                             //Baudrate speed macro
#define _TMR1H                 0xF0                                             //Value for timer1 TMR1H register
#define _TMR1L                 0x60                                             //Value for timer1 TMR1L register
#define _WAKEBYTE              0x00
#define _EWDATARANGE           0x65
#define _MASKADDREW1           0x00                                             //Ebow1 masked address macro
#define _MASKADDREW2           0x10                                             //Ebow2 masked address macro
#define _MASKADDREW3           0x20                                             //Ebow3 masked address macro
#define _MASKADDREW4           0x30                                             //Ebow4 masked address macro
#define _MASKADDREW5           0x40                                             //Ebow5 masked address macro
#define _MASKADDREW6           0x50                                             //Ebow6 masked address macro
#define _MASKADDREW7           0x60                                             //Ebow7 masked address macro
#define _RESETCMD              0x70
#define _INDTCYCCMD            0xFA
#define _GRPTCYCCMD            0xFB
#define _ASLPEN                0x80                                             //Autosleep enable macro
#define _ASLPDIS               0x81                                             //Autosleep disable macro
#define _ASLP15M               0x82
#define _ASLP20M               0x83
#define _ASLP25M               0x84
#define _ASLP30M               0x85
#define _ASLP35M               0x86
#define _ASLP40M               0x87
#define _ASLP45M               0x88
#define _ASLP50M               0x89
#define _ASLP55M               0x8A
#define _ASLP60M               0x8B
#define _INSTANTSLP            0xFF

void setup(void);                                                               //Prototype for mcu setup
void main(void);                                                                //Prototype for main loop
void get_command(unsigned char cmd);
void set_ebow(unsigned char ewdata);                                            //Prototype for ebow voltage setting
void asleep_config(unsigned char aslp_dat);
void rst_ebow(unsigned char rst_type);
void sleep(void);
void ind_testcycle(unsigned char v_select);            //Change output value sequentially and loop
void grp_testcycle(unsigned char v_select);                                                       //Change output value simultaneously on all ebow and loop
void blink_led2(unsigned char count, unsigned char maxcnt);

volatile unsigned char aslp_lcnt = 0;
volatile unsigned int aslp_scnt = 0;                                            //Incremented everytime TMR2 interrupt (at every 5ms), aslp_cnt=12000 for 60 second
volatile unsigned char aslp_mcnt = 0;                                           //Incremented everytime aslp_cnt=12000
unsigned char aslp_timeout = 1;

volatile unsigned char indc_scnt = 0;
volatile unsigned int  grpc_scnt = 0;
volatile unsigned char tcyc_idx = 0;
volatile unsigned char tcyc_led = 0;

//volatile unsigned char ebow_idx = 0;
//unsigned char tcyc_idxlimit;
                                               //Default value set to autosleep at 15min
volatile unsigned char rxbuf = 0;                                               //Hold the value read from EUSART module buffer
volatile unsigned char dummy;
const unsigned char voltmap[5] = {0b1111,0b0111,0b0011,0b0001,0b0000};
const unsigned char indtmap[42] = {1,2,3,4,5,6,
                                 17,18,19,20,21,6,
                                 33,34,35,36,37,6,
                                 49,50,51,52,53,6,
                                 65,66,67,68,69,6,
                                 81,82,83,84,85,6,
                                 97,98,99,100,101,6};

bit RXDONE;
bit GOTOSLEEP;
bit WAKEUP;
bit INDTCYCLE;
bit GRPTCYCLE;
bit CLATCH;

sbit LED1 at LATA.B4;                                                           //LED1 link at RA4 pin
sbit LED2 at LATA.B5;                                                           //LED2 link at RA5 pin

sbit ENEW1 at LATB.B0;                                                          //Enable EBOW1 at RB0 pin
sbit ENEW2 at LATB.B1;                                                          //Enable EBOW2 at RB1 pin
sbit ENEW3 at LATB.B2;                                                          //Enable EBOW3 at RB2 pin
sbit ENEW4 at LATB.B3;                                                          //Enable EBOW4 at RB3 pin
sbit ENEW5 at LATB.B4;                                                          //Enable EBOW5 at RB4 pin
sbit ENEW6 at LATB.B5;                                                          //Enable EBOW6 at RB5 pin
sbit ENEW7 at LATB.B6;                                                          //Enable EBOW7 at RB6 pin
sbit LATEW1 at LATC.B0;                                                         //Volt.select latch EBOW1 at RC0
sbit LATEW2 at LATC.B1;                                                         //Volt.select latch EBOW2 at RC1
sbit LATEW3 at LATC.B2;                                                         //Volt.select latch EBOW3 at RC2
sbit LATEW4 at LATC.B3;                                                         //Volt.select latch EBOW4 at RC3
sbit LATEW5 at LATC.B4;                                                         //Volt.select latch EBOW5 at RC4
sbit LATEW6 at LATC.B5;                                                         //Volt.select latch EBOW6 at RC5
sbit LATEW7 at LATB.B7;                                                         //Volt.select latch EBOW7 at RC7

#endif