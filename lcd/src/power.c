/**
@file	power.c
@brief	Butterfly MP3 Power Routines
@author	Nick Lott 
@date	January 2006
 
$Id: power.c,v 1.9 2007/06/04 15:12:18 brokentoaster Exp $

 
Copyright (C) 2004 Nick Lott <brokentoaster@users.sf.net>
http://butterflymp3.sf.net/

This is the power control software for the ButterflyMP3 project.
This file contains all software responsible for controlling the
power save modes for the mp3 player.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation, 
Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Target(s)...: ATmega169

Compiler....: AVR-GCC 3.3.1; avr-libc 1.2
 
**/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "power.h"
#include "main.h"
#include "types.h"
#include "uart.h"
#include "vs1001.h"
#include "lcdgraphics.h"
#include "eeprom.h"
#include "ADC.h"

/**
*	Power_save
*
*	Set Butterfly into powersave mode when nothing happening
*	Keep timer2 and pin change interupts on only.
**/
void Power_save(void)
{
	PRINT("pDWN");
	SMCR = (2<<SM0) | (1<<SE);      // Enable Power-down mode
	asm volatile ("sleep"::);
	// NOTE: need to hit a button when inserting card to wakeup. !!
	// will add RTC later for power down time out etc which should 
	// wake up and detect card once a second.
	SMCR = 0;                       // Just woke, disable sleep
}


/**
 *	Power_idle
 *
 *	Set Butterfly into idle mode when nothing happening
 *	Keeps the timers running etc.
 **/
void Power_idle(void)
{
	//PRINT("pIDLE");
	SMCR = (1<<SE);                 // Enable idle mode
	asm volatile ("sleep"::);
	
	SMCR = 0;                       // Just woke, disable sleep
}


/**
 *	Power_off
 *
 *	Shut down all non essential systems and go into low power mode.
 **/
void Power_off(void)
{
	PRINT("pOFF");EOL();
	uint16	buf[2];
	
	//SHUTDOWN LCD
	LCD_Sleep();					
	
	//SHUTDOWN UART
	UART_Shutdown();
	
	//SHUTDOWN vs1001k
	vs1001_read(0, 1, buf);
	buf[1] |= SM_PDOWN;
	vs1001_write(0,1,buf);
	
	CBI(RESET_PORT, RESET_PIN);	// RESET- low
	// RESET_PIN is pulled low by external resistor ( R9 rev C) during bootloader mode
	// R9 is neccesary to hold reset low while Butterfly is in bootloader mode.
	
	//MMC should go into shutdown by default as per 
	//"SanDisk MultiMediaCard and Reduced-Size MultiMediaCard Product Manual"
	//Doc No. 80-36-00320 v1.0 page 1-4 sec 1.10
	
	while(1){
		// Put into low power mode (no write to eeprom)
		Power_save();
		
		if(!(PINB & (1<<PINB6)))	// mt if(!(PINB & (1<<PORTB6)))
		{   // joystick "up" detected
			
			//wake up UART
			UART_Init();
			
			//wake up VS1001
			SBI(RESET_PORT, RESET_PIN);	// RESET- Hi
			
			//wake up LCD
			LCD_Wake();
			
			return;
		}
		else if(!(PINB & (1<<PINB7)))                
		{	//Joystick "down" detected
			
			//save current settings
			save_player_options();
			
			//TODO: Write MMC Serial Number to EEPROM
			
			//write current song to EEPROM
			save_player_state();
			
			//Jump to Bootloader
			WDTCR = (1<<WDCE) | (1<<WDE);     //Enable Watchdog Timer to give reset
			while(1);   
			// wait for watchdog-reset, since the BOOTRST-fuse is 
			// programmed, the Boot-section will be entered upon reset.
			
			// Press UP to turn on again. (and exit the bootloader)
			
		}
	}
	
	// NOTE: for the standby mode to work as intended, the butterfly must have 
	// the original bootloader installed and have the 
	// "Boot Reset vector Enabled - BOOTRST" fuse programmed.
	
}

