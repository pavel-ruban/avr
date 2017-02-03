/**
	@file	main.c
	@brief	Butterfly MP3 main
	@author	Nick Lott 
	@date	November 2004
 
	$Id: main.c,v 1.53 2007/06/04 15:12:17 brokentoaster Exp $

**/

/** @mainpage
Copyright (C) 2004 Nick Lott <brokentoaster@users.sf.net>
http://butterflymp3.sf.net/

This is a simple MP3 player based around the AVR Butterfly.It currently 
uses a VS1001 mp3 decoder, MMC card and Nokia LCD. It has been heavily 
influenced by the Yampp system by Jesper Hansen <jesperh@telia.com>.

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
#include "main.h"
#include "types.h"
#include "uart.h"
#include "delay.h"
#include "vs1001.h"
#include "mmc.h"
#include "avrfat16.h"
#include "timer0.h"
#include "button.h"
#include "eeprom.h"
#include "lcdgraphics.h"
#include "power.h"
#include "utils.h"

//#include "testing.h"

/// tag rom with verion info
const char  VERSIONINFO1[] PROGMEM = "ButterflyMP3_sf_net V0.6 ";	
/// tag rom with compile date and time
const char  VERSIONINFO2[] PROGMEM = __DATE__ __TIME__ ;


#ifdef NOKIA
extern	uint8 fbXY[];		///< graphical LCD position array
#endif

// player option variables
uint8 	gmusic_vol	=5;	///< music volume.
uint8 	gbass_boost	=0;	///< bass boost off.

// player state variables
uint8 	gMode		=0;	///< mode of player, idle, playing, off, etc
BOOL 	gLCD_Refresh;		///< Update LCD
// @TODO: move these into file struct
uint32 	gFAT_entry 	=0; 	///< global current entry number in FAT table. (aka file number)
uint32 	gCluster; 		///< current  cluster in song
uint32 	gFileSectorSize; 	///< size of current file in complete sectors.
uint32	gFileSectorsPlayed;	///< number of full sectors played. (aka position)
BOOL   	gFile_good; 		///< File has been read from Fat and ready to go.
char	cluster_pos=0;		///< position in cluster 
char	buff_pos=16;		///< position in buffer

//volatile uint16 gBatteryVoltage;	///< last ADC battery Voltage reading

volatile uint16	gPlayTimeSeconds; ///< # of seconds current file has been playing.
uint8	gDisplayTimeOut=0;	  ///< # of seconds+1 before changing display;
	
///  setup volume levels  0	1	2	3	4	5	6	7	8	9
const uint8	__attribute__ ((progmem)) volume_val[10] = {160,128,112, 96, 80, 64, 48, 32, 16, 0};

// internal Function Prototypes
//uint8	FAT_readRoot(uint32 filenumber);
void 	flush_decoder(char reset_type);
void 	cue_file(void);
void 	streaming (void);
void 	handle_interface(void);
void 	update_LCD(void);
void	mainTimer(void);
uint8 	init_mmc_fat(void);
uint8	handle_mmc(uint8 MMC_present);

/* ..Removed to save space.
void DumpGlobals(void)
{
#ifdef ALPHA
	extern volatile signed char gScroll;
	extern volatile char gScrollMode;
#endif
	
	UART_Printfu08(gmusic_vol);EOL();
	UART_Printfu08(gbass_boost);EOL();
	UART_Printfu08(gMode);EOL();
	UART_Printfu08(gLCD_Refresh);EOL();
	UART_Printfu08(gFile_good);EOL();
	UART_Printfu08(gDisplayTimeOut);EOL();
#ifdef ALPHA
	UART_Printfu08(gScroll);EOL();
	UART_Printfu08(gScrollMode);EOL();
#endif
	UART_Printfu16(gPlayTimeSeconds);EOL();
	
	UART_Printfu32(gFAT_entry);EOL();
	UART_Printfu32(gCluster);EOL();
	UART_Printfu32(gFileSectorSize);EOL();
	UART_Printfu32(gFileSectorsPlayed);EOL();
}
*/


