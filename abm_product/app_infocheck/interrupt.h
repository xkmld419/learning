#ifndef	_INTERRUPT__H_
#define	_INTERRUPT__H_

#include<signal.h>

void DenyInterrupt ( void );
void AllowInterrupt ( void );
bool GetInterruptFlag ( void );
void __Interrupt ( int Signo );

#endif//_INTERRUPT__H_
