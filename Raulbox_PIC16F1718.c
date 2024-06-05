/*******************************************************************************
** PROJECT:     RAULBOX_MKII_PIC16F1718
** VERSION:     V1.0
** CIE-AUTHOR:  SOUNDMOD - SIMON ROBITAILLE
** DATE: 2018/06/26
** DESCRIPTION:
**
**
**
********************************************************************************
********************************************************************************
** PORT CONFIGURATION:

*******************************************************************************/

#include <Raulbox_PIC16F1718.h>

void interrupt(void) {
     GIE_bit = CLR;                                                             //Disable global interrupt

     if(PIR1.CCP1IF) {                                                          //On interrupt this will clear all pin mapped to CCP1
        CCP1IF_bit = CLR;                                                       //Clear ccp1 interrupt flag
        TMR1IF_bit = CLR;
        LATC = PORTC&0xC0;                                                      //Clear pin EBOW.CLK01 to EBOW.CLK06
        LATEW7 = CLR;                                                           //Clear pin EBOW.CLK07
        TMR1ON_bit = CLR;                                                       //Disable timer1
        TMR1H = _TMR1H;                                                         //Reload preload value in timer1 high and low register
        TMR1L = _TMR1L;
     }
     
     if(PIR1.RCIF) {
        rxbuf = RC1REG;
        RCIF_bit = CLR;
        RXDONE=SET;
     }
     
     if(TMR2IF_bit) {
       TMR2IF_bit=CLR;
       aslp_scnt++;
       aslp_lcnt++;
       if(aslp_lcnt > 200){aslp_lcnt=0;LED1=~LED1;}
       if(aslp_scnt > 12000) {aslp_scnt=0;aslp_mcnt++;}
       if(aslp_mcnt > aslp_timeout) {
          TMR2ON_bit=CLR;
          aslp_mcnt=0;
          aslp_scnt=0;
          GOTOSLEEP=SET;
       } else {
          GOTOSLEEP=CLR;
       }
     }

     if(TMR4IF_bit) {
       TMR4IF_bit=CLR;
       indc_scnt++;
       tcyc_led++;
       if(indc_scnt > 180) {indc_scnt=0;tcyc_idx++;CLATCH=SET;}
       if(tcyc_idx > 41) {tcyc_idx=0;}
     }

     if(TMR6IF_bit) {
        TMR6IF_bit=CLR;
        grpc_scnt++;
        tcyc_led++;
        if(grpc_scnt > 5000) {grpc_scnt=0;tcyc_idx++;CLATCH=SET;}
        if(tcyc_idx > 4) {tcyc_idx=0;}
     }
     
     GIE_bit = SET;                                                             //Enable global interrupt
}

void setup(void) {

     TRISA = 0X00;                                                              //Set port A as digital output
     ANSELA = 0X00;                                                             //Disable analog input on port A
     LATA = 0x00;                                                               //Clear PORTA
     
     TRISB = 0X00;                                                              //Set port B as digital output
     ANSELB = 0X00;                                                             //Disable analog input on port B
     LATB = 0x00;                                                               //Clear PORTB
     WPUB = 0x00;
     ODCONB = 0x00;
     
     TRISC = 0X80;                                                              //Set port RC0-RC6 as digital output, RC7 as digital input
     ANSELC = 0X00;                                                             //Disable analog input on port C
     LATC = 0x00;                                                               //Clear PORTC

     UART1_Remappable_Init(_BAUDRATE);                                          //Init UART module                                                             //Lock peripheral pin select
     RCIF_bit = CLR;                                                            //Clear MSSP RX interrupt flag
     RCIE_bit = SET;                                                            //Enable MSSP RX interrupt

     
     //CCP1CON = 0x09;                                                          //Set CCP1 to compare and clear output on CCP1IF
     CCP1CON = 0x0A;                                                            //Set CCP1 to compare and generate software interrupt only every 500us
     T1CON = 0x00;                                                              //Timer1 set to (Fosc/4) no prescaler
     TMR1H = _TMR1H;                                                            //Configure high register of timer1
     TMR1L = _TMR1L;                                                            //Configure low register of timer1
     CCPR1H = 0x00;                                                             //Initialise both CPP1 register (combination of TMR1x and CCPR1x allow
     CCPR1L = 0x00;                                                             //an CCP1 interrupt at 500us
     CCP1IE_bit = SET;                                                          //Enable CCP1 interrupt
     CCP1IF_bit = CLR;                                                          //Clear CCP1 interrupt flag
     
     T2CON = 0x4A;                                                              //Set TMR2 with prescale 1:10 and postscaler 1:16, but keep disabled
     PR2 = 250;                                                                 //Set PR2 value to interrupt every 5ms
     TMR2IE_bit = SET;                                                          //Enable TMR2 interrupt
     TMR2IF_bit = CLR;                                                          //Clear TMR2 interrupt flag

     T4CON = 0x39;                                                              //Timer4 used for the Independent cycle test mode.
     PR4 = 250;                                                                 //Set PR4 value to interrupt every 1ms
     TMR4IE_bit = SET;                                                          //Enabled TMR4 interrupt
     TMR4IF_bit = CLR;                                                          //Clear TMR4 interrupt flag

     T6CON = 0x39;
     PR6 = 250;
     TMR6IE_bit = SET;
     TMR6IF_bit = CLR;

     GIE_bit = SET;                                                             //Enable global interrupt
     PEIE_bit = SET;                                                            //Enable peripheral interrupt

     VREGCON = 0x00;                                                            //Sleep configure as normal mode (faster wake-up)
}