/**
*	init_HW
*
*	Initialize basic hardware for the butterfly.
**/
void 	init_HW(void)
{
	// boost IntRC to 8Mhz 
	//OSCCAL_calibration();
	CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
	CLKPR = (0<<CLKPS1)|(0<<CLKPS0); // set prescaler = 1, Internal RC 8Mhz   

	// Disable Analog Comparator (power save)
	ACSR = (1<<ACD);

	// Disable Analog to Digital converter (power save)
	//ADCSRA = 0;

	// Disable Digital input on PF0-7 (power save)
	DIDR1 = 0xFF; 

#ifdef PWR_CHK_INTERVAL
	Power_Init();		// init power monitoring 
	// abort power up if battery is low.
	if (Power_check()<PWR_MIN_V) { 
		//Jump to Bootloader
		WDTCR = (1<<WDCE) | (1<<WDE);     //Enable Watchdog Timer to give reset
		while(1);   
	}
#endif
	
	// setup IO
	UART_Init();		// init RS-232 link
	/* enable interrupts */
	sei();
	SpiInit();		// init spi pins (mmc & VS1001)
	vs1001_init_io();	// init IO pins
	vs1001_init_chip();	// Reset Chip
	Timer0_Init();		// setup timers for debounce and scroll
	Button_Init(); 		// setup joystick.
	LCD_Initialize();	// SETUP lcd
	
	//register  timer routine 
	Timer0_RegisterCallbackFunction(mainTimer);
	
	// Read settings (volume and stuff from eeprom or flash)
	load_player_options();
	vs_1001_setvolume(pgm_read_byte(volume_val+gmusic_vol), pgm_read_byte(volume_val+gmusic_vol));
	

	
	// say hello on Serial 
  	PRINT("AVR ButterflyMP3 Player  "__DATE__" "__TIME__);EOL();	
	
	// set up the FAT variables
	FAT_buffer = mmc_sbuf; 	// 512 byte buffer for sector reads/writes
	FAT_scratch = mmc_scratch; 
	FAT_read = MMC_Read; // pointer to read block function 
	//FAT_write = MMC_Write;// pointer to write block function 
	FAT16_entryMAX =-1;
	FAT16_entryMIN = 0;
} 



void mainTimer(void){
	static uint8 timeout_count=0; 
	static uint8 second_timer=100;
#ifdef PWR_CHK_INTERVAL
	static uint8 power_timer=PWR_CHK_INTERVAL;
	uint16 batteryVoltage;
#endif
	extern unsigned char gButtonTimeout;  	// Variable from "button.c" to prevent button-bouncing

	
	
	
	/**************** Button timeout for the button.c, START ****************/
    if(!gButtonTimeout)
    {
        timeout_count++;
        
        if(timeout_count > 20) 
        {
            gButtonTimeout = TRUE;
            timeout_count = 0;
			//long_timeout_count=0;
        }
		
    }


   // RA Sewell - other half a second
   if (second_timer == 50)
   {
      gLCD_Refresh = TRUE;
   }
   
	if (!second_timer--){/// do stuff every second in here
		second_timer=100;
		if (gMode != MODE_PLAY){
			gPowerSaveTimer++;
			//UART_Printfu08(gPowerSaveTimer);EOL();
		}else{
			gPlayTimeSeconds++;			
		}
      gLCD_Refresh = TRUE;
#ifdef PWR_CHK_INTERVAL
		if ((power_timer--)==0){
			power_timer = PWR_CHK_INTERVAL;
			batteryVoltage = Power_check();
			if (batteryVoltage>PWR_GOOD_V){
				//PRINT("Battery GOOD");
				//EOL();
			}else if (batteryVoltage>PWR_LOW_V){
				//PRINT("Battery OK");
				//EOL();
			}else if (batteryVoltage>PWR_MIN_V){
				//PRINT("Battery LOW");
				//EOL();
			}else if (batteryVoltage>PWR_ABS_MIN_V){
				//PRINT("Battery FLAT");
				//EOL();
			}else {
				PRINT("LOW BTRY PWR DWN");
				EOL();
				// stop playing and power down
				gMode = MODE_PAUSE;
				// save everything to eeprom
				//save current settings
				save_player_options();
				
				//TODO: Write MMC Serial Number to EEPROM
				
				//write current song to EEPROM
				save_player_state();
								
				gPowerSaveTimer=AUTOPOWEROFF_TIMEOUT;
			}
		}
#endif
		if(gDisplayTimeOut){
			if (gDisplayTimeOut==1){
				#ifdef ALPHA
				// Display back to normal
				LCD_FileName(TRUE);
				#endif	
				gDisplayTimeOut = 0;
			}else{ 
				// continue the count down
				gDisplayTimeOut--;
			}
		}
		
	}
	/*
	 if(!gButtonLongTimeout){ // start the hold down timer
		 long_timeout_count++;
		 
		 if(long_timeout_count > 1000){
			 gButtonLongTimeout = TRUE;
			 KEY_SHIFT=TRUE;
			 KEY_VALID=TRUE;
			 timeout_count = 0;
			 long_timeout_count=0;
		 }
		 
	 }*/
	
	/**************** Button timeout for the button.c, END ******************/
}



