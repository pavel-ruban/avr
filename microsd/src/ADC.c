//***************************************************************************
//
//  File........: ADC.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: AVR-GCC 3.3.1; avr-libc 1.2
//
//  Description.: AVR Butterfly ADC routines
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - LHM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20050610	      modified for VUW datalogger use		    - N.Lott
//
//***************************************************************************

//mtA
//#include <inavr.h>
//#include "iom169.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "pgmspacehlp.h"

//mtE

#include "main.h"
#include "ADC.h"
#include "BCD.h"
#include "LCD_functions.h"
#include "types.h"
//#include "timer0.h"
#include "uart.h"
//#include "speed.h"
//MP3 Player #include "dataflash.h"

#define Log2Flash 1

// mt __flash int TEMP_Celcius_pos[] =    // Positive Celcius temperatures (ADC-value)
const int TEMP_Celcius_pos[] PROGMEM =    // Positive Celcius temperatures (ADC-value)
        {                           // from 0 to 60 degrees
            806,796,786,775,765,754,743,732,720,709,697,685,673,661,649,
            636,624,611,599,586,574,562,549,537,524,512,500,488,476,464,
            452,440,429,418,406,396,385,374,364,354,344,334,324,315,306,
            297,288,279,271,263,255,247,240,233,225,219,212,205,199,193,
            187,
        };

// mt __flash int TEMP_Celcius_neg[] =    // Negative Celcius temperatures (ADC-value)
const int TEMP_Celcius_neg[] PROGMEM =    // Negative Celcius temperatures (ADC-value)
        {                           // from -1 to -15 degrees
            815,825,834,843,851,860,868,876,883,891,898,904,911,917,923,
        };
/* Removed for space - NAL
// mt __flash int TEMP_Farenheit_pos[] =  // Positive Farenheit temperatures (ADC-value)
const int TEMP_Fahrenheit_pos[] PROGMEM =  // Positive Fahrenheit temperatures (ADC-value)
        {                           // from 0 to 140 degrees
		    938, 935, 932, 929, 926, 923, 920, 916, 913, 909, 906, 902, 898, 
    		894, 891, 887, 882, 878, 874, 870, 865, 861, 856, 851, 847, 842, 
    		837, 832, 827, 822, 816, 811, 806, 800, 795, 789, 783, 778, 772, 
    		766, 760, 754, 748, 742, 735, 729, 723, 716, 710, 703, 697, 690, 
    		684, 677, 670, 663, 657, 650, 643, 636, 629, 622, 616, 609, 602, 
    		595, 588, 581, 574, 567, 560, 553, 546, 539, 533, 526, 519, 512, 
    		505, 498, 492, 485, 478, 472, 465, 459, 452, 446, 439, 433, 426, 
    		420, 414, 408, 402, 396, 390, 384, 378, 372, 366, 360, 355, 349, 
    		344, 338, 333, 327, 322, 317, 312, 307, 302, 297, 292, 287, 282, 
    		277, 273, 268, 264, 259, 255, 251, 246, 242, 238, 234, 230, 226, 
    		222, 219, 215, 211, 207, 204, 200, 197, 194, 190, 187, 
        };
*/
   /*     
// mt __flash int LIGHT_ADC[] = // Table used to find the Vref, when using the voltage-reading function 
const int LIGHT_ADC[] PROGMEM = // Table used to find the Vref, when using the voltage-reading function 
        { 
            0x35,0x60,0x80,0x0B0,0x11D,0x13D,0x15A,0x17A,0x197,0x1B9,0x1DA,
            0x1F9,0x216,0x240,0x26D,0x282,0x2A2,0x2EF,0x332,0x3B0,0x3F2
        };
    */
/*    
//mt __flash float LIGHT_VOLTAGE[] = // Vref table correspondent to the LIGHT_ADC[] table
const float LIGHT_VOLTAGE[] PROGMEM = // Vref table correspondent to the LIGHT_ADC[] table
        {
            2.818,2.820,2.824,2.827,2.832,2.835,2.839,2.841,2.843,2.847,2.850,
            2.853,2.857,2.863,2.867,2.870,2.874,2.882,2.893,2.917,2.939
        };

*/
float 	Vref = 2.900; // initial value
char 	degree = CELCIUS; // char degree = CELCIUS;