void main(void) {

      setup();                                                                  //Configure microcontroller

      RXDONE=CLR;                                                               //Status bit initialization
      WAKEUP=CLR;
      GOTOSLEEP=CLR;
      INDTCYCLE=CLR;
      GRPTCYCLE=CLR;
      CLATCH=CLR;
      LED1 = CLR;
      LED2 = SET;
      
      while(1){
      
            static unsigned int runcnt = 0;                                     //Initialize main loop counter for LED2
            
            if(WAKEUP) {LED1=CLR;WAKEUP=CLR;}                                   //Turn on LED1 if recover from sleep and clear flag
            if(RXDONE) {                                                        //Check if data reception is done
               if(rxbuf!=_WAKEBYTE) {get_command(rxbuf);}                       //If RX data byte isn't a WAKEUP byte (0x00) get the command to execute
               aslp_scnt=0;                                                     //Reset the second counter of the sleep timer
               aslp_mcnt=0;                                                     //Reset the minute counter of the sleep timer
               RXDONE = CLR;                                                    //Clear the RXDONE flag to avoid reexecution of the previous step
            }
            if(!INDTCYCLE && !GRPTCYCLE) {                                      //If both test mode enable bit are disabled
              runcnt++;                                                         //Increment the main loop counter
              blink_led2(runcnt>>8, 240);                                       //Set LED2 state to blink when main loop count reach 240
            }
            if(GOTOSLEEP) {sleep();}                                            //If flag set enter sleep mode
            if(INDTCYCLE) {                                                     //If independent cycle test mode is enabled...
               ind_testcycle(indtmap[tcyc_idx]);                                //Select and set ebow output
               blink_led2(tcyc_led, 200);                                       //Set LED2 state to blink when timer interrupt count reach 200
            }
            if(GRPTCYCLE) {                                                     //If group cycle test mode is enabled...
               grp_testcycle(tcyc_idx);                                         //Select and set ebow output
               blink_led2(tcyc_led, 200);                                       //Set LED2 state to blink when timer interrupt count reach 200
            }


      }
}