/**
*	restore_player
*	@return		MMC Card Status
*
*	Restore the player state from eeprom if possible
**/
uint8 restore_player(void)
{
	uint8	MMC_present,mode;
	uint16	time;
	uint32	file,sector,played;
	
	mode=0; file=0;	sector=0;	played=0;	time=0;

	// check for an mmc on startup
	MMC_present = handle_mmc(FALSE);
	
	if (!MMC_present){ //Can't restore no MMC  
		//PRINT("NO MMC. Rstre abortd");EOL();
		return MMC_present; 
	}
	
	// read last song details from EEPROM
	load_player_state(&mode,&file,&sector,&played,&time);

		// Debug loading info
//		PRINT("Loaded from EEPROM");EOL();
//		UART_Printfu08(mode);EOL();
//		UART_Printfu32(file);EOL();
//		UART_Printfu16(time);EOL();
//		UART_Printfu32(sector);EOL();
//		UART_Printfu32(played);EOL();
	
	if ((mode != MODE_PAUSE)&&( mode != MODE_IDLE)){ // nothing to restore
		//PRINT("MODE != PAUSE");EOL();
		return MMC_present;
	}
	
	// todo: check if mmc is same mmc
	
	// check song is available
	if ( FAT_getNextSong(file-1,FAT16_dir_first_sector) == file){ // song found
		gFAT_entry = file; // select file
		cue_file(); // load the file into memory
		gMode = mode;
		gCluster = sector; // move to last played position
		gFileSectorsPlayed = played;
		gPlayTimeSeconds = time;
		gLCD_Refresh=TRUE;
	}else{ // song not found
		gFAT_entry =FAT16_entryMIN;
	}
	return MMC_present;
}

uint8 open_Dir(uint32 dir_lba){
	uint8 files =0;
	
	// display mode on LCD
	LCD_Scanning();
	
	files = FAT_scanDir_lba(dir_lba);
	
	gFAT_entry= FAT16_entryMIN;
	cue_file(); // load the file into memory
	
	// Display back to normal
	LCD_FileName(TRUE);
	
	//PRINT("1ST FLE @ ");UART_Printfu16(FAT16_entryMIN);
	//PRINT("  LST FLE @ ");UART_Printfu16(FAT16_entryMAX);EOL();
	//UART_Printfu08(files);PRINT("files");EOL();
	
	return files;
}

/**
*   init_mmc_fat
*	@return		error from MMC or fat initialization or 0 for everything okay
*
*	initialize the mmc and read fat/partition details from the disk.
*	Needs to be run everytime MMC is changed.
**/
uint8 	init_mmc_fat(void)
{ 
	uint8 result;
	
	//DumpGlobals();
	gFAT_entry = 0;
	FAT16_entryMIN = 0;
	FAT16_entryMAX = -1;
	
	result =  MMC_Reset();				// init mmc
	if (result) return result;
	
	result = FAT_initFat16();
	if (result) return result; // abort on non-zero reply 
	
	UART_Printfu32(FAT16_root_dir_first_sector);EOL();
	
	result = open_Dir(FAT16_root_dir_first_sector);
	
	//return 0 if files found, 1 if 0 files
	return !result;
}



/**
*	flush_decoder
*
*	Resets the decoder and prepares it for a new stream of 
*	audio data. all setup params must be resent.
**/
void 	flush_decoder(char reset_type)
{
	uint16 buf[2];
	
	// reset VS1001 and set clock doubler
	vs1001_reset(reset_type);

	// send VS1001 settings
	vs_1001_setvolume(pgm_read_byte(volume_val+gmusic_vol), pgm_read_byte(volume_val+gmusic_vol));
	if (gbass_boost){
		vs1001_read(0, 1, buf);
		buf[1] |= SM_BASS;
		buf[1] &= ~SM_MP12;
		vs1001_write(0,1,buf);
	}else{
		vs1001_read(0, 1, buf);
		buf[1] &= ~SM_BASS;
		buf[1] &= ~SM_MP12;
		vs1001_write(0,1,buf);
	}

}
    

