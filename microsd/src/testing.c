
#include "testing.h"

/***************************************************************************
*   Name:	dump_buffer
*	Description: Dumps the mmc_sbuf[] to the Uart
*	Parameters: <lines> # of lines (16 Bytes) to send starting from 0x00
*	Returns: 	none
***************************************************************************/
void dump_buffer(uint8 lines, uint8 buffer[])
{
	uint8 c,i;
	for (c=0;c<lines;c++){
		EOL();
		UART_Printfu08(c);
		PRINT(": ");
		for (i=0;i<16;i++){
			UART_Printfu08(buffer[i+c*16]);
			PRINT(" ");
		}
		for (i=0;i<16;i++){
			if ((buffer[i+c*16] > 31) && (buffer[i+c*16] <= 'z'))
				UART_SendByte(buffer[i+c*16]);	
			else
				UART_SendByte('.');
			//PRINT(" ");
		}
	}
}

/***************************************************************************
*   Name:			MMC_tester
*	Description:	Check out basic MMC functions
*	Parameters:		none
*	Returns:	Status byte, non-zero for failure.
***************************************************************************/
uint8 MMC_tester(void)
{
	uint8 c;
	uint16 status;
	uint32 cap;
	
	c =  MMC_Reset();				// init mmc
	PRINT("MMC_RESET returned ");
	UART_Printfu08(c);
	EOL();
	if (c==0){
		status = MMC_Check();				// check status
		PRINT("MMC_SEND_STATUS returned ");
		UART_Printfu16(status);
		c = (uint8) status;
		EOL();
	}
	if (c==0){
		c = MMC_Identify();
		PRINT("MMC_SEND_CID returned ");
		UART_Printfu08(c);
		if (c==0){ // identity OK
			dump_buffer(2,mmc_scratch);// dump 2 lines from the buffer
		}
		c=0;
		EOL();
	}
	if (c==0){
		c = MMC_CardType();
		PRINT("MMC_SEND_CSD returned ");
		UART_Printfu08(c);
		if (c==0){ // CSD OK
			dump_buffer(2,mmc_scratch);// dump 2 lines from the buffer
		}
		c=0;
		EOL();
	}
	if (c==0){
		cap = MMC_Capacity();
		PRINT("MMC_Capacity returned ");
		UART_Printfu32(cap);
		EOL();
	}
	
	if (c==0){
		c = MMC_Name();
		PRINT("MMC_Name returned ");
		UART_Printfu08(c);
		PRINT(" ");
		if (c==0){
			UART_Puts(mmc_scratch);
		}	
		EOL();
	}
	
	if (c == 0) {
		c =  MMC_Read(0x20);// read boot sector
		PRINT("MMC_Read returned ");
		UART_Printfu08(c);
		EOL();
		if (c==0){
			PRINT("MMC Boot Sector: ");
			dump_buffer(32,mmc_sbuf); // dump the sector
		}
	}
			
	EOL();

	
	
	return c;
}


/***************************************************************************
*   Name:			FAT_tester
*	Description:	Routines to test filesystem functions to the mmc
*	Parameters:		none
*	Returns:		error code
***************************************************************************/
uint8	FAT_tester(void)
{
	uint8 result,i;
	uint8 record;
	uint8 attrib;
	uint16 sector=0;
//	uint16 cluster;
//	uint32 size;
	
	// set up the FAT variables
	FAT_buffer = mmc_sbuf; // 512 byte buffer for sector reads/writes
	FAT_read = MMC_Read; // pointer to read block function 
	FAT_write = MMC_Write;// pointer to write block function 
	
	result =  MMC_Reset();				// init mmc
	if (result) return result;
	
	result = FAT_initFat16();
	if (result) return result; // abort on non-zero reply 
	
	// print Fat info
	PRINT("FAT boot Sector info");EOL();
	PRINT("FAT begins at sector ");
	UART_Printfu32(FAT16_fat_begin_lba);EOL();
	PRINT("Clusters begin at sector ");
	UART_Printfu32(FAT16_cluster_begin_lba);EOL();
	PRINT("Sectors per cluster = ");
	UART_Printfu08(FAT16_sectors_per_cluster);EOL();
	PRINT("Root dir starts at sector ");
	UART_Printfu32(FAT16_root_dir_first_sector);EOL();
	
	//show volume label
	result = FAT_get_label(mmc_scratch);
	if (!result){
		PRINT("Volume Name is ");
		UART_Puts(mmc_scratch);
		EOL();
	}
	
	// read the root dir
	sector = FAT16_root_dir_first_sector;
	result = FAT_read(sector);

	record =0;
	while((result==0) && mmc_sbuf[record*32]!=0){
		 // check firstByte
		if (mmc_sbuf[record*32] != 0xe5){ // not used (aka deleted)
			
			// get the attrib byte
			attrib = mmc_sbuf[(record*32)+11];
			
			if (attrib == FILE_TYPE_FILE || attrib == FILE_TYPE_DIR ){ // entry is normal 8.3 entry
				
				if (attrib == FILE_TYPE_DIR) PRINT("[");
				
				// construct short filename string
				for (i=0;i<8;i++){ 
					mmc_scratch[i] = mmc_sbuf[(record*32)+i];
				}
				mmc_scratch[8] = '.';
				mmc_scratch[9] = mmc_sbuf[(record*32)+8];
				mmc_scratch[10] = mmc_sbuf[(record*32)+9];
				mmc_scratch[11] = mmc_sbuf[(record*32)+10];
				mmc_scratch[12] = 0x00;
				UART_Puts(mmc_scratch);		
				if (attrib == FILE_TYPE_DIR) PRINT("]");
				PRINT("\t");
				
				// get Cluster 
				UART_Printfu08(mmc_scratch[13] = mmc_sbuf[(record*32)+0x15]);
				UART_Printfu08(mmc_scratch[14] = mmc_sbuf[(record*32)+0x14]);
				UART_Printfu08(mmc_scratch[15] = mmc_sbuf[(record*32)+0x1B]);
				UART_Printfu08(mmc_scratch[16] = mmc_sbuf[(record*32)+0x1A]);
				PRINT("\t");
				
				
				//get fileSize
				UART_Printfu08(mmc_scratch[17] = mmc_sbuf[(record*32)+0x1f]);
				UART_Printfu08(mmc_scratch[18] = mmc_sbuf[(record*32)+0x1e]);
				UART_Printfu08(mmc_scratch[19] = mmc_sbuf[(record*32)+0x1d]);
				UART_Printfu08(mmc_scratch[20] = mmc_sbuf[(record*32)+0x1c]);
				PRINT("\t");
				
				//get filenumber
				UART_Printfu32(record+(sector<<4));
				EOL();
				
			}
			
				
		}
		
		// next record or on to next sector
		record++;
		
		if (record==16){
			record = 0;
			record = 0;
			record = 0;

			sector++;
			result = FAT_read(sector);
			
		}
	}
	 
	 // print number of files in directory
	
	// get limits of file system.
	
	// play file "test.mp3"
	EOL();
	
	return result;
}