void get_command(unsigned char cmd) {

     if((cmd != _INDTCYCCMD)&&(cmd != _GRPTCYCCMD)) {                           //If not a test cycle command, then make sure test cycles are disabled
        INDTCYCLE=CLR;                                                          //Disable Independent cycle test mode
        GRPTCYCLE=CLR;                                                          //Disable Group cycle test mode
        TMR4ON_bit=CLR;                                                         //Disable Timer4
        TMR4IF_bit=CLR;                                                         //Clear Timer4 interrupt flag
        TMR6ON_bit=CLR;                                                         //Disable Timer6
        TMR6IF_bit=CLR;                                                         //Clear Timer6 interrupt flag
        LED2 = SET;                                                             //Turn off LED2
     }
     else if((cmd == _INDTCYCCMD)||(cmd == _GRPTCYCCMD)) {                      //If its one of the test cycle command then...
        asleep_config(_ASLPDIS);                                                //Disable autosleep mode
        rst_ebow(0xFF);                                                         //Reset and disable ebow outputs

        if(cmd==_INDTCYCCMD) {TMR4ON_bit=SET; TMR6ON_bit=CLR;}                  //If independent cycle mode selected, start Timer4 and disable Timer6
        else {TMR4ON_bit=CLR; TMR6ON_bit=SET;}                                  //If group cycle mode selected, disable Timer4 and start Timer6

        LED2 = CLR;                                                             //Turn on LED2
     }
     
     if(cmd <= _EWDATARANGE) {                                                  //Check if received command is an ebow control value
        set_ebow(cmd);                                                          //Set voltage output on selected ebow
     } else {
       switch(cmd) {
              default:                                                          //Used for timer value
                 if(cmd>=_ASLPEN&&rxbuf<=_ASLP60M) {                            //Check if data is an auto sleep config command
                   asleep_config(cmd);                                          //Configure auto-sleep timer (enable/disable, set timeout value)
                 }
              break;
              case _RESETCMD:                                                   //Reset all output command
                   rst_ebow(0x00);                                              //Execute ebow reset but keep ebow regulator enabled
              break;
              case _INDTCYCCMD:                                                 //Independent test cycle command
                   INDTCYCLE=SET;                                               //Set independent test cycle flag (enable execution)
                   GRPTCYCLE=CLR;                                               //Clear group test cycle flag
              break;
              case _GRPTCYCCMD:                                                 //Group test cycle command
                   GRPTCYCLE=SET;                                               //Set group test cycle flag (enable execution)
                   INDTCYCLE=CLR;                                               //Clear independent test cycle flag
              break;
              case _INSTANTSLP:                                                 //Instant sleep command
                   sleep();                                                     //Execute sleep routine
              break;
       }
     }
}


void asleep_config(unsigned char aslp_dat) {
     switch(aslp_dat) {
           case _ASLPEN:                                                        //Enable auto sleep timer
                TMR2ON_bit = SET;
           break;
           case _ASLPDIS:                                                       //Disable auto sleep timer
                TMR2ON_bit=CLR;
                aslp_scnt = 0;
                aslp_mcnt = 0;
                LED1 = CLR;
           break;
           case _ASLP15M:                                                       //Set auto sleep timer duration to 15min
                aslp_timeout=15;
           break;
           case _ASLP20M:                                                       //Set auto sleep timer duration to 20min
                aslp_timeout=20;
           break;
           case _ASLP25M:                                                       //Set auto sleep timer duration to 25min
                aslp_timeout=25;
           break;
           case _ASLP30M:                                                       //Set auto sleep timer duration to 30min
                aslp_timeout=30;
           break;
           case _ASLP35M:                                                       //Set auto sleep timer duration to 35min
                aslp_timeout=35;
           break;
           case _ASLP40M:                                                       //Set auto sleep timer duration to 40min
                aslp_timeout=40;
           break;
           case _ASLP45M:                                                       //Set auto sleep timer duration to 45min
                aslp_timeout=45;
           break;
           case _ASLP50M:                                                       //Set auto sleep timer duration to 50min
                aslp_timeout=50;
           break;
           case _ASLP55M:                                                       //Set auto sleep timer duration to 55min
                aslp_timeout=55;
           break;
           case _ASLP60M:                                                       //Set auto sleep timer duration to 60min
                aslp_timeout=60;
           break;
     }
}