/**
*	cue_file			
*
*	Gets file info for file pointed to by gFAT_entry
*	and inits MP3 chip. Checks for MMC card as well.
*	Reads ID3v2 tag for title and artist
*	If successful then gFile_good is set
**/
void 	cue_file(void)
{
	uint8 check;
//	uint8 temp;
	uint32 ID3index;
	uint8 i;
	uint16 j;
	uint32 offset;
	uint16 ID3_Bytes;
	uint32 ID3_clusters;
	uint16 ID3_sectors;
	uint32 nextCluster=0;
	
	
	// read file data and check gFAT_entry is aValid file entry.
	check = FAT_readFile(gFAT_entry,FAT16_dir_first_sector);
	if (check) return;
	
	if (FAT16_filetype & FILE_TYPE_DIR) return;
/*
	//skip or recurse Directories
	if (FAT16_filetype & FILE_TYPE_DIR){
		if (RECURSE_DIRECTORIES){
			// scan a new directory....
			// get cluster address from fat entry
			gCluster = FAT_scratch[13];
			gCluster <<= 8;
			gCluster += FAT_scratch[14];
			gCluster <<= 8;
			gCluster += FAT_scratch[15];
			gCluster <<= 8;
			gCluster += FAT_scratch[16];
			
			// check for files and exit on no files(always 2 entries . and ..)
			//if (open_Dir(gCluster)<3) {
			//	return;
			//}
			
			nextCluster = FAT_readFile(FAT16_entryMIN,FAT16_dir_first_sector);
			
		}else{
			if (PLAY_NEXT_SONG){ // cue up next song and play it.
				nextCluster = FAT_getNextSong(gFAT_entry,FAT16_dir_first_sector);
				if(nextCluster){// play next song
					gFAT_entry = nextCluster;
					gLCD_Refresh=TRUE;
					gMode = MODE_PLAY;
				}else if(PLAY_REPEAT_ALL){
					gFAT_entry = FAT16_entryMIN;
					gLCD_Refresh=TRUE;
					gMode = MODE_PLAY;
				}
			}
		}
	}
	*/
	nextCluster=0;
	
	// get first Cluster 
	FAT_Scratch2Cluster();
//	gCluster = FAT_scratch[13];
//	gCluster <<= 8;
//	gCluster += FAT_scratch[14];
//	gCluster <<= 8;
//	gCluster += FAT_scratch[15];
//	gCluster <<= 8;
//	gCluster += FAT_scratch[16];

	//get fileSize in whole sectors
	/*
	*((char *)&gFileSectorSize+2) = FAT_scratch[17];
	*((char *)&gFileSectorSize+1)= FAT_scratch[18];
	*((char *)&gFileSectorSize+0) = FAT_scratch[19];

*/
	
	gFileSectorSize += FAT_scratch[17];
	gFileSectorSize <<= 8;
	gFileSectorSize += FAT_scratch[18];
	gFileSectorSize <<= 8;
	gFileSectorSize += FAT_scratch[19];

	cluster_pos = 0;
	buff_pos = 0;
	FAT_readCluster(gCluster,0);
	
	gFile_good = TRUE;
	gFileSectorsPlayed = 0;
	
	// Detect and Skip ID3 Info at start of file
	// (we ignore tags at end of file)
	
	// Version 1.x 
	if (FAT_buffer[0] == 'T' && FAT_buffer[1] == 'A' && FAT_buffer[2]=='G') {
		//PRINT("ID3 v1 TAG");EOL();
		// jump to byte 128
		buff_pos = 4; 
	}else
		
	// Version 2.x - now with very basic interpretation!
	if (FAT_buffer[0] == 'I' && FAT_buffer[1] == 'D' 
			  && FAT_buffer[2]=='3'){ 
		//PRINT("ID3 v2.");
		//UART_Printfu08(FAT_buffer[5]);
		//UART_Printfu08(FAT_buffer[4]);
		//PRINT(" TAG");

      
		/*
		An ID3v2 tag can be detected with the following pattern:
		$49 44 33 yy yy xx zz zz zz zz
		Where yy is less than $FF, xx is the 'flags' byte and zz is less than $80.
		z = ID3 tag length.
		 */
		
		offset = 0;
		for(i=6;i<10;i++){
			offset <<= 7; // only shift by 7 as MSB is unused.
			offset += FAT_buffer[i];
		}



      
		offset += 10; //include length of header
		//UART_Printfu32(offset);PRINT("B");EOL();

      // offset is now equal to the the length of the TAG
      ID3index = 10; // skip header


      switch (FAT_buffer[3])
      {
         case 2:
            // Search for song / artist frame and replace long filename with them
            while ((ID3index < offset) && (ID3index < 512))
            {
               if (FAT_buffer[ID3index] == 'T' &&
                   FAT_buffer[ID3index+1] == 'T' &&
                   FAT_buffer[ID3index+2] == '2')
               {
                  //PRINT(" SONG");          

                  // For some reason, all strings have a Null at the front of the 
                  // name - I think this indicates the encoding type - we'll assume
                  // its always 00, string! (this is what the -1 is for)
                  // other -1 is for the null on the end of the string
                  for (i=0; (i<FAT_buffer[ID3index+5]-1) && (i<LFN_TITLE_LENGTH-1); i++)
                  {
                     FAT16_longfilename[LFN_TITLE_INDEX+i] = FAT_buffer[ID3index+7+i]; 
                  }
                  FAT16_longfilename[LFN_TITLE_INDEX+i] = '\0';

                  // Recalculate string length / add a blank
                  FAT16_LFNTitleLen = strLen(FAT16_longfilename+LFN_TITLE_INDEX);

                  // Indicate TITLE found
                  FAT16_longfilename[LFN_TYPE_INDEX] |= LFN_TYPE_ID3_TITLE;
               }
               else if (FAT_buffer[ID3index] == 'T' &&
                        FAT_buffer[ID3index+1] == 'P' &&
                        FAT_buffer[ID3index+2] == '1')
               {
                  //PRINT(" ARTIST");
                  for (i=0; (i<FAT_buffer[ID3index+5]-1) && (i<LFN_ARTIST_LENGTH-1); i++)
                  {
                     FAT16_longfilename[LFN_ARTIST_INDEX+i] = FAT_buffer[ID3index+7+i]; 
                  }
                  FAT16_longfilename[LFN_ARTIST_INDEX+i] = '\0';

                  // Recalculate string length / add a blank
                  FAT16_LFNArtistLen = strLen(FAT16_longfilename+LFN_ARTIST_INDEX);

                  // Indicate ARTIST found
                  FAT16_longfilename[LFN_TYPE_INDEX] |= LFN_TYPE_ID3_ARTIST;
               }
               
               // Skip to next ID3 'frame'
               ID3index += FAT_buffer[ID3index+5] + 6;
               //PRINT(" I:");
               //UART_Printfu08(ID3index);
            }


            break;
            
         case 3:
         case 4:

            // Search for song / artist frame and replace long filename with them
            while ((ID3index < offset) && (ID3index < 512))
            {
               if (FAT_buffer[ID3index] == 'T' &&
                   FAT_buffer[ID3index+1] == 'I' &&
                   FAT_buffer[ID3index+2] == 'T' &&
                   FAT_buffer[ID3index+3] == '2')
               {
                  //PRINT(" SONG");          

                  // For some reason, all strings have a Null at the front of the 
                  // name - I think this indicates the encoding type - we'll assume
                  // its always 00, string! (this is what the -1 is for)
                  // other -1 is for the null on the end of the string
                  for (i=0; (i<FAT_buffer[ID3index+7]-1) && (i<LFN_TITLE_LENGTH-1); i++)
                  {
                     FAT16_longfilename[LFN_TITLE_INDEX+i] = FAT_buffer[ID3index+11+i]; 
                  }
                  FAT16_longfilename[LFN_TITLE_INDEX+i] = '\0';

                  // Recalculate string length / add a blank
                  FAT16_LFNTitleLen = strLen(FAT16_longfilename+LFN_TITLE_INDEX);

                  // Indicate TITLE found
                  FAT16_longfilename[LFN_TYPE_INDEX] |= LFN_TYPE_ID3_TITLE;
               }
               else if (FAT_buffer[ID3index] == 'T' &&
                        FAT_buffer[ID3index+1] == 'P' &&
                        FAT_buffer[ID3index+2] == 'E' &&
                        FAT_buffer[ID3index+3] == '1')
               {
                  //PRINT(" ARTIST");
                  for (i=0; (i<FAT_buffer[ID3index+7]-1) && (i<LFN_ARTIST_LENGTH-1); i++)
                  {
                     FAT16_longfilename[LFN_ARTIST_INDEX+i] = FAT_buffer[ID3index+11+i]; 
                  }
                  FAT16_longfilename[LFN_ARTIST_INDEX+i] = '\0';

                  // Recalculate string length / add a blank
                  FAT16_LFNArtistLen = strLen(FAT16_longfilename+LFN_ARTIST_INDEX);

                  // Indicate ARTIST found
                  FAT16_longfilename[LFN_TYPE_INDEX] |= LFN_TYPE_ID3_ARTIST;
               }
               
               // Skip to next ID3 'frame'
               ID3index += FAT_buffer[ID3index+7] + 10;
               //PRINT(" I:");
               //UART_Printfu08(ID3index);
            }


            break;
         default:
            ; // Unknown version, do nothing
      }

		// skip through to end of ID3 
		ID3_clusters = (offset/512) / FAT16_sectors_per_cluster;
		ID3_sectors = (offset/512) % FAT16_sectors_per_cluster;
		ID3_Bytes = offset % 512;
		
		//UART_Printfu32(ID3_clusters);EOL();
		//UART_Printfu16(ID3_sectors);EOL();
		//UART_Printfu16(ID3_Bytes);EOL();
		
		// find first cluster of actual song
		for (j=0;j<ID3_clusters;j++){
			
			nextCluster = FAT_NextCluster(gCluster);
		//	UART_Printfu16(j);UART_SendByte(0x09);
		//	UART_Printfu32(nextCluster);EOL();
			if (nextCluster == 0xffffffff){
				gFile_good = FALSE;
				PRINT("EOF: ID3 Tag @ Clstr ");
				UART_Printfu32(gCluster);
				EOL();
				return;
			}
			gCluster = nextCluster;
		}

		
		// Adjust variables for new position.
		gFileSectorsPlayed = (offset / 512);
		buff_pos = (ID3_Bytes/32);
		cluster_pos = ID3_sectors;

		/*
		while(FAT_readCluster(gCluster,cluster_pos)){
			MMC_Reset();	
		}
		*/	
		
		/*PRINT("ID3 Tag skipped. First ten bytes of MP3 stream are ... ");
		for (i=0;i<10;i++){
			UART_Printfu08(FAT_buffer[i+ID3_Bytes]);
		}
		EOL();
		*/
		j=ID3_Bytes % 32;
		if (j){//we need to play(skip;) some bytes
			buff_pos++; // skips to next 32 byte chunk of buffer
		}
	}else{ //dump  first 10 bytes of file
		/*
		for (i=0;i<10;i++){
			UART_Printfu08(FAT_buffer[i]);
		}
		EOL();
		 */
	}
	
	// send params to vs1001k
	flush_decoder(SOFT_RESET); 	
	
	//reset play timer...
	gPlayTimeSeconds = 0;
	return;
}