//MP3 Player extern char gLogging;
//MP3 Player extern char gLogNow;
//MP3 Player int 	gMaxTemp=1023;
//MP3 Player int 	gMinTemp=0;

/*****************************************************************************
*
*   Function name : ADC_init
*
*   Returns :       None
*
*   Parameters :    char input
*
*   Purpose :       Initialize the ADC with the selected ADC-channel
*
*****************************************************************************/
void ADC_init(char input)
{
  
	// todo : change to internal bandgap referencec when input = voltage.
//	if (input == VOLTAGE_SENSOR){
//		// disable the VCP (VC-peripheral)
//		PORTF &= ~(1<<PF3); // mt sbi(PORTF, PORTF3);     // Enable 
//		DDRF &=  ~(1<<DDF3);    
//		ADMUX = VOLTAGE_SENSOR|(1<<REFS0)|(1<<REFS1); // select internal bandgap reference see VOLTAGE_SENSOR definition
//		Vref = 3.0;
//	}else{
// !!!:nick:20051120 
	    PORTF |= (1<<PF3); // mt sbi(PORTF, PORTF3);     // Enable the VCP (VC-peripheral)
		DDRF|=  (1<<DDF3); // sbi(DDRF, PORTF3);      
		ADMUX = input;    // external AREF and ADCx
		Vref = 2.900;
  //  }
    ADCSRA = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);    // set ADC prescaler to , 1MHz / 8 = 125kHz    
	//MP3 PLayer : note above line has timing 8 x faster.
    input = ADC_read();        // dummy 
}


/*****************************************************************************
*
*   Function name : ADC_read
*
*   Returns :       int ADC
*
*   Parameters :    None
*
*   Purpose :       Do a Analog to Digital Conversion
*
*****************************************************************************/
int ADC_read(void)
{
    char i;
    int ADC_temp;
	// mt int ADC = 0 ;
	int ADCr = 0;
    
    // To save power, the voltage over the LDR and the NTC is turned off when not used
    // This is done by controlling the voltage from a I/O-pin (PORTF3)
	/*
    CBI(PORTD, PF4); // Set internal pullup
    CBI(DDRF, DDF4
    */
	PORTF &= ~(1<<PF4); /// WARNING IS THIS A TYPO PORTD OR PORTF?????
	DDRF &= ~(1<<DDF4);
	/*
    SBI(PORTF, PF3); // mt sbi(PORTF, PORTF3);     // Enable the VCP (VC-peripheral)
    SBI(DDRF, DDF3); // sbi(DDRF, PORTF3);        
	 */

	PORTF |=(1<<PF3);
	DDRF |= (1<<DDF3);
	
	ADCSRA |= (1<<ADEN);
	
    //SBI(ADCSRA, ADEN);     // Enable the ADC

    //do a dummy readout first
    ADCSRA |= (1<<ADSC);        // do single conversion
    while(!(ADCSRA & 0x10));    // wait for conversion done, ADIF flag active
        
    for(i=0;i<8;i++)            // do the ADC conversion 8 times for better accuracy 
    {
        ADCSRA |= (1<<ADSC);        // do single conversion
        while(!(ADCSRA & 0x10));    // wait for conversion done, ADIF flag active
        
        ADC_temp = ADCL;            // read out ADCL register
        ADC_temp += (ADCH << 8);    // read out ADCH register        

        ADCr += ADC_temp;      // accumulate result (8 samples) for later averaging
    }

    ADCr = ADCr >> 3;     // average the 8 samples
/*        
    CBI(PORTF,PF3);     // mt CBI(PORTF, PORTF3);     // disable the VCP
    CBI(DDRF,DDF3);     // mt CBI(DDRF, PORTF3);  
    
    CBI(PORTF,PF6);     //disable VCP
    CBI(DDRF,DDF6);
    CBI(PORTF,PF4);     //disable the internal Pullup.
    CBI(DDRF,DDF4);
*/
  //  PORTF &= ~(1<<PF3)|(1<<PF4)|(1<<PF6);
	DDRF &= ~(1<<DDF3)|(1<<DDF4)|(1<<DDF6);
	
    
	//CBI(ADCSRA, ADEN);      // disable the ADC
	ADCSRA &= ~(1<<ADEN);
    return ADCr;
}


