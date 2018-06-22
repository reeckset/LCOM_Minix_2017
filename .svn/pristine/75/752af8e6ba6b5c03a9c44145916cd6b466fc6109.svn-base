#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

extern unsigned char g_bufferData;
extern void getBuffer();


//timer functions
int timer_subscribe_int();
int timer_unsubscribe_int();

int kbd_test_scan(unsigned short asm_enable);
int kbd_test_poll();
int kbd_test_timed_scan(unsigned short n);

int kbd_subscribe_int();
int kbd_unsubscribe_int();
int kbd_int_handler(unsigned short);

void processOutputCode(unsigned short scancode);
void processDualOutputCode(unsigned short firstByte, unsigned short scancode);

#endif /* __KEYBOARD_H */