/**
*	Data Streaming
*
*	Take care of feeding data to mp3 chip and reading 
*	mmc in free time. AKA playing.
**/   
void 	streaming (void)
{
//	uint16 c;
	uint8	abort=0;
//	uint16 tmp=0;
//	uint16 state[2];
	uint32 nextfile;

	
	if (!gFile_good) return; // only run if we have a valid file.
	
	if (buff_pos>15) { // we need to get a new sector
		gFileSectorsPlayed++;

		if (cluster_pos>FAT16_sectors_per_cluster-1){ // need new cluster
			cluster_pos=0;
			gCluster = FAT_NextCluster(gCluster);

		}
		
		if (gCluster == 0xffffffff) { // finished song. on to next ?
			
			// Stop all playing and reset everything
			gFile_good = FALSE;
			gMode = MODE_STOP;
			MMC_Reset();
			flush_decoder(SOFT_RESET);
			
			if (PLAY_NEXT_SONG){ // cue up next song and play it.
				nextfile = FAT_getNextSong(gFAT_entry,FAT16_dir_first_sector);
				if (RECURSE_DIRECTORIES == 0){ // then skip directories	
					while (FAT16_filetype & FILE_TYPE_DIR) {
						nextfile = FAT_getNextSong(nextfile,FAT16_dir_first_sector);
					}
				}
				if(nextfile){// play next song
					gFAT_entry = nextfile;
					gLCD_Refresh=TRUE;
					cue_file();
					gMode = MODE_PLAY;
				}else if(PLAY_REPEAT_ALL){
					gFAT_entry = FAT16_entryMIN;
					gLCD_Refresh=TRUE;
					cue_file();
					gMode = MODE_PLAY;
				}
			}
			return;
		}
		
		// read sector from MMC up to 255 times
		while (FAT_readCluster(gCluster,cluster_pos)&& (--abort)){
			MMC_Reset(); // reset if read failed.
		}
		
		buff_pos=0;
		cluster_pos++; // increment for next time
	}
	
	// need to send data to mp3?
	// send at most the rest of sector in memory.
	while (bit_is_set(DREQ_PORT,DREQ_PIN) && (buff_pos<16)){
		vs1001_send_32(FAT_buffer+(32*buff_pos++));
	}
	
	return;
}
    