/*****************************************************************************
*
*   Function name : ADC_periphery
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calculates the Temperature/Voltage/Ligth from the ADC_read
*                   and puts it out on the LCD.
*
*****************************************************************************/
//void ADC_periphery(void)
//{
//    int ADCresult = 0;
//    
//    if (!gLogNow) {      // don't read while lig in process
//	ADCresult=ADC_read();   // read ADC Value
//	
//	switch ((ADMUX & 7)){
//	    case TEMPERATURE_SENSOR:
//		ADC2Temp(FALSE,ADCresult);
//		break;
//	    case DIR_SENSOR :
//		ADC2Direction(FALSE,ADCresult);
//		break;
//	    case VOLTAGE_SENSOR:
//		ADC2Volt(FALSE,ADCresult);
//		break;
//	    case LIGHT_SENSOR:
//		ADC2Light(FALSE,ADCresult);
//	}
//    }
//    
//}

/*****************************************************************************
*
*   Function name : ADC2Temp
*
*   Returns :       nothing
*
*   Parameters :    char log (char to log to rs232 else print to screen)
*					int ADCResult (ADC reading to convert)
*
*   Purpose :       Convert an ADC reading into a temp
*
*****************************************************************************/
void ADC2Temp(char log,int ADCresult)
{
    //   int ADCresult = 0;
    //   int ADCresult_temp = 0;
    int Temp_int=0;
    int dif;
    int top;
    
    char Temp;
    unsigned char i = 0;
    char TL;
    char TH; 
    char VH;	
    char VL;
    
//    if (ADCresult<gMaxTemp)   // these appear backwards (the gt and lt) due to NTC of thermister
//		gMaxTemp=ADCresult;
//    if (ADCresult>gMinTemp)
//		gMinTemp=ADCresult;
    
    /*	if (gTempOffset)
    {
	ADCresult = ADCresult -gTempOffset;
    }	          
    */
    if(degree == CELCIUS){
		if(ADCresult > 810){         // If it's a negtive temperature
			for (i=0; i<=25; i++){   // Find the temperature
				if (ADCresult <= pgm_read_word(&TEMP_Celcius_neg[i])){
					// add a decimal reading
					if (i>0){	
						dif= pgm_read_word(&TEMP_Celcius_neg[i])-pgm_read_word(&TEMP_Celcius_neg[i-1]);
						top =(ADCresult-pgm_read_word(TEMP_Celcius_neg[i-1]))*100;
						
						Temp_int = top/(dif+1);
						i++;
					}
					break;
				}
			}
			
			//LCD_putc(0, '-');       // Put a minus sign in front of the temperature
		}
		else if (ADCresult < 800){   // If it's a positive temperature
			for (i=0; i<100; i++)  {
				if (ADCresult >= pgm_read_word(&TEMP_Celcius_pos[i])){
					// add a decimal reading
					if (i>0){	
						dif= pgm_read_word(&TEMP_Celcius_pos[i-1])-pgm_read_word(&TEMP_Celcius_pos[i]);
						top =(pgm_read_word(&TEMP_Celcius_pos[i-1])-ADCresult)*100;
						
						Temp_int = top/(dif+1);
						i--;
					}
					break;
				}
			}        
	    //LCD_putc(0, '+');       // Put a plus sign in front of the temperature
		} else{                        //If the temperature is zero degrees
			i = 0;
			LCD_putc(0, ' ');
		}
	
		Temp = CHAR2BCD2(i);        // Convert from char to bin
		TL = (Temp & 0x0F) + '0';   // Find the low-byte
		TH = (Temp >> 4) + '0';     // Find the high-byte
		
		Temp= CHAR2BCD2(Temp_int);
		VH = (Temp >> 4) + '0';
		VL = (Temp & 0x0F) + '0';
		
		if(log)	{
			/*		Usart_Tx(TH);
			Usart_Tx(TL);
			Usart_Tx('.');
			Usart_Tx(VH);
			Usart_Tx(VL);
			Usart_Tx(' ');	   
			*/
//	#ifdef Log2Flash
//			DF_SPI_RW(TH);
//			DF_SPI_RW(TL);
//			DF_SPI_RW('.');
//			DF_SPI_RW(VH);
//			DF_SPI_RW(VL);
//			DF_SPI_RW(' ');	
//	#endif
		} else {
			LCD_putc(0, TH);
			LCD_putc(1, TL);
			LCD_putc(2, VH);
			LCD_putc(3, VL);
			LCD_putc(4, '*');
			LCD_putc(5, 'C');
			LCD_Colon(TRUE);
			LCD_putc(6, '\0');
		}
		
    }
    /*
	else if (degree == FARENHEIT)
     {
	    for (i=0; i<=141; i++)   // Find the temperature
	    {
		if (ADCresult > pgm_read_word(TEMP_Farenheit_pos[i]))
		{
		    break;
		}
	    }        
	    
	    Temp_int = CHAR2BCD3(i);
	    
	    if (i > 99) // if there are three digits
	    {
		LCD_putc(0, '+');
		TH = (Temp_int >> 8) + '0';   // Find the high-byte
		LCD_putc(1, TH);
	    }
	    else    // if only two digits
	    {
		LCD_putc(0, ' ');
		LCD_putc(1, '+');
	    }
	    
	    TL = (Temp_int & 0x0F) + '0';   // Find the low-byte
	    TH = ( (Temp_int >> 4) & 0x0F ) + '0';     // Find the high-byte                
	    
	    LCD_putc(2, TH);
	    LCD_putc(3, TL);
	    LCD_putc(4, '*');
	    LCD_putc(5, 'F');
	    LCD_putc(6, '\0');
	    LCD_Colon(FALSE);
	    
     }
     */
}


