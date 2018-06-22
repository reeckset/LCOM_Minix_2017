#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

int kbd_get_key(unsigned short stopCode);

int kbd_subscribe_int();
int kbd_unsubscribe_int();
int kbd_int_handler(unsigned short);

void processOutputCode(unsigned short scancode);
void processDualOutputCode(unsigned short firstByte, unsigned short scancode);

#endif /* __KEYBOARD_H */
