#ifndef _TIMER_H_
#define _TIMER_H_

#define BIT(n) (0x01<<(n))

#define TIMER0_IRQ	        	0    /**< @brief Timer 0 IRQ line */

#define DELAY_US 		   20000 /*sets the delay variable*/

int timer_subscribe_int(void );

int kbd_unsubscribe_int();

#endif