/*****************************************************************************
*
*   Function name : ADC2Direction
*
*   Returns :       nothing
*
*   Parameters :    char log (char to log to flash else print to screen)
*					int ADCResult (ADC reading to convert)
*
*   Purpose :       Convert an ADC reading into a Direction Reading
*
*****************************************************************************/
//void ADC2Direction(char log, int ADCresult)
//{
//    //  int Temp_int;
//    int Temp;
//    char TL;
//    char TH; 
//    char THH;
//    
//    Temp = CHAR2BCD3((ADCL+(ADCH<<8)));                
//    
//    TL = (Temp & 0x0F) + '0';       
//    if(TL > '9')        // if the hex-value is over 9, add 7 in order to go 
//	TL += 7;        // jump to the character in the ASCII-table
//    
//    TH = ((Temp >>= 4)& 0x0F) + '0';
//    if(TH > '9')        // if the hex-value is over 9, add 7 in order to go 
//	TH += 7;        // jump to the character in the ASCII-table
//    
//    THH = ((Temp >>= 4)& 0x0F) + '0';
//    if(THH > '9')        // if the hex-value is over 9, add 7 in order to go 
//	THH += 7;        // jump to the character in the ASCII-table
//    
//    if (log)
//    {
//	/*	Usart_Tx((ADCH + 0x30));
//	Usart_Tx(TH);
//	Usart_Tx(TL);
//	Usart_Tx(' ');           */      
//#ifdef Log2Flash
//        DF_SPI_RW(THH);
//	DF_SPI_RW(TH);
//	DF_SPI_RW(TL);
//	DF_SPI_RW(' '); 
//#endif
//	
//    }else
//    {
//	LCD_putc(0, 'D');
//	LCD_putc(1, 'I');
//	LCD_putc(2, 'R');
//	LCD_putc(3, THH);
//	LCD_putc(4, TH);
//	LCD_putc(5, TL);
//	LCD_putc(6, '\0');
//    }
//}

/*****************************************************************************
*
*   Function name : ADC2Volt
*
*   Returns :       nothing
*
*   Parameters :    char log (char to log to flash else print to screen)
*					int ADCResult (ADC reading to convert)
*
*   Purpose :       Convert an ADC reading into a Voltage
*
*****************************************************************************/

