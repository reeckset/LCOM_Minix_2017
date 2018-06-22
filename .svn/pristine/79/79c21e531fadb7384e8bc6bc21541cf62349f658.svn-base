#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"
#include "timer.h"
#include <minix/com.h>

int g_timerCounter = 0;
int g_hookID = 3;

int timer_set_frequency(unsigned char timer, unsigned long freq){
	unsigned char conf;
	timer_get_conf(timer, &conf);
	conf &= 0x0F; //Keep timer's BCD and Mode digits
	conf |= BIT(4) | BIT(5); //Add "LSB followed by MSB"
	unsigned char timerID = timer << 6; //Move timer identifier to the most significative bits
	conf |= timerID; //Add timer identifier to control word

	sys_outb(TIMER_CTRL, conf);
	unsigned long divisor = TIMER_FREQ / freq;
	unsigned char lsb = divisor & 0xFF;
	unsigned char msb = divisor >> 8;
	sys_outb(TIMER_0 + timer, lsb);
	sys_outb(TIMER_0 + timer, msb);
	return 0;
}

int timer_subscribe_int(void ) {

	sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &g_hookID);

	sys_irqenable(&g_hookID);

	return g_hookID;
}

int timer_unsubscribe_int() {

	sys_irqrmpolicy(&g_hookID);


	return 0;
}

void timer_int_handler() {

	int ipc_status, r;
	message msg;

	// Get a request message.
	if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
		printf("driver_receive failed with: %d", r);
		return;//continue while loop
	}
	if (is_ipc_notify(ipc_status)) { // received notification
		switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: // hardware interrupt notification
				if (msg.NOTIFY_ARG & g_hookID) { // subscribed interrupt
					g_timerCounter++;//increment timer
				}
				break;
			default:
				break; // no other notifications expected: do nothing
		}
	} else { // received a standard message, not a notification
		// no standard messages expected: do nothing
	}

}

int timer_get_conf(unsigned char timer, unsigned char *st) {
	unsigned long readBackCmd = TIMER_RB_CMD | BIT(5); //long containing read-back command code
	port_t timerPort;

	//Select timer to update readBackCmd
	if (timer == 0x00){
		readBackCmd |= BIT(1); //Timer 0
		timerPort = TIMER_0;
	}
	else if (timer == 0x01){
		readBackCmd |= BIT(2); //Timer 1
		timerPort = TIMER_1;
	}
	else if (timer == 0x02){
		readBackCmd |= BIT(3); //Timer 2
		timerPort = TIMER_2;
	}
	else return 1; //Return error 1 if timer does not exist

	sys_outb(TIMER_CTRL, readBackCmd); //Write read-back command to timer control

	unsigned long result;

	sys_inb(timerPort, &result); //Read config from desired timer

	*st = (unsigned char) result; //Return Value
	return 0;
}

int timer_display_conf(unsigned char conf) {
	//MASKS
	char maskBCD = BIT(0); //BCD
	char maskPM = BIT(1) | BIT(2) | BIT(3); //Programmed Mode
	char maskTOA = BIT(4) | BIT(5); //Type Of Access
	char maskNC = BIT(6); //Null Count
	char maskOut = BIT(7); //Output

	//CONF VALUES  (applying mask (between parentisis) and Right shift for value checking)
	char bcd = maskBCD & conf; //BCD
	char pm = (maskPM & conf) >> 1; //Programmed Mode
	char toa = (maskTOA & conf) >> 4; //Type Of Access
	char nc = (maskNC & conf) >> 6; //Null Count
	char out = (maskOut & conf) >> 7; //Output

	//strings (char*) descriptions initialization
	char* bcdDescription;
	char* pmDescription;
	char* toaDescription;

	switch (bcd) {
		case 0x0:
			bcdDescription = " - Binary";
			break;
		default://bcd = 0x1
			bcdDescription = " - BCD";
			break;
	}

	switch (pm) {
			case 0x0:
				pmDescription = " - Interrupt on Terminal Count";
				break;
			case 0x1:
				pmDescription = " - Hardware retriggerable one-shot";
				break;
			case 0x2:
				pmDescription = " - Rate generator";
				break;
			case 0x3:
				pmDescription = " - Square Wave generator";
				break;
			case 0x4:
				pmDescription = " - mode4";
				break;
			case 0x5:
				pmDescription = " - mode5";
				break;
			case 0x6:
				pmDescription = " - Rate generator";
				break;
			case 0x7:
				pmDescription = " - Square Wave generator";
				break;
		}

	switch (toa) {
		case 0x1:
			toaDescription = " - LSB";
			break;
		case 0x2:
			toaDescription = " - MSB";
			break;
		default: //toa = 0x3
			toaDescription = " - LSB followed by MSB";
			break;
	}


	//Display info
	printf("\nCounting Mode: %d %s \nProgrammed Mode: %d %s \nType Of Access: %d %s \nNull Count: %d \nOutput: %x\n", bcd, bcdDescription, pm, pmDescription, toa, toaDescription, nc, out);
	return 0;
}

int timer_test_time_base(unsigned long freq) {
		timer_set_frequency(0x00, freq);
	return 0;
}

int timer_test_int(unsigned long time) {
	timer_subscribe_int(); //subscribe to interrupts
	int secondsCounter = 0; //local second counter
	g_timerCounter = 0; //reset global counter
	while(g_timerCounter < 60*time) { //timeout at 60sec, assuming 60Hz timer freq
		timer_int_handler();//Incrementing global counter every 1/60 of a second
		if(g_timerCounter % 60 == 0){ //Detect every second
			secondsCounter++; //increment local seconds counter
			printf("Seconds passed: %d\n", secondsCounter); //print message
		}
	}
	timer_unsubscribe_int(); //Stops interrupts subscription
	return 0;
}

int timer_test_config(unsigned char timer) {
	unsigned char conf;
	timer_get_conf(timer, &conf);
	timer_display_conf(conf);
	return 0;
}

