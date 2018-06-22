#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include "keyboard.h"
#include "i8042.h"

#ifdef LAB3
int g_inb_counter = 0;
int sys_inb_cnt(port_t port, unsigned long *byte);
#else
#define sys_inb_cnt(p,q) sys_inb(p,q)
#endif

int g_initialHookID_TIMER = 3;
int g_hookID_TIMER = 3;

int g_initialHookID_KBD = 4;
int g_hookID_KBD = 4;

int g_timerCounter = 0;


/*TIMER FUNCTIONS*/

int timer_subscribe_int(void ) {


	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &g_hookID_TIMER) != OK ) return -1;

	sys_irqenable(&g_hookID_TIMER);


	return g_initialHookID_TIMER;
}

int timer_unsubscribe_int() {

	sys_irqrmpolicy(&g_hookID_TIMER);


	return 0;
}


/*KEYBOARD FUNCTIONS*/

int kbd_test_scan(unsigned short asm_enable) {

	//se asm_enable == 0, executa ih em c

	//subscribe keyboard interrupts
	kbd_subscribe_int();

	int ipc_status, r;
	message msg;

	int stopLoop = 1; //0 = true -|- 1 = false

	while(stopLoop == 1) {
		// Get a request message.
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d\n", r);
			continue;//continue while loop
		}
		if (is_ipc_notify(ipc_status)) { // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: // hardware interrupt notification
					if (msg.NOTIFY_ARG & BIT(g_initialHookID_KBD)) { // subscribed interrupt
						stopLoop = kbd_int_handler(asm_enable);
					}
					break;
				default:
					break; // no other notifications expected: do nothing
			}
		} else { // received a standard message, not a notification
			// no standard messages expected: do nothing
		}
	}

	//unsubscribe keyboard interrupts
	kbd_unsubscribe_int();






	return 0;
}

int kbd_test_poll(){

	int is2Byte= 1, stopLoop = 1; //0 = true -|- 1 = false


	unsigned long statusReg;
		while(stopLoop == 1) {
			sys_inb(KBD_IN_BUF, &statusReg); // read th+e status register (0x64)


			if (((statusReg & BIT(0)) == BIT(0)) && ((statusReg & BIT(5)) != BIT(5))) { // check if OBF is set and AUX is clear

				//buffer declaration (will hold the keyboard scancode)
				unsigned long bufferData;

				//write the keyboard scancode to bufferData var
				sys_inb_cnt(KBD_OUT_BUF, &bufferData);

				//PROCESSING THE SCANCODE

				if( (unsigned short) bufferData == 0x81) {
					stopLoop = 0;
					printf("sysinb counter: %d\n", g_inb_counter);
				}

				else if ((unsigned short) bufferData == 0xe0 && is2Byte == 1) {
					is2Byte = 0;
				} else {
					if(is2Byte == 1) {
						//normal scancode processing
						processOutputCode((unsigned short) bufferData); //if no 0xe0 was received before, process the scancode normally
					}
					else {
						//if an 0xe0 was received before, the function must process the 2 bytes
						processDualOutputCode((unsigned short) 0xe0, (unsigned short) bufferData);
						//reset flag value
						is2Byte = 1;
					}
				}

		}

		tickdelay(micros_to_ticks(DELAY_US));
	}


	return 0;
}