void set_ebow(unsigned char ew_data) {

      unsigned char address = 0;
      unsigned char value = 0;

      address = ew_data&0xF0;                                                   //Get address from rxbuf high nibble
      value = voltmap[(ew_data&0x0F)-1];                                        //Get voltage from rxbuf low nibble
      LATA = (PORTA&0xF0)|value;                                                //Set voltage value

      switch(address) {
          case _MASKADDREW1:                                                    //Received address is for EBOW1
             LATEW1=SET;                                                        //Latch data to EBOW1
             ENEW1=SET;                                                         //Enable EBOW1 regulator
          break;
          case _MASKADDREW2:                                                    //Received address is for EBOW2
             LATEW2=SET;                                                        //Latch data to EBOW2
             ENEW2=SET;                                                         //Enable EBOW2 regulator
          break;
          case _MASKADDREW3:                                                    //Received address is for EBOW3
             LATEW3=SET;                                                        //Latch data to EBOW3
             ENEW3=SET;                                                         //Enable EBOW3 regulator
          break;
          case _MASKADDREW4:                                                    //Received address is for EBOW4
             LATEW4=SET;                                                        //Latch data to EBOW4
             ENEW4=SET;                                                         //Enable EBOW4 regulator
          break;
          case _MASKADDREW5:                                                     //Received address is for EBOW5
             LATEW5=SET;                                                        //Latch data to EBOW5
             ENEW5=SET;                                                         //Enable EBOW5 regulator
          break;
          case _MASKADDREW6:                                                    //Received address is for EBOW6
             LATEW6=SET;                                                        //Latch data to EBOW6
             ENEW6=SET;                                                         //Enable EBOW6 regulator
          break;
          case _MASKADDREW7:                                                    //Received address is for EBOW7
             LATEW7=SET;                                                        //Latch data to EBOW7
             ENEW7=SET;                                                         //Enable EBOW7 regulator
          break;
      }
      TMR1ON_bit=SET;                                                           //Start clock pulse to latch data
}

void rst_ebow(unsigned char rst_type) {
         if(rst_type==0x00) {
            LATA = (PORTA&0xF0)|voltmap[0];                                     //Set port A to select to lowest voltage (1.75V)
            LATC = 0x3F;                                                        //Latch data to ebow 1 to 6
            LATEW7 = SET;                                                       //Latch data to ebow7
            TMR1ON_bit = SET;                                                   //Start clock pulse to latch data
            //while(TMR1ON_bit) {};                                             //Wait until CCP1 clear timer1 enable bit and return
         } else {                                                               //If low_power reset type(0xFF)
            LATA = (PORTA&0xF0)|voltmap[4];                                     //Turn off all the optocoupler output to minimize current
            LATC = 0x3F;                                                        //Latch data to ebow 1 to 6
            LATEW7 = SET;                                                       //Latch data to ebow 7
            TMR1ON_bit = SET;                                                   //Start clock pulse to latch data
            //while(TMR1ON_bit) {};                                             //Wait until CCP1 clear timer1 enable bit and return
            LATB = PORTB&0x80;                                                  //Disable voltage regulator on EBOW 1 to 7
         }
}

void ind_testcycle(unsigned char v_select) {
    if(CLATCH) {
      TMR4ON_bit=CLR;
      if(v_select!=6){set_ebow(v_select);}
      else{rst_ebow(0xFF);}
      CLATCH=CLR;
      TMR4ON_bit=SET;
    }
}

void grp_testcycle(unsigned char v_select) {
     if(CLATCH) {
       //TMR4ON_bit=CLR;
       TMR6ON_bit=CLR;
       if(v_select==0) {LATB = PORTB&0x80;}else{LATB = PORTB|0x7F;}
       LATA = (PORTA&0xF0)|voltmap[v_select];
       LATC = 0x3F;
       LATEW7=SET;
       TMR1ON_bit=SET;
       //while(TMR1ON_bit) {};
       CLATCH=CLR;
       //TMR4ON_bit=SET;
       TMR6ON_bit=SET;
     }
}

void blink_led2(unsigned char count, unsigned char maxcnt) {
     if(count > maxcnt) {tcyc_led=0; LED2=~LED2;}
}

void sleep(void) {
         GOTOSLEEP=CLR;                                                         //Clear flag to not reenter sleep on wake-up
         rst_ebow(0xFF);                                                        //Disable all ebow and put optocoupler in low current state (all LED off)
         WUE_bit = SET;                                                         //Enable wake-up from EUSART module
         LED1 = SET;                                                            //Shutdown LED1
         LED2 = SET;                                                            //Shutdown LED2
         //LED1 = CLR;
         //OSCCON = OSCCON|0x02;                                                //Switch to internal clock
         //while(!OSCSTAT.B7) {};
         WAKEUP=SET;
         asm sleep;                                                             //Go to sleep
         asm nop;                                                               //No operation
         if(nPD_bit&&nTO_bit){asm sleep;}                                       //Reexecute sleep command if previous execute as nop
         asm nop;                                                               //No operation
}