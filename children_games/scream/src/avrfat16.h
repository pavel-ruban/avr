/**
	@file	avrfat16.h
	@brief	Fat16 Functions
	@author	Nick Lott 
	@date	September 2004
 
	$Id: avrfat16.h,v 1.19 2007/06/04 15:12:18 brokentoaster Exp $

	Copyright (C) 2004 Nick Lott <brokentoaster@users.sf.net>

	This is a simple implementation of the FAT16 file system. It is designed
	to be small for use with MP3 players and MMC cards. Currently it is 
	readonly.NOTE: The code acknowledges only the first partition on the drive.
	
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

	Compiler....: AVR-GCC 3.3.1; avr-libc 1.0
**/


#include "types.h"
#include "uart.h"

#define PARTITION_START		446	///< Start address of partition 1 in MBR

//-----------------------------------------------------------------------------
// FAT32 File Attributes and Types
//-----------------------------------------------------------------------------
#define FILE_ATTR_READ_ONLY   	0x01
#define FILE_ATTR_HIDDEN 		0x02
#define FILE_ATTR_SYSTEM 		0x04
#define FILE_ATTR_SYSHID		0x06
#define FILE_ATTR_VOLUME_ID 	0x08
#define FILE_ATTR_DIRECTORY		0x10
#define FILE_ATTR_ARCHIVE  		0x20
#define FILE_ATTR_LFN_TEXT		0x0F
#define FILE_ATTR_LFN_MASK		0x3F
#define FILE_HEADER_BLANK		0x00
#define FILE_HEADER_DELETED		0xE5
#define FILE_TYPE_DIR			0x10
#define FILE_TYPE_FILE			0x20
#define FILE_TYPE_MP3			0x55
#define FAT16_LDIR_Ord			0
#define FAT16_LDIR_Name1		1
#define FAT16_LDIR_Attr			11
#define FAT16_LDIR_Type			12
#define FAT16_LDIR_Chksum		13
#define FAT16_LDIR_Name2		14
#define FAT16_LDIR_FstClusLO	26
#define FAT16_LDIR_Name3		28

// For ID3 tags
#define LFN_TYPE_INDEX        208
#define LFN_TYPE_FILENAME     0x00        ///< No bits set
#define LFN_TYPE_ID3_TITLE    0x01        ///< Bit 0
#define LFN_TYPE_ID3_ARTIST   0x02        ///< Bit 1
#define LFN_BUFFER_LENGTH     208
#define LFN_TITLE_INDEX       0
#define LFN_TITLE_LENGTH      100      ///< Title length including '\0'
#define LFN_ARTIST_INDEX      100
#define LFN_ARTIST_LENGTH     100      ///< Artist length including '\0'
#define LFN_FREE_INDEX        200
#define LFN_FREE_LEN          8        
///< Currently 8 free bytes in buffer if using ID3 tags not filenames

uint32 	FAT16_fat_begin_lba;
uint32 	FAT16_cluster_begin_lba;
uint8	FAT16_sectors_per_cluster;
uint32	FAT16_dir_first_sector;
uint32	FAT16_root_dir_first_sector;
uint32	FAT16_parent_dir_first_sector;

uint8	FAT16_filetype;
uint8 	FAT16_longfilename[LFN_BUFFER_LENGTH+1]; ///< buffer for long filenames / ID3 tags
uint8	FAT16_LFNTitleLen;
uint8	FAT16_LFNArtistLen;
uint16 	FAT16_entryMAX; 	///< Maximum entry number in FAT table. (aka file number)
uint16 	FAT16_entryMIN; 	///< Minimum entry number in FAT table. (aka file number)

uint8 *FAT_buffer; ///< 512 byte buffer for sector reads/writes
uint8 *FAT_scratch; ///< 32 byte buffer for filenames and data
uint8 (*FAT_read)(uint32 lba); ///< pointer to read block function 
uint8 (*FAT_write)(uint32 lba);///< pointer to write block function 

uint8 FAT_initFat16(void); ///< read MBR and bootsector and set fat variables
uint8 FAT_readCluster(uint32 cluster, uint8 sector_offset); ///< read a sector from a cluster
//uint8 FAT_get_label(uint8 label[]); 
uint32 FAT_NextCluster(uint32 cluster); //
//uint8 FAT_NextnCluster(uint16 cluster, uint16 fatBuf[], uint8 buffer_size);
//uint8 FAT_getLongFilename(uint32 filenumber);
uint32 FAT_getNextSong(uint32 filenumber,uint32 dir_lba);
uint32 FAT_getPrevSong(uint32 filenumber,uint32 dir_lba);

/** 
*	FAT_ChkSum() 
* 
* Returns an unsigned byte check sum computed on an unsignedbyte 
* array. The array must be 11 bytes long and is assumed to contain 
* a name stored in the format of a MS-DOS directory entry. 
* 
* @param	pFcbName Pointer to an unsigned byte array assumed to be 
*		11 bytes long. 
* @Return	An 8-bit unsigned checksum of the array pointed 
*		to by pFcbName. 
**/
uint8 FAT_ChkSum(uint8 *pFcbName); 

/**
 *   FAT_readRoot
 *
 *	Read the root directory of the disk and obtain details 
 *	about FAT entry number filenumber
 *
 *	@param		filenumber 32bit uInt pointing to file of interst.
 *	@return		error code
 **/
//uint8	FAT_readRoot(uint32 filenumber);

/**
 *   FAT_read
 *
 *	Read any directory of the disk and obtain details 
 *	about FAT entry number filenumber
 *
 *	@param		filenumber 32bit uInt pointing to file of interst.
 *	@param		dir_first_entry 32bit uInt pointing to first cluster of directory
 *	@return		error code
 **/
uint8	FAT_readFile(uint32 filenumber, uint32 dir_first_sector);

/**
 *	FAT_scanDir
 *
 *	scan a directory for songs, set max, min and currentdir variables
 *
 *	@param	directory_cluster uint32 pointing to director start cluster
 *	@return	number of files found (up to 255)
 **/
uint8 FAT_scanDir(uint32 directory_cluster);

/**
 *	FAT_scanDir_lba
 *
 *	scan a directory for songs, set max, min and currentdir variables
 *
 *	@param	lba_addr uint32 pointing to director starting sector
 *	@return	number of files found (up to 255)
 **/
uint8 FAT_scanDir_lba(uint32 lba_addr);

/**
*	FAT_cluster2lba
 *
 *	Calculate the sector address of a cluster.
 *
 *	@param	uint32 pointing to cluster
 *	@return	lba of first sector in cluster
 **/
uint32 FAT_cluster2lba(uint32 cluster);

/**
*	FAT_lba2cluster
 *
 *	Calculate the cluster address of a sector.
 *
 *	@param	lba_addr uint32 pointing to lba in cluster
 *	@return	cluster containing lba
 **/
uint32 FAT_lba2cluster(uint32 lba_addr);

/**
*	FAT_getParentDir
 *		
 *	@param	lba_addr uint32 pointing to current directory starting sector
 *	@return	first sector of the parent directory or 0 for error
 *
 *	return the first sector of the .. entry. assuming it is the 
 *	second entry in the directory file. otherwise return 0 to do nothing.
 **/
uint32 FAT_getParentDir(uint32 lba_addr);

/**
 *	FAT_Scratch2Cluster
 *
 *	Extracts the first cluster from the scratchpad buffer
 *	left after the most recent FAT_readFile() call
 **/
void FAT_Scratch2Cluster(void);