void ADC2Volt(char log, int ADCresult)
{
    //	int ADCresult_temp = 0;
    char Temp;
    
    char TL;
    // char TH;
    char VH;	
    char VL;
    
    float V_ADC;
    char VoltageHB;
    char VoltageLB;
    
    V_ADC = ( ADCresult * Vref ) / 1024; // Calculate the voltage
	   
    V_ADC = ( V_ADC * 6 );      // Multiply by 6 because of the voltage division
    
    VoltageHB = V_ADC;              // Store the high-byte
    V_ADC = ( V_ADC - VoltageHB );
    VoltageLB = ( V_ADC * 100 );    // Store the low-byte
    
    Temp = CHAR2BCD2(VoltageHB);    // Convert from char to bin
    
    TL = (Temp & 0x0F) + '0';
    //TH = (Temp >> 4) + '0';
    
    Temp = CHAR2BCD2(VoltageLB);    // Convert from char to bin
    
    VH = (Temp >> 4) + '0';
    VL = (Temp & 0x0F) + '0';
    if (log)
    {
	
	/*	Usart_Tx(TL);
	Usart_Tx('.');
	Usart_Tx(VH);
	Usart_Tx(VL);
	Usart_Tx(' ');   */
	
//#ifdef Log2Flash
//	DF_SPI_RW(TL);
//	DF_SPI_RW('.');
//	DF_SPI_RW(VH);
//	DF_SPI_RW(VL);
//	DF_SPI_RW(' ');
//#endif
    }else
    {
	LCD_putc(0, ' ');
	LCD_putc(1, ' ');
	LCD_putc(2, TL);
	LCD_putc(3, 'v');
	LCD_putc(4, VH);
	LCD_putc(5, VL);
	LCD_putc(6, '\0');
    }   
    
}
/*****************************************************************************
*
*   Function name : ADC2Light
*
*   Returns :       nothing
*
*   Parameters :    char log (char to log to flash else print to screen)
*					int ADCResult (ADC reading to convert)
*
*   Purpose :       Convert an ADC reading into a Light Reading
*
*****************************************************************************/
void ADC2Light(char log, int ADCresult)
{
    //  int Temp_int;
    int Temp;
    //    char i = 0;
    char TL;
    char TH; 
    char THH;
    /*
	// Find Vref
     for (i=0; i<=22; i++)
     {
	 if (ADCresult <= LIGHT_ADC[i])
	 {
	     break;
	 }
     }
     
     if(!i)              // if it's very bright
     Vref = 2.815;
     else if(i > 21)
     Vref = 2.942;   // if it's totally dark
     else
     Vref = LIGHT_VOLTAGE[i];   
     */
    // The relation between ADC-value and lux is yet to be found, 
    // for now the ADC-value is presented on the LCD
    
    Temp = CHAR2BCD3(1023 - (ADCL+(ADCH<<8)));                
    
    TL = (Temp & 0x0F) + '0';       
    if(TL > '9')        // if the hex-value is over 9, add 7 in order to go 
	TL += 7;        // jump to the character in the ASCII-table
    
    TH = ((Temp >>= 4)& 0x0F) + '0';
    if(TH > '9')        // if the hex-value is over 9, add 7 in order to go 
	TH += 7;        // jump to the character in the ASCII-table
    
    THH = ((Temp >>= 4)& 0x0F) + '0';
    if(THH > '9')        // if the hex-value is over 9, add 7 in order to go 
	THH += 7;        // jump to the character in the ASCII-table
    
    if (log)
    {
	/*	Usart_Tx((ADCH + 0x30));
	Usart_Tx(TH);
	Usart_Tx(TL);
	Usart_Tx(' ');           */      
//#ifdef Log2Flash
//        DF_SPI_RW(THH);
//	DF_SPI_RW(TH);
//	DF_SPI_RW(TL);
//	DF_SPI_RW(' '); 
//#endif
	
    }else
    {
	LCD_putc(0, 'L');
	LCD_putc(1, 'D');
	LCD_putc(2, 'R');
	LCD_putc(3, THH);
	LCD_putc(4, TH);
	LCD_putc(5, TL);
	LCD_putc(6, '\0');
    }
}