/** 
*	Power_Init
*
*	Set up ADC for monitoring the battery voltage using the
*	internal 1.1V reference.
**/
void Power_Init(void)
{
	
	// clear portf for ADC use 
	DDRF = 0x00;
	PORTF = 0x00;
	
	//setup adc internal 1.1 volt reference and voltage sensor
	ADMUX = VOLTAGE_SENSOR|(1<<REFS0)|(1<<REFS1);
	//Free running mode 
	ADCSRB = (0<<ADTS2) |(0<<ADTS1) |(0<<ADTS0);
	// set ADC prescaler to , 8MHz / 128 = 62.5kHz  ,  auto trigger
	ADCSRA = (1<<ADEN)| (1<<ADSC) | (0<<ADATE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	
}




/**
*	Power_check
*
*	Check the battery voltage and return level
*
*	@return uint8 containing voltage level
**/
uint16	Power_check(void)
{
	uint8 i;
	uint16	batteryVoltage=0;
	uint32 ADC_temp;
	
	// Check Battery Voltage.
	ADCSRA |= (1<<ADEN)|(1<<ADSC);		//Start ADC
	while(!(ADCSRA & 0x10));    // wait for conversion done, ADIF flag active
	
    for(i=0;i<8;i++)            // do the ADC conversion 8 times for better accuracy 
    {
        ADCSRA |= (1<<ADSC);        // do single conversion
        while(!(ADCSRA & 0x10));    // wait for conversion done, ADIF flag active
        
        ADC_temp = ADCL;            // read out ADCL register
        ADC_temp += (ADCH << 8);    // read out ADCH register        
		
        batteryVoltage += ADC_temp;      // accumulate result (8 samples) for later averaging
    }
	
    batteryVoltage = batteryVoltage >> 3;     // average the 8 samples
	
	ADCSRA &= ~((1<<ADEN));
	//ADCSRA |= (1<<ADIF); //reset interruppt flag
	
	PRINT("VOLTAGE ");
	UART_Printfu16(batteryVoltage);
	EOL();
	
	return batteryVoltage;
}

/**
 *	Power_pcnt
 *
 *	Returns the ammount of percentage remaingin
 *	battery time based on experimental values.
 <table>
 <tr><td>mins </td><td>  %  </td><td> ADC </td><td> ADC   </td><td>~Voltage</td></tr>
 <tr><td>0    </td><td> 100 </td><td> 655 </td><td> 0x28f </td><td> 4.2 </td></tr>
 <tr><td>400  </td><td> 66  </td><td> 608 </td><td> 0x260 </td><td> 3.9	</td></tr>
 <tr><td>1050 </td><td> 12  </td><td> 572 </td><td> 0x23c </td><td> 3.6	</td></tr>
 <tr><td>1200 </td><td> 0   </td><td> 445 </td><td> 0x1BD </td><td> 2.8	</td></tr>
 </table>
 *	@param	voltage	uint16 reading from ADC
 *	@return 0-100 percentage remaining battery
 *	
 **/
uint16 Power_pcnt(uint16 voltage)
{
	uint16 percentage;
	
	if (voltage > PWR_MAX_V){ // battery is really really good
		percentage = 100;
		
	}else if (voltage> PWR_GOOD_V){ // battery is freshly charged
		percentage =  (voltage - PWR_GOOD_V) * (100-66);
		percentage /= (PWR_MAX_V -PWR_GOOD_V);
		percentage += 66;
		
	}else if (voltage>PWR_LOW_V){ // normal operation mode
		percentage =  (voltage - PWR_LOW_V) * (66-12); 
		percentage /= (PWR_GOOD_V - PWR_LOW_V);
		percentage += 12;
		
	}else{ // only 12% left
		percentage =  (voltage - PWR_ABS_MIN_V) * (12-0);
		percentage /= (PWR_LOW_V - PWR_ABS_MIN_V);
		
	}
	
	// old calculation
	//percentage = (voltage-PWR_ABS_MIN_V)*100/(PWR_MAX_V-PWR_ABS_MIN_V);	

	return percentage;
}