/**
 *	handle_mmc
 *
 *	Check for MMC  insertion/removal 
 *
 *	@param		old status of MMC
 *	@return		new status of MMC
 *
 **/
uint8 handle_mmc(uint8 MMC_present)
{
	uint16	c;
//	uint32  mmc_size=0;
//	uint32	nextfile;
	
	#ifdef SW_MMC_IDENTIFY
	if (MMC_present == FALSE) {
		MMC_Reset();
	}
	c = (((MMC_Check() & 0xFF00) == 0) ? TRUE : FALSE);
	#else
	c =  MMC_Detect();			// detect mmc
	#endif
	
	if (c && ( MMC_present == FALSE) ){ // mmc just inserted
		gPowerSaveTimer=0; // don't time out I just put a card in.
		
		// startup mmc, FAT and find first File
		if (!init_mmc_fat()){ // mmc and Fat init okay..
			MMC_present = TRUE;
			//mmc_size = MMC_Capacity(); // get capacity in Bytes
			//PRINT("MMC :");UART_Printfu32(mmc_size); 
			//update_LCD();
			gLCD_Refresh=TRUE;
			
			if((FAT16_entryMIN==0) && (FAT16_entryMAX==-1)){// NO SONGS!
				gMode= MODE_IDLE;
				PRINT("NO MP3");EOL();
				//FAT_tester();
			}else{
				gFAT_entry = FAT16_entryMIN;
				gMode= MODE_STOP;
			}

		}
		
		
	}else if ((c == FALSE) && MMC_present ){	//mmc just removed
		//	PRINT("No MMC\t");UART_Printfu16(c);EOL();
		//	gFAT_entry=0; // reset FAT to beginning
		gMode = MODE_IDLE;
		gLCD_Refresh=TRUE;
		gFile_good= FALSE;
		MMC_present = FALSE;
		//update_LCD();
		gPowerSaveTimer=0; //don't time out we're changing cards
	}

	return MMC_present;
}
   

/**
 *	handle_interface	
 *
 *	Scans for button presses and change activity accordingly
 **/
