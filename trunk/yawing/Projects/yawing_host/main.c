#include "msp430x22x4.h"

volatile unsigned int i;                    // volatile to prevent optimization

void main(void)
{
  	WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer
	BCSCTL1 = CALBC1_8MHZ;                  // Set DCO to 8MHz
  	DCOCTL = CALDCO_8MHZ;  
	
  P1DIR = 0xFF;                             // All P1.x outputs
  P1OUT = 0;                                // All P1.x reset
  P2SEL = 0;                                // All P2.x GPIO function
  P2DIR = 0xFF;                             // All P2.x outputs
  P2OUT = 0;                                // All P2.x reset
  P3DIR = 0xFF;                             // All P3.x outputs
  P3OUT = 0;                                // All P3.x reset
  P4DIR = 0xFF;                             // All P4.x outputs
  P4OUT = 0;                                // All P4.x reset
  TACTL = TASSEL_2 + MC_3 + TAIE;           // SMCLK, contmode, interrupt

  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
  
  for (;;)
  {
  }
}

// Timer_A3 Interrupt Vector (TAIV) handler
#pragma vector=TIMERA1_VECTOR
__interrupt void Timer_A(void)
{
	switch  (TAIV)       // Efficient switch-implementation
	{
		case  2: 
				break;                         // TACCR1 not used
	    case  4: 
				break;                         // TACCR2 not used
	    case 10: 
				P1OUT ^= 0x01;                 // overflow
				TAR = 15535;
	    		break;
		default:
				break;
	}
}

