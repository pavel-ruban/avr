/**

  @file		timer0.h
  @brief	AVR Butterfly Timer0 routines
  @author	ATMEL Norway

  <pre>
  Target(s)...: ATmega169
  Compiler....: AVR-GCC 3.3.1; avr-libc 1.0
  Revisions...: 1.0

  YYYYMMDD - VER. - COMMENT                                       - SIGN.

  20030116 - 1.0  - Created                                       - KS
  20031009          port to avr-gcc/avr-libc                      - M.Thomas
  </pre>

  $Id: timer0.h,v 1.2 2007/06/04 15:11:34 brokentoaster Exp $

**/


// Typedef for Timer callback function 
typedef void (*TIMER_CALLBACK_FUNC) (void);

#define TIMER0_NUM_CALLBACKS        4
#define TIMER0_NUM_COUNTDOWNTIMERS  4

void Timer0_Init(void);

BOOL Timer0_RegisterCallbackFunction(TIMER_CALLBACK_FUNC pFunc);
BOOL Timer0_RemoveCallbackFunction(TIMER_CALLBACK_FUNC pFunc);


//mt 
// char Timer0_AllocateCountdownTimer();
char Timer0_AllocateCountdownTimer(void);
char Timer0_GetCountdownTimer(char timer);
void Timer0_SetCountdownTimer(char timer, char value);
void Timer0_ReleaseCountdownTimer(char timer);