/*****************************************************************************
*
*   Function name : TemperatureFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable or disable temperature measurements
*
*****************************************************************************/
//char TemperatureFunc(char input)
//{
//    static char enter = 1;
//    
//    if (enter)
//    {
//        enter = 0;
//        
//        ADC_init(TEMPERATURE_SENSOR);       // Init the ADC
//
//        // Enable auto-run of the ADC_perphery every 10ms 
//        // (it will actually be more than 10ms cause of the SLEEP)
//        Timer0_RegisterCallbackFunction(ADC_periphery); 
//    }
//    else
//        LCD_UpdateRequired(TRUE, 0);        // New data to be presented
//    
//    if (input == KEY_PREV)
//    {
//        // Disable the auto-run of the ADC_periphery
//        Timer0_RemoveCallbackFunction(ADC_periphery);
//        
//        enter = 1;  // Set enter to 1 before leaving the TemperatureFunc
//        
//        return ST_TEMPERATURE;
//    }/*
//    else if (input == KEY_PLUS)
//    {   
//        if (degree == FAHRENHEIT)
//            degree = CELCIUS;
//        else
//            degree = FAHRENHEIT;
//    }
//    else if (input == KEY_MINUS)
//    {
//        if (degree == FAHRENHEIT)
//            degree = CELCIUS;
//        else
//            degree = FAHRENHEIT;
//    }
//     else if (input == KEY_ENTER)
//     {
//	 if (gTempOffset)
//	     gTempOffset=0;
//	 else
//	     gTempOffset=ADC_read()-806;
//     }
//     */
//    
//    return ST_TEMPERATURE_FUNC;        
//}


/*****************************************************************************
*
*   Function name : VoltageFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable or disable voltage measurements
*
*****************************************************************************/
//char VoltageFunc(char input)
//{
//    static char enter = 1;
//    
//    if (enter)
//    {
//        enter = 0;
//
//        ADC_init(VOLTAGE_SENSOR);       // Init the ADC
//        
//        // Enable auto-run of the ADC_perphery every 10ms 
//        // (it will actually be more than 10ms cause of the SLEEP)        
//        Timer0_RegisterCallbackFunction(ADC_periphery);        
//    }
//    else
//        LCD_UpdateRequired(TRUE, 0); 
//
//    if (input == KEY_PREV)
//    {
//        // Disable the auto-run of the ADC_periphery        
//        Timer0_RemoveCallbackFunction(ADC_periphery);
//        
//        enter = 1;  // Set enter to 1 before leaving the TemperatureFunc
//        
//        return ST_VOLTAGE;
//    }
//    else
//        return ST_VOLTAGE_FUNC;    
//}    



/*****************************************************************************
*
*   Function name : DirectionFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable or disable voltage measurements
*
*****************************************************************************/
//
//char DirectionFunc(char input)
//{
//    static char enter = 1;
//    
//    if (enter)
//    {
//        enter = 0;
//	
//        ADC_init(DIR_SENSOR);       // Init the ADC
//        
//        // Enable auto-run of the ADC_perphery every 10ms 
//        // (it will actually be more than 10ms cause of the SLEEP)        
//        Timer0_RegisterCallbackFunction(ADC_periphery);        
//    }
//    else
//        LCD_UpdateRequired(TRUE, 0); 
//    
//    if (input == KEY_PREV)
//    {
//        // Disable the auto-run of the ADC_periphery        
//        Timer0_RemoveCallbackFunction(ADC_periphery);
//        
//        enter = 1;  // Set enter to 1 before leaving the TemperatureFunc
//        
//        return ST_DIR;
//    }
//    else
//        return ST_DIR_FUNC;    
//}    


/*****************************************************************************
*
*   Function name : LightFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable or disable light measurements
*
*****************************************************************************/
//char LightFunc(char input)
//{
//    static char enter = 1;
//    
//    if (enter)
//    {
//    
//        enter = 0;
//        
//        ADC_init(LIGHT_SENSOR);     // Init the ADC
//        
//        // Enable auto-run of the ADC_perphery every 10ms 
//        // (it will actually be more than 10ms cause of the SLEEP)  
//        Timer0_RegisterCallbackFunction(ADC_periphery);        
//    }
//    else
//        LCD_UpdateRequired(TRUE, 0); 
//
//    if (input == KEY_PREV)
//    {
//        // Disable the auto-run of the ADC_periphery      
//        Timer0_RemoveCallbackFunction(ADC_periphery);
//        
//        enter = 1;  // Set enter to 1 before leaving the TemperatureFunc
//    
//        return ST_LIGHT;
//    }
//    else
//        return ST_LIGHT_FUNC;    
//}    
