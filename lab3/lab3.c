#include <minix/drivers.h>
#include "keyboard.h"



int main(int argc, char **argv)
{
	/* Initializing service */
	sef_startup();

	/* Enable IO-sensitive operations for ourselves */
	sys_enable_iop(SELF);



	return 0;
}



