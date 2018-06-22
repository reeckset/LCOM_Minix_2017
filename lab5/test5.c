#include <stdint.h>
#include <machine/int86.h> // /usr/src/include/arch/i386
#include <minix/drivers.h>
#include <minix/driver.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <sys/mman.h>
#include "lmlib.h"
#include "test5.h"
#include "macros.h"
#include "video_gr.h"
#include "video_test.h"
#include "keyboard.h"
#include "timer.h"
#include "vbe.h"

void *video_test_init(unsigned short mode, unsigned short delay) {

	void* video_mem = vg_init(mode);
	sleep(delay);

	vg_exit();

	return video_mem;
}



int video_test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	//changing to video mode
	vg_init(GRAPHIC_MODE);
	int xDraw, yDraw;
	for(xDraw = 0; xDraw <= size/2; xDraw++){
		for(yDraw = 0; yDraw <= size/2; yDraw++){
			video_draw_pixel(x + xDraw, y + yDraw, color);
			video_draw_pixel(x - xDraw, y + yDraw, color);
			video_draw_pixel(x + xDraw, y - yDraw, color);
			video_draw_pixel(x - xDraw, y - yDraw, color);
		}
	}


	video_dump_fb();

	kbd_get_key(KEY_ESC);

	vg_exit();

	return 0;

}

int video_test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {
	vg_init(GRAPHIC_MODE);

	if(abs(yf - yi) < abs(xf - xi)){ //If slope is less than 45º
		if(xi >= xf){ //Swap points if xf is before xi
			int tmp = xf;
			xf = xi;
			xi = tmp;
			tmp = yf;
			yf = yi;
			yi = tmp;
		}
		float m = (float)(yf - yi)/(float)(xf - xi); //m is the Slope
		int xDraw;
		for(xDraw = xi; xDraw <= xf; xDraw++){
			video_draw_pixel(xDraw, yi + xDraw * m, color);
		}
	}else{//Slope is bigger than 45º
		if(yi >= yf){ //Swap points if yf is before yi
			int tmp = xf;
			xf = xi;
			xi = tmp;
			tmp = yf;
			yf = yi;
			yi = tmp;
		}
		float m = (float)(xf - xi)/(float)(yf - yi); //m is the Slope
		int yDraw;
		for(yDraw = yi; yDraw <= yf; yDraw++){
			video_draw_pixel(xi + yDraw * m, yDraw, color);
		}
	}

	video_dump_fb();

	kbd_get_key(KEY_ESC);

	vg_exit();

	return 0;

}

int video_test_xpm(char *xpm[], unsigned short xi, unsigned short yi) {

		vg_init(GRAPHIC_MODE);


    video_draw_xpm(xpm, xi, yi);

    kbd_get_key(KEY_ESC);

    vg_exit();

    return 0;

}

int video_draw_xpm(char *xpm[], unsigned short xi, unsigned short yi) {

	int width, height;

	char *pixmap = read_xpm(xpm, &width, &height);

	int x, y;
	for(x = 0; x < width; x++){
			for(y = 0; y < height; y++){
					video_draw_pixel(x + xi,y + yi,pixmap[x+width*y]);
			}
	}

	video_dump_fb();

	return 0;

}

