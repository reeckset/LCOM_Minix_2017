#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define BIT(n) (0x01<<(n))

#define KEYBOARD_IRQ	        1    /**< @brief KEYBOARD IRQ line */
#define TIMER0_IRQ	        	0    /**< @brief Timer 0 IRQ line */

#define KBD_IN_BUF			0x64 /**< @brief Keyboard input register */
#define KBD_OUT_BUF			0x60 /**< @brief Keyboard output register */

#define DELAY_US 		   20000 /*sets the delay variable*/

#endif
