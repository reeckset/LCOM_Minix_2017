#include <minix/drivers.h>
#include "test5.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "video_test.h"
#include "pixmap.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static long parse_long(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	/* Initializing service */
	sef_startup();

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"init <hexadecimal no.- mode> <decimal no.- delay>\"\n"
			"\t service run %s -args \"test-square <decimal no.- x> <decimal no.- y> <decimal no.- size> <decimal no.- color>\"\n"
			"\t service run %s -args \"test-line <decimal no. - xi> <decimal no.- yi> <decimal no.- xf> <decimal no.- yf> <decimal no.- color>\"\n"
			"\t service run %s -args \"test-xpm <decimal no. - pic-option [1-5]> <decimal no.- xi> <decimal no.- yi>\"\n"
			"\t service run %s -args \"test-move <decimal no. - pic-option [1-5]> <decimal no.- xi> <decimal no.- yi> <decimal no.- xf> <decimal no.- yf> <decimal no.- speed> <decimal no.- frame-rate>\"\n"
			"\t service run %s -args \"test-controller\"\n",
			argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned long mode, delay, x, y, size, color, xi, yi, xf, yf, picOption, frame_rate;
	long speed;

	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if (argc != 4) {
			printf("init: wrong no. of arguments for video_test_init()\n");
			return 1;
		}
		mode = parse_ulong(argv[2], 16);