int video_test_move(char *xpm[], unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, short speed, short frame_rate) {
	if(frame_rate > 60) frame_rate = 60; //Due to timer frequency limitations

	vg_init(GRAPHIC_MODE);
	int stopFlag = 0; //Flag that is activated when the user releases ESC key

	int timerCounter = 0; //incremented 60 times/second [DEFAULT - 60hz timer freq]
	short updateFreq = TIMER0_FREQ / frame_rate; // Gives how many counter ticks must pass to update to the next frame
	short speedCounter; // to be used to calculate the time to move an object, without changing the initial variable

	if(speed < 0) {
		speedCounter = speed + 1; // necessary for the number of steps to be correct conforming to the algorithm used below
	} else speedCounter = speed;

	struct Position CurrentPosition;
	CurrentPosition.x = xi;
	CurrentPosition.y = yi;

	//subscribe keyboard interrupts
	int initialHookID_KBD = kbd_subscribe_int();

	if(initialHookID_KBD == -1) return 1;

	//subscribe Timer interrupts
	int initialHookID_TIMER = timer_subscribe_int();

	if(initialHookID_TIMER == -1) return 1;

	int ipc_status, r;
	message msg;

	while(!stopFlag) {
		// Get a request message.
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
			printf("driver_receive failed with: %d\n", r);
			continue;//continue while loop
		}
		if (is_ipc_notify(ipc_status)) { // received notification
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: // hardware interrupt notification
					if (msg.NOTIFY_ARG & BIT(initialHookID_KBD)) { // subscribed interrupt
						if(kbd_int_handler(KEY_ESC) == 0) stopFlag = 1;
					}

					//if the sprite has completed the animation,
					//there's no need to bother with the rest of the cycle, only wait for the key press
					if(CurrentPosition.x == xf && CurrentPosition.y == yf) continue;

					if (msg.NOTIFY_ARG & BIT(initialHookID_TIMER)) { // subscribed interrupt
						timerCounter++;
					}
					if(timerCounter == updateFreq) { // If enough time has passed to go to the next frame, the next frame will be drawn

						if(xi == xf) { //vertical movement
							if(yf > yi){ //from top to bottom
								if (speed > 0) {
									//if the speed is positive the sprite will move speed units each frame,
									//unless the distance between the final point and the current one is less than speed

									if(yf - CurrentPosition.y < speed) {
										//If the increment passes the needed distance, the sprite will only move the necessary distance

										//clear screen
										clearScreen();

										video_draw_xpm(xpm, xf, yf);
									} else {
										CurrentPosition.y += speedCounter;
										//clear screen
										clearScreen();
										video_draw_xpm(xpm, xf, CurrentPosition.y);
									}
								} else if(speed < 0) {
									//if the speed is negative the sprite will move 1 unit only after |speed| frames passed,

									if((yf - CurrentPosition.y) > 0) { //If the sprite hasn't reached the final point

										if(speedCounter == 1) {
											CurrentPosition.y += speedCounter;
											//clear screen
											clearScreen();
											video_draw_xpm(xpm, xf, CurrentPosition.y);
											speedCounter = speed + 1;
										} else {
											speedCounter++;
										}
									}
								}
							} else { //from bottom to top

								if (speed > 0) {
									//if the speed is positive the sprite will move speed units each frame,
									//unless the distance between the final point and the current one is less than speed

									if((CurrentPosition.y - yf) < speed) {
										//If the increment passes the needed distance, the sprite will only move the necessary distance
										//clear screen
										clearScreen();
										video_draw_xpm(xpm, xf, yf);
									} else {
										CurrentPosition.y -= speedCounter;
										//clear screen
										clearScreen();
										video_draw_xpm(xpm, xf, CurrentPosition.y);
									}
								} else if(speed < 0) {
									//if the speed is negative the sprite will move 1 unit only after |speed| frames passed,

									if((CurrentPosition.y - yf) > 0) { //If the sprite hasn't reached the final point

										if(speedCounter == 1) {
											CurrentPosition.y -= speedCounter;
											//clear screen
											clearScreen();
											video_draw_xpm(xpm, xf, CurrentPosition.y);
											speedCounter = speed + 1;
										} else {
											speedCounter++;
										}
									}
								}
							}
						} else { //horizontal movement

							if (xf > xi) { //from left to right

								if (speed > 0) {
									//if the speed is positive the sprite will move speed units each frame,
									//unless the distance between the final point and the current one is less than speed

									if(xf - CurrentPosition.x < speed) {
										//If the increment passes the needed distance, the sprite will only move the necessary distance
										//clear screen
										clearScreen();
										video_draw_xpm(xpm, xf, yf);
									} else {
										CurrentPosition.x += speedCounter;
										//clear screen
										clearScreen();
										video_draw_xpm(xpm, CurrentPosition.x, yf);
									}
								} else if(speed < 0) {
									//if the speed is negative the sprite will move 1 unit only after |speed| frames passed,

									if((xf - CurrentPosition.x) > 0) { //If the sprite hasn't reached the final point

										if(speedCounter == 1) {
											CurrentPosition.x += speedCounter;
											//clear screen
											clearScreen();
											video_draw_xpm(xpm, CurrentPosition.x, yf);
											speedCounter = speed + 1;
										} else {
											speedCounter++;
										}
									}
								}
							} else { //from right to left

								if (speed > 0) {
									//if the speed is positive the sprite will move speed units each frame,
									//unless the distance between the final point and the current one is less than speed

									if((CurrentPosition.x - xf) < speed) {
										//If the increment passes the needed distance, the sprite will only move the necessary distance
										//clear screen
										clearScreen();
										video_draw_xpm(xpm, xf, yf);
									} else {
										CurrentPosition.x -= speedCounter;
										//clear screen
										clearScreen();
										video_draw_xpm(xpm, CurrentPosition.x, yf);
									}

								} else if(speed < 0) {
									//if the speed is negative the sprite will move 1 unit only after |speed| frames passed,

									if((CurrentPosition.x - xf) > 0) { //If the sprite hasn't reached the final point

										if(speedCounter == 1) {
											CurrentPosition.x -= speedCounter;
											//clear screen
											clearScreen();
											video_draw_xpm(xpm, CurrentPosition.x, yf);
											speedCounter = speed + 1;
										} else {
											speedCounter++;
										}
									}
								}
							}
						}

						//reset timerCounter
						timerCounter = 0;



					}
					break;
				default:
					break; // no other notifications expected: do nothing
			}
		} else { // received a standard message, not a notification
			// no standard messages expected: do nothing
		}
	}

	vg_exit();
	return 0;

}

int video_test_controller() {
	vbe_info_t vbeInfo;
	if(vg_get_vbe_info(&vbeInfo) != OK) return 1;

	unsigned char majorversion, minorversion;;


	majorversion = (vbeInfo.VbeVersion) >> 8;
	minorversion = (vbeInfo.VbeVersion & 0x0011);
	printf("%x.%x \n", majorversion, minorversion);

	unsigned short * videoModes = (unsigned short *) vbeInfo.reservedArea;

	int i;
	for(i=0; videoModes[i] != 0xFFFF; i++){
		printf("0x%x:", videoModes[i]);
	}

	printf("\n%d\n", vbeInfo.totalMem*64);


	return 0;
}