int kbd_test_timed_scan(unsigned short n) {

// FALTA ATIVAR INTERRUPÇOES DE TIMER E PROCESSAR AS GLOBAIS E MUDAR A CONDIÇAO DE PARAGEM (INCLUIR TEMPO)

	//subscribe timer interrupts
	timer_subscribe_int();
	//subscribe keyboard interrupts
	kbd_subscribe_int();

	int ipc_status, r;
	message msg;

	//reseting the timer counter
	g_timerCounter = 0;


	int is2Byte= 1, stopLoop = 1; //0 = true -|- 1 = false

	//while the user hasn't entered the ESC key and the idle time hasn't past
	while(stopLoop == 1 && g_timerCounter < 60*n) {
		// Get a request message.
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d\n", r);
			continue;//continue while loop
		}
		if (is_ipc_notify(ipc_status)) { // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: // hardware interrupt notification
					if (msg.NOTIFY_ARG & BIT(g_initialHookID_TIMER)) { // subscribed interrupt TIMER
						g_timerCounter++;//increment timer counter

					}

					if (msg.NOTIFY_ARG & BIT(g_initialHookID_KBD)) { // subscribed interrupt KEYBOARD



						//since a key was pressed, the timer counter wil be reset
						g_timerCounter = 0;


						//buffer declaration (will hold the keyboard scancode)
						unsigned long bufferData;
						//write the keyboard scancode to bufferData var

						sys_inb_cnt(KBD_OUT_BUF, &bufferData);


						if( (unsigned short) bufferData == 0x81) {
							stopLoop = 0;
							printf("sysinb counter: %d\n", g_inb_counter);
						}

						else if ((unsigned short) bufferData == 0xe0 && is2Byte == 1) {
							is2Byte = 0;
						} else {
							if(is2Byte == 1) {
								//normal scancode processing
								processOutputCode((unsigned short) bufferData); //if no 0xe0 was received before, process the scancode normally
							}
							else {
								//if an 0xe0 was received before, the function must process the 2 bytes
								processDualOutputCode((unsigned short) 0xe0, (unsigned short) bufferData);
								//reset flag value
								is2Byte = 1;
							}
						}


					}
					break;
				default:
					break; // no other notifications expected: do nothing
			}
		} else { // received a standard message, not a notification
			// no standard messages expected: do nothing
		}
	}


	//unsubscribe timer interrupts
	timer_unsubscribe_int();
	//unsubscribe keyboard interrupts
	kbd_unsubscribe_int();
	printf("stopped after %d seconds idle.\n", g_timerCounter/60);


	return 0;
}

int kbd_subscribe_int(void ) {

	if(sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &g_hookID_KBD) != OK ) return -1;

	sys_irqenable(&g_hookID_KBD);


	return g_initialHookID_KBD;
}

int kbd_unsubscribe_int() {

	sys_irqrmpolicy(&g_hookID_KBD);


	return 0;
}

int kbd_int_handler(unsigned short asm_enable) {
	int stopLoop = 1;

	static int is2Byte= 1; //0 = true -|- 1 = false

	unsigned long bufferData;
	if(asm_enable == 0) {

		//C version
		sys_inb_cnt(KBD_OUT_BUF, &bufferData);
	} else {
		//ASM version
		getBuffer();
		bufferData = (unsigned long) g_bufferData;
	}


	if( (unsigned short) bufferData == 0x81) {
		stopLoop = 0;
		if(asm_enable == 0) printf("sysinb counter: %d\n", g_inb_counter);
	}

	else if ((unsigned short) bufferData == 0xe0 && is2Byte == 1) {
		is2Byte = 0;
	} else {
		if(is2Byte == 1) {
			//normal scancode processing
			processOutputCode((unsigned short) bufferData); //if no 0xe0 was received before, process the scancode normally
		}
		else {
			//if an 0xe0 was received before, the function must process the 2 bytes
			processDualOutputCode((unsigned short) 0xe0, (unsigned short) bufferData);
			//reset flag value
			is2Byte = 1;
		}
	}
	return stopLoop;//return the stopping flag
}

void processOutputCode(unsigned short scancode) {
	if(scancode < 0x80) {
		printf("Makecode: 0x%x\n", scancode);
	} else {
		printf("Breakcode: 0x%x\n", scancode);
	}
}

void processDualOutputCode(unsigned short firstByte, unsigned short scancode) {
	if(scancode < 0x80) {
		printf("Makecode: 0x%x 0x%x\n", firstByte, scancode);
	} else {
		printf("Breakcode: 0x%x 0x%x\n", firstByte, scancode);
	}
}

int sys_inb_cnt(port_t port, unsigned long *byte){
	g_inb_counter++;
	return sys_inb(port, byte);
}