void 	handle_interface(void)
{
	char c;
	uint32 nextfile;
	

	c = getkey(); // get key press
	
	if (c == KEY_NULL && UART_HasChar() ){ // check for serial
		c = UART_ReceiveByte();
		switch(c){
			case 'j': c = KEY_PREV;		break;
			case 'k': c = KEY_MINUS;	break;
			case 'l': c = KEY_NEXT;		break;
			case 'i': c = KEY_PLUS;		break;
			case ' ': c = KEY_ENTER;	break;
			default : c = KEY_NULL;		break;
		}	
		
	}

	if (c != KEY_NULL){ // check for button press
		gPowerSaveTimer = 0;                // Reset the Auto Power Down timer
		
		PRINT("KEY: ");UART_Printfu08(c);EOL();
		
		switch (c){
			case KEY_ENTER : 
				// change mode.
				if (gMode == MODE_PLAY){// currently playing
					gMode = MODE_PAUSE; // stop
				}else if (gMode == MODE_PAUSE){
					UART_Printfu08(FAT16_filetype);
					UART_Printfu08(FILE_TYPE_DIR);EOL();
					if (FAT16_filetype & FILE_TYPE_DIR){
						//PRINT("DIR");EOL();
						
						// scan a new directory....
						FAT_readFile(gFAT_entry,FAT16_dir_first_sector);
						 
						// get cluster address from fat entry
						FAT_Scratch2Cluster();
						
						// check for files and exit on no files(always 2 entries . and ..)
						open_Dir(FAT_cluster2lba(gCluster));
						
						//	return;
						gFAT_entry = FAT16_entryMIN;
						
					}else{
						gMode = MODE_PLAY;
					}
					
				}else if (gMode == MODE_STOP){
					if (FAT16_filetype & FILE_TYPE_DIR){
						// PRINT("DIR");EOL();
						// scan a new directory....
						FAT_readFile(gFAT_entry,FAT16_dir_first_sector);
						
						// get cluster address from fat entry
						FAT_Scratch2Cluster();
						
						// check for files and exit on no files(always 2 entries . and ..)
						open_Dir(FAT_cluster2lba(gCluster));
						
						//	return;
						gFAT_entry = FAT16_entryMIN;
						
					}else{
						gMode = MODE_PLAY;
						cue_file();
					}
					
				}else if ((gMode == MODE_IDLE) && gFile_good){
					gMode = MODE_STOP;
				}
				
				// decoder will have shut down if stream has stoped.
				// so we need to  restart
				flush_decoder(SOFT_RESET); 
				gLCD_Refresh=TRUE;
				
				// TODO: add long short presses.
				
				break;
				
			case KEY_NEXT : // find next file
				if (SCAN_JUMPTOSTART && (gFAT_entry==FAT16_entryMAX)){
					gFAT_entry=FAT16_entryMIN;
					gLCD_Refresh = TRUE;
					cue_file();
				}else{
					nextfile = FAT_getNextSong(gFAT_entry,FAT16_dir_first_sector);
					if(nextfile){// get next song
						gFAT_entry = nextfile;
						gLCD_Refresh=TRUE;	
						cue_file();
					}
				}
				break;
				
			case KEY_PREV ://find previous file 
				if (SCAN_JUMPTOSTART && (gFAT_entry==FAT16_entryMIN)){
					gFAT_entry=FAT16_entryMAX;
					gLCD_Refresh = TRUE;
					cue_file();
				}else{
					nextfile = FAT_getPrevSong(gFAT_entry,FAT16_dir_first_sector);
					if(nextfile){// play next song
						gFAT_entry = nextfile;
						gLCD_Refresh = TRUE;
						cue_file();
					}
				}
				break;
				
			case KEY_PLUS:
				//if (gMode == MODE_PAUSE && !(FAT16_filetype & FILE_TYPE_DIR)){ 	// toggle Bass boost
				//	gbass_boost = !gbass_boost;
				//	gLCD_Refresh=TRUE;
				//}else 
		
					// no mmc? then go to options
				if(gMode == MODE_PLAY){ 					// increase the volume
					gmusic_vol++;
					if (gmusic_vol>9)
						gmusic_vol=9;
						
					//PRINT("Volume :");UART_Printfu08(gmusic_vol); EOL();
					vs_1001_setvolume(pgm_read_byte(volume_val+gmusic_vol), pgm_read_byte(volume_val+gmusic_vol));
					//LCD_Vol(gmusic_vol);
					//LCD_puts_f("VOL +",TRUE);
					gLCD_Refresh=TRUE;
					
				}else {//if(((gMode == MODE_PAUSE) || (gMode == MODE_IDLE)) ){
					//  && (FAT16_filetype & FILE_TYPE_DIR)	){
					// go up a directory?
					
					//PRINT("..");EOL();
					
					// NOTE: gCluster is an lba here not a cluster address 
					if ((gCluster = FAT_getParentDir(FAT16_dir_first_sector)) != 0x00){	
						open_Dir(gCluster);
						//PRINT("fdg");EOL();
						gFAT_entry = FAT16_entryMIN;
						cue_file();
					}
				}
				break;
				
			case KEY_MINUS:
				if ((gMode == MODE_PAUSE) || (gMode == MODE_IDLE)|| 
					(gMode == MODE_STOP)){	// Power OFF
					Power_off();
				}else {
					gmusic_vol--;
					if (gmusic_vol>9) // overflow to 255
						gmusic_vol=0;
						
					//PRINT("Volume :");UART_Printfu08(gmusic_vol); EOL();
					////LCD_puts_f("VOL -",TRUE);
					vs_1001_setvolume(pgm_read_byte(volume_val+gmusic_vol), pgm_read_byte(volume_val+gmusic_vol));
					gLCD_Refresh=TRUE;
				}
				break;
				
			default	:	break;
		}
	}
}
   


/**
 *	update_LCD 
 *
 *	Update the display depending on mode.
 *
 **/

