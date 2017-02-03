/**
************************************************************************

  @file		eeprom.c
  @author	ATMEL Norway
  @brief	AVR Butterfly EEPROM routines
 
 $Id: eeprom.c,v 1.8 2007/06/04 15:12:17 brokentoaster Exp $
 
 </pre>
  Target	ATmega169
  Compiler....: AVR-GCC 3.3.1; avr-libc 1.0
  Revisions...: 1.0


  YYYYMMDD - VER. - COMMENT                                       - SIGN.
  20030116 - 1.0  - Created                                       - LHM
  20031009          port to avr-gcc/avr-libc                      - M.Thomas
 </pre>
 
*************************************************************************
**/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "types.h"
#include "eeprom.h"
// include workaround for ATmega169 and avr-libc 1.0
#include "eeprom169.h"
#include "main.h"
#include "uart.h"

// player variables from main.c
extern uint8 	gmusic_vol;		// music volume.
extern uint8 	gbass_boost;		// bass boost off.
extern uint8 	gMode;		// mode of player, idle, playing, off, etc
extern uint32 	gFAT_entry; 	// global current entry number in FAT table. (aka file number)
extern uint32 	gCluster; 			// current sector (cluster) in song
extern uint32	gFileSectorsPlayed; // number of full sectors played. (aka position)
extern volatile uint16	gPlayTimeSeconds;	// # of seconds current file has been playing.


/*****************************************************************************
*
*   Function name : StoreEEPROM
*
*   Returns :       None
*
*   Parameters :    Pointer to string, number of bytes to write, adress in EEPROM
*
*   Purpose :       Write byte(s) to the EEPROM
*
*****************************************************************************/
void StoreEEPROM(uint8 *pBuffer, uint8 num_bytes, unsigned int EE_START_ADR)
{
    unsigned char i;
    for (i=0;i<num_bytes;i++) {
        // mt __EEPUT(EEPROM_START_ADR++, pBuffer[i]); // Store parameters
		eeprom_write_byte_169(&EE_START_ADR, pBuffer[i]);
		EE_START_ADR++;
	}
}


/*****************************************************************************
*
*   Function name : ReadEEPROM
*
*   Returns :       None
*
*   Parameters :    Pointer to string, number of bytes to read, adress in EEPROM
*
*   Purpose :       Read byte(s) to the EEPROM
*
*****************************************************************************/
void LoadEEPROM(uint8 *pBuffer, uint8 num_bytes, unsigned int EE_START_ADR)
{
    unsigned char i;
    for (i=0;i<num_bytes;i++) {
        // mt __EEGET(pBuffer[i], EEPROM_START_ADR++); // Load parameters
		pBuffer[i]=eeprom_read_byte_169(&EE_START_ADR);
		EE_START_ADR++;
	}
}

/*****************************************************************************
*
*   Function name : ReadEEPROM
*
*   Returns :       byte read from EEPROM
*
*   Parameters :    adress in EEPROM
*
*   Purpose :       Read a single byte from the EEPROM
*
*****************************************************************************/
uint8 ReadEEPROM(unsigned int EE_START_ADR)
{
	return eeprom_read_byte_169(&EE_START_ADR);
}



/**
*	findCurrentEepromAddr
 *
 *	@params	index_buffer uint16 pointing to  buffer index of interest
 *	@returns	index to last used entry in buffer
 *
 *	0Finds the last used entry in a ring buffer
 **/
uint8 findCurrentEepromAddr(uint16 index_buffer )
{
#ifdef USE_EEPROM
#ifdef ENDURANCE_EEPROM
	// Modified from AVR application note 
	// AVR101: High Endurance EEPROM Storage
	uint16 temp;
	uint16 EeBufPtr;
	uint16 EeBufEnd;
	
	EeBufPtr = index_buffer ;              // Point the the status buffer
	EeBufEnd = EeBufPtr + EEPROM_BFFRSZE;   // The first address outside the buffer
	
	/* Identify the last writen element of the status buffer */
	do{
		if( EeBufPtr == EeBufEnd )
			break;
		temp = ReadEEPROM( EeBufPtr++ );
	}while ( ReadEEPROM( EeBufPtr ) == temp +1 );
	
	EeBufPtr -=  (index_buffer + 1);       // offset to the last used element of the parameter buffer
	return (uint8) EeBufPtr;
#else
	return 0;
#endif
#endif

}


/**
 *	save_player_options
 *
 *	Save current state of player to eeprom
 **/