/* Parses string to unsigned long */
		delay = parse_ulong(argv[3], 10);
		if (mode == ULONG_MAX)
			return 1;
		if (delay == ULONG_MAX)
			return 1;
		printf("video::video_test_init(%lu, %lu)\n", mode, delay);
		video_test_init(mode, delay);
		return 0;
	}
	else if (strncmp(argv[1], "test-square", strlen("test-square")) == 0) {
		if (argc != 6) {
			printf("test-square: wrong no. of arguments for video_test_square()\n");
			return 1;
		}
		x = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		y = parse_ulong(argv[3], 10);					/* Parses string to unsigned long */
		size = parse_ulong(argv[4], 10);					/* Parses string to unsigned long */
		color = parse_ulong(argv[5], 10);					/* Parses string to unsigned long */

		if (x == ULONG_MAX)
			return 1;
		if (y == ULONG_MAX)
			return 1;
		if (size == ULONG_MAX)
			return 1;
		if (color == ULONG_MAX)
			return 1;
		printf("video::video_test_square(%lu, %lu, %lu, %lu)\n", x, y, size, color);
		return video_test_square(x, y , size, color);
	}
	else if (strncmp(argv[1], "test-line", strlen("test-line")) == 0) {
		if (argc != 7) {
			printf("test-line: wrong no of arguments for video_test_line()\n");
			return 1;
		}
		xi = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		yi = parse_ulong(argv[3], 10);						/* Parses string to unsigned long */
		xf = parse_ulong(argv[4], 10);						/* Parses string to unsigned long */
		yf = parse_ulong(argv[5], 10);						/* Parses string to unsigned long */
		color = parse_ulong(argv[6], 10);						/* Parses string to unsigned long */
		if (xi == ULONG_MAX)
			return 1;
		if (yi == ULONG_MAX)
			return 1;
		if (xf == ULONG_MAX)
			return 1;
		if (yf == ULONG_MAX)
			return 1;
		if (color == ULONG_MAX)
			return 1;
		printf("video::video_test_line(%lu, %lu, %lu, %lu, %lu)\n", xi, yi, xf, yf, color);
		return video_test_line(xi, yi, xf, yf, color);
	}
	else if (strncmp(argv[1], "test-xpm", strlen("test-xpm")) == 0) {

		if (argc != 5) {
			printf("test-xpm: wrong no of arguments for video_test_xpm()\n");
			return 1;
		}
		picOption = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		xi = parse_ulong(argv[3], 10);								/* Parses string to unsigned long */
		yi = parse_ulong(argv[4], 10);								/* Parses string to unsigned long */

		if (picOption == ULONG_MAX)
			return 1;
		if (xi == ULONG_MAX)
			return 1;
		if (yi == ULONG_MAX)
			return 1;

		switch(picOption) {
		case 1:
			printf("video::video_test_xpm(pic%lu, %lu, %lu)\n", picOption, xi, yi);
			return video_test_xpm(pic1, xi, yi);
		case 2:
			printf("video::video_test_xpm(pic%lu, %lu, %lu)\n", picOption, xi, yi);
			return video_test_xpm(pic2, xi, yi);
		case 3:
			printf("video::video_test_xpm(pic%lu, %lu, %lu)\n", picOption, xi, yi);
			return video_test_xpm(cross, xi, yi);
		case 4:
			printf("video::video_test_xpm(pic%lu, %lu, %lu)\n", picOption, xi, yi);
			return video_test_xpm(pic3, xi, yi);
		case 5:
			printf("video::video_test_xpm(pic%lu, %lu, %lu)\n", picOption, xi, yi);
			return video_test_xpm(penguin, xi, yi);
		default:
			printf("test-xpm: invalid pic-option [1-5] \n");
			return 1;
		}
	}
	else if (strncmp(argv[1], "test-move", strlen("test-move")) == 0) {
		if (argc != 9) {
			printf("move-xpm: wrong no of arguments for video_move_xpm()\n");
			return 1;
		}
		picOption = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		xi = parse_ulong(argv[3], 10);								/* Parses string to unsigned long */
		yi = parse_ulong(argv[4], 10);								/* Parses string to unsigned long */
		xf = parse_ulong(argv[5], 10);								/* Parses string to unsigned long */
		yf = parse_ulong(argv[6], 10);								/* Parses string to unsigned long */
		speed = parse_long(argv[7], 10);								/* Parses string to long */
		frame_rate = parse_ulong(argv[8], 10);								/* Parses string to unsigned long */

		if (picOption == ULONG_MAX)
			return 1;
		if (xi == ULONG_MAX)
			return 1;
		if (yi == ULONG_MAX)
			return 1;
		if (xf == ULONG_MAX)
			return 1;
		if (yf == ULONG_MAX)
			return 1;
		if (speed == LONG_MAX)
			return 1;
		if (frame_rate == ULONG_MAX)
			return 1;

		switch(picOption) {
		case 1:
			printf("video::video_test_move(pic%lu, %lu, %lu, %lu, %lu, %li, %lu,)\n", picOption, xi, yi, xf, yf, speed, frame_rate);
			return video_test_move(pic1, xi, yi, xf, yf, speed, frame_rate);
		case 2:
			printf("video::video_test_move(pic%lu, %lu, %lu, %lu, %lu, %li, %lu,)\n", picOption, xi, yi, xf, yf, speed, frame_rate);
			return video_test_move(pic2, xi, yi, xf, yf, speed, frame_rate);
		case 3:
			printf("video::video_test_move(pic%lu, %lu, %lu, %lu, %lu, %li, %lu,)\n", picOption, xi, yi, xf, yf, speed, frame_rate);
			return video_test_move(cross, xi, yi, xf, yf, speed, frame_rate);
		case 4:
			printf("video::video_test_move(pic%lu, %lu, %lu, %lu, %lu, %li, %lu,)\n", picOption, xi, yi, xf, yf, speed, frame_rate);
			return video_test_move(pic3, xi, yi, xf, yf, speed, frame_rate);
		case 5:
			printf("video::video_test_move(pic%lu, %lu, %lu, %lu, %lu, %li, %lu,)\n", picOption, xi, yi, xf, yf, speed, frame_rate);
			return video_test_move(penguin, xi, yi, xf, yf, speed, frame_rate);
		default:
			printf("test-xpm: invalid pic-option [1-5] \n");
			return 1;
		}
	}
	else if (strncmp(argv[1], "test-controller", strlen("test-controller")) == 0) {
		if (argc != 2) {
			printf("test-controller: wrong no. of arguments for video_test_controller()\n");
			return 1;
		}
		printf("video::video_test_controller()\n");
		return video_test_controller();
	}
	else {
		printf("video: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("video: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static long parse_long(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtol(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == LONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtol");
		return LONG_MAX;
	}

	if (endptr == str) {
		printf("video: parse_long: no digits were found in %s\n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
}
