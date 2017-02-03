/**
***************************************************************************

  @file		eeprom.h
  @author		ATMEL Norway
  @brief		AVR Butterfly EEPROM routines
  Target		ATmega169

  Compiler....: AVR-GCC 3.3.1; avr-libc 1.0

  Revisions...: 1.0
  $Id: eeprom.h,v 1.5 2007/06/04 15:12:18 brokentoaster Exp $

  YYYYMMDD - VER. - COMMENT                                       - SIGN.

  20030116 - 1.0  - Created                                       - LHM
  20031009          port to avr-gcc/avr-libc                      - M.Thomas

***************************************************************************
**/
#ifndef _eeprom_h_
#define _eeprom_h_

// mt - moved to vcard.h
//#define EEPROM_START 0x100 //the name will be put at this adress


void LoadEEPROM(uint8 *pBuffer, uint8 num_bytes, unsigned int EE_START_ADR);
void StoreEEPROM(uint8 *pBuffer, uint8 num_bytes, unsigned int EE_START_ADR);
uint8 ReadEEPROM(unsigned int EE_START_ADR);

void	save_player_state(void);
void	save_player_options(void);
void	load_player_state(uint8 *mode, uint32 *file,
						  uint32 *sector, uint32 *played, uint16 *time);
void	load_player_options(void);
uint8	findCurrentEepromAddr(uint16 index_buffer );

#endif
