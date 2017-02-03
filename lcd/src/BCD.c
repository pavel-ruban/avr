/**
***************************************************************************
*
*  @file	BCD.c
*  @author	ATMEL Norway
*  @brief	AVR Butterfly BCD conversion algorithms
*
* <pre>
*  Target(s)...: ATmega169
*  Compiler....: IAR EWAAVR 2.28a
*  Revisions...: 1.0
* 
*  YYYYMMDD - VER. - COMMENT                                       - SIGN.
*  20030116 - 1.0  - Created                                       - KS
* </pre>
*
*  $Id: BCD.c,v 1.4 2007/06/04 15:11:27 brokentoaster Exp $
**************************************************************************
**/


/**
***************************************************************************
*
*   Function name : CHAR2BCD2
*
*   @return	Binary coded decimal value of the input (2 digits)
*   @param	input Value between (0-99) to be encoded into BCD 
*
*     Convert a character into a BCD encoded character.
*     The input must be in the range 0 to 99.
*     The result is byte where the high and low nibbles
*     contain the tens and ones of the input.
*
***************************************************************************
**/
char CHAR2BCD2(char input)
{
    char high = 0;
    
    
    while (input >= 10)                 // Count tens
    {
        high++;
        input -= 10;
    }

    return  (high << 4) | input;        // Add ones and return answer
}

/**
***************************************************************************
*
*   Function name : CHAR2BCD3
*
*   @return         Binary coded decimal value of the input (3 digits)
*
*   @param          input Value between (0-255) to be encoded into BCD 
*
*   Purpose  :      Convert a character into a BCD encoded character.
*                   The input must be in the range 0 to 255.
*                   The result is an integer where the three lowest nibbles
*                   contain the ones, tens and hundreds of the input.
*
*   @callergraph
***************************************************************************
 **/
//
//unsigned int CHAR2BCD3(char input)
//{
//    int high = 0;
//        
//    while (input >= 100)                // Count hundreds
//    {
//        high++;
//        input -= 100;
//    }
//
//    high <<= 4;
//    
//    while (input >= 10)                 // Count tens
//    {
//        high++;
//        input -= 10;
//    }
//
//    return  (high << 4) | input;        // Add ones and return answer
//}