void 	update_LCD(void)
{
	uint16	voltage;
	uint8	percentage;
	static uint8 vol = -1,mode = -1,second = -1;
	static uint16 file = -1;
	
#ifdef ALPHA
	if (gLCD_Refresh){
    //  LCD_Blank();								// clear LCD

		if (gMode == MODE_PLAY){
   			if (file!=(uint16) gFAT_entry) LCD_FileName(FALSE);
   			if (mode!=gMode) LCD_Play();
   			if (vol !=gmusic_vol) LCD_Vol(gmusic_vol, gbass_boost);
   			if (second !=(uint8) gPlayTimeSeconds) LCD_Time(gPlayTimeSeconds);

		}else if (gMode == MODE_PAUSE){
   			if (file!= (uint16) gFAT_entry) LCD_FileName(FALSE);
   			if (mode!=gMode) LCD_Pause();
   			if (vol !=gmusic_vol) LCD_Vol(gmusic_vol, gbass_boost);

		}else if (gMode == MODE_STOP){
   			if (file!= (uint16) gFAT_entry) LCD_FileName(FALSE);
   			if (mode!=gMode) LCD_Stop();
   			if (vol !=gmusic_vol) LCD_Vol(gmusic_vol, gbass_boost);
         
		}else if (gMode == MODE_IDLE){
		   if (mode!=gMode) {
			LCD_Blank();						// clear LCD
			LCD_Tester();						// Print the welcome screen and info.
			
			//display battery status
			voltage = Power_check();
			
			percentage = Power_pcnt(voltage);
			//UART_Printfu08(percentage);EOL();
			LCD_Battery(percentage);
			
			// revert to filname after seconds 
			gDisplayTimeOut=(2*STAT_DISPLAY_TIME)+1;
			}
		}
		
		mode=gMode;
		vol=gmusic_vol;
		file= (uint16) gFAT_entry;
		second =(uint8) gPlayTimeSeconds;

		
		gLCD_Refresh =FALSE;
	}

#endif // ALPHA

#ifdef NOKIA

	if (gLCD_Refresh){

	   if ((mode!=gMode) || (file!=(uint16) gFAT_entry)) LCD_Blank(); // clear LCD

  		// Always display battery status
		voltage = Power_check();
		percentage = Power_pcnt(voltage);
		LCD_Battery(percentage);


		if (gMode == MODE_PLAY){
			LCD_FileName((file!=(uint16) gFAT_entry));
   			LCD_Play();
   			LCD_Vol(gmusic_vol, gbass_boost);
   			LCD_Time(gPlayTimeSeconds);

		}else if (gMode == MODE_PAUSE){
			LCD_FileName((file!=(uint16) gFAT_entry));
   			LCD_Pause();
   			LCD_Vol(gmusic_vol, gbass_boost);
			LCD_Time(gPlayTimeSeconds);

		}else if (gMode == MODE_STOP){
   			LCD_FileName((file!=(uint16) gFAT_entry));
   			LCD_Stop();
   			LCD_Vol(gmusic_vol, gbass_boost);
         
		}else if (gMode == MODE_IDLE){
			// Print the welcome screen and info.
			LCD_Tester(); 		
			
			// revert to filname after seconds 
			gDisplayTimeOut=(2*STAT_DISPLAY_TIME)+1;
			
		}

      
		mode=gMode;
		vol=gmusic_vol;
		file= (uint16) gFAT_entry;
		second =(uint8) gPlayTimeSeconds;

		
		gLCD_Refresh =FALSE;
	}
#endif // NOKIA

}


/**
 *	main
 *
 *	The guts.
 *
 *	@return		int (never returns)
 **/
int 	main(void) 
{
	uint8	MMC_present,j;
	
	//********************************
	//* 		Initialize 
	//********************************
	init_HW();		// Do hardware Initialization.
	gMode=MODE_IDLE;
	gLCD_Refresh=TRUE;
	update_LCD();		// make sure NOKIA screen displayed b4 scanning MMC
	gLCD_Refresh=TRUE;
	MMC_present = restore_player(); // Restore saved position.
	gPowerSaveTimer =0;
	
	//********************************
	//* 		Main Loop 
	//********************************
	while (1)
	{	
		// check for button press
		handle_interface();
		
		// update lcd
		update_LCD();

		// check for MMC removal or insertion.
		MMC_present = handle_mmc(MMC_present);
		
		if (MMC_present){
			// check mode ie playing idle sleep nommc
			if(gMode == MODE_PLAY){ 
				// keep streaming if mp3 decoder can take it 
				// (up to 4 sectors at a time).
				j=4;
				while (bit_is_set(DREQ_PORT,DREQ_PIN) && j--)
					streaming();
			}
			else{
				Power_idle();
				if (gPowerSaveTimer >= AUTOPOWEROFF_TIMEOUT)
					Power_off();
			}
		}
		else{
			Power_idle();
			if (gPowerSaveTimer >= AUTOPOWEROFF_TIMEOUT)
				Power_off();
		}

	}
	return 0;
}