void save_player_options(void)
{
#ifdef USE_EEPROM
	uint8 tmp;
	uint8 index=0;
	uint8 oldIndexVal;
	
#ifdef ENDURANCE_EEPROM	
	/* Store the old status value and move pointer to the next element in the buffer */
	index= findCurrentEepromAddr(EEPROM_OPTIONS_INDEX);
	//PRINT("INDEX =");UART_Printfu08(index);EOL();
	
	oldIndexVal = ReadEEPROM(EEPROM_OPTIONS_INDEX+index);
	//PRINT("VALUE =");UART_Printfu08(oldIndexVal);
	if (oldIndexVal == 0xFF) 
		index--; //workaround for odd bug at 255 writes 
#endif	
	
	//load saved options
	LoadEEPROM(&tmp,1,EEPROM_VOL+index);
	
	// compare saved to current options
	if (tmp!=gmusic_vol){ // save vol if changed 
		
#ifdef ENDURANCE_EEPROM	
		// Move to  next available position in buffer
		index++;
		if (index == EEPROM_BFFRSZE){
			index =0;
		}
#endif		
		
		// write options to eeprom
		StoreEEPROM(&gmusic_vol,1,EEPROM_VOL+index);
		
#ifdef ENDURANCE_EEPROM			
		// write new buffer index
		oldIndexVal++;
		StoreEEPROM(&oldIndexVal,1,EEPROM_OPTIONS_INDEX+index);
		//UART_Printfu08(oldIndexVal); UART_Printfu08(index); EOL();
#endif
		
		//PRINT("vol saved as ");	UART_Printfu08(gmusic_vol);EOL();
	}// else leave eeprom unchanged.
#endif
}


/**
 *	save_player_state
 *
 *	Save current state of player to eeprom
 **/
void save_player_state(void)
{
#ifdef USE_EEPROM
	uint8 index=0;
	uint8 oldIndexVal;
	
#ifdef ENDURANCE_EEPROM	
	/* Store the old status value and move pointer to the next element in the buffer */
	index= findCurrentEepromAddr(EEPROM_STATE_INDEX);
	//PRINT("INDEX =");UART_Printfu08(index);EOL();
	
	oldIndexVal = ReadEEPROM(EEPROM_STATE_INDEX+index);
	//PRINT("VALUE =");UART_Printfu08(oldIndexVal);
	if (oldIndexVal == 0xFF) 
		index--; //workaround for odd bug at 255 writes 
	
	// Move to  next available position in buffer
	index++;
	if (index == EEPROM_BFFRSZE){
		index =0;
	}
#endif
	
	// Store player state
	StoreEEPROM(&gMode,1,EEPROM_MODE+index);
	StoreEEPROM((uint8 *)&gFAT_entry,4,EEPROM_FILE+index);
	StoreEEPROM((uint8 *)&gCluster,4,EEPROM_SECTOR+index);
	StoreEEPROM((uint8 *)&gFileSectorsPlayed,4,EEPROM_PLAYED+index);
	StoreEEPROM((uint8 *)&gPlayTimeSeconds,2,EEPROM_TIME+index);
	
#ifdef ENDURANCE_EEPROM	
	// write new buffer index
	oldIndexVal++;
	StoreEEPROM(&oldIndexVal,1,EEPROM_STATE_INDEX+index);
#endif
	
	// Debug data about buffer contents
//	PRINT("saving to eeprom");EOL();
//	UART_Printfu08(gMode);EOL();
//	UART_Printfu32(gFAT_entry);EOL();
//	UART_Printfu16(gPlayTimeSeconds);EOL();
//	UART_Printfu32(gCluster);EOL();
//	UART_Printfu32(gFileSectorsPlayed);EOL();
#endif
}


/**
 *	load_player_options
 *
 *	load current state of player from eeprom
 **/
void load_player_options(void)
{
#ifdef USE_EEPROM
	uint8 index=0;
	
#ifdef ENDURANCE_EEPROM	
	// get position in ring buffer
	index= findCurrentEepromAddr(EEPROM_OPTIONS_INDEX);
	//PRINT("INDEX =");UART_Printfu08(index);EOL();
#endif
	
	// get options (volume only at this stage
	LoadEEPROM(&gmusic_vol,1,EEPROM_VOL+index);
	if (gmusic_vol>9)
		gmusic_vol = 5; // set to default if invalid
	
	//PRINT("vol Loaded as ");UART_Printfu08(gmusic_vol);EOL();
#endif
}


/**
 *	load_player_state
 *
 *	load current state of player from eeprom
 *	
 **/
void load_player_state(uint8 *mode, uint32 *file, uint32 *sector, uint32 *played,uint16 *time)
{
#ifdef USE_EEPROM
	uint8 index=0;
	
#ifdef ENDURANCE_EEPROM	
	// get position in ring buffer
	index= findCurrentEepromAddr(EEPROM_STATE_INDEX);
	//PRINT("INDEX =");UART_Printfu08(index);EOL();	
#endif
	
	// try to find last song played
	LoadEEPROM(mode,1,EEPROM_MODE+index);
	LoadEEPROM((uint8 *)file,4,EEPROM_FILE+index);
	LoadEEPROM((uint8 *)sector,4,EEPROM_SECTOR+index);
	LoadEEPROM((uint8 *)played,4,EEPROM_PLAYED+index);
	LoadEEPROM((uint8 *)time,2,EEPROM_TIME+index);
#endif
}


