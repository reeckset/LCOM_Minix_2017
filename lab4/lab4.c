/*#include <minix/drivers.h>
#include "test4.h"



int main(int argc, char **argv)
{
	//Initializing service
	sef_startup();

	// Enable IO-sensitive operations for ourselves
	sys_enable_iop(SELF);

	mouse_test_gesture(100);

	return 0;
}*/

#include <minix/drivers.h>
#include "test4.h"
#include <limits.h>
#include <string.h>
#include <errno.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
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
			"\t service run %s -args \"test-packet <decimal no.- no. of packets>\"\n"
			"\t service run %s -args \"test-remote <decimal no.- period> <decimal no.- no. of packets>\"\n"
			"\t service run %s -args \"test-async <decimal no. - idle time>\"\n"
			"\t service run %s -args \"test-gesture <decimal no.- no. of packets>\"\n",
			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned long period;
	unsigned short nPackets, idleTime;
	short length;

	if (strncmp(argv[1], "test-packet", strlen("test-packet")) == 0) {
		if (argc != 3) {
			printf("test-packet: wrong no. of arguments for mouse_test_packet()\n");
			return 1;
		}
		nPackets = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (nPackets == ULONG_MAX)
			return 1;
		printf("mouse::mouse_test_packet(%lu)\n", nPackets);
		return mouse_test_packet((unsigned short) nPackets);
	}
	else if (strncmp(argv[1], "test-remote", strlen("test-remote")) == 0) {
		if (argc != 4) {
			printf("test-remote: wrong no. of arguments for mouse_test_remote()\n");
			return 1;
		}
		period = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		nPackets = parse_ulong(argv[3], 10);					/* Parses string to unsigned long */
		if (period == ULONG_MAX)
			return 1;
		if (nPackets == ULONG_MAX)
			return 1;
		printf("mouse::mouse_test_remote(%lu, %lu)\n", period, nPackets);
		return mouse_test_remote(period,(unsigned short) nPackets);
	}
	else if (strncmp(argv[1], "test-async", strlen("test-async")) == 0) {
		if (argc != 3) {
			printf("test-async: wrong no of arguments for mouse_test_async()\n");
			return 1;
		}
		idleTime = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (idleTime == ULONG_MAX)
			return 1;
		printf("mouse::mouse_test_async(%lu)\n", idleTime);
		return mouse_test_async((unsigned short) idleTime);
	}
	else if (strncmp(argv[1], "test-gesture", strlen("test-gesture")) == 0) {
			if (argc != 3) {
				printf("test-gesture: wrong no of arguments for mouse_test_gesture()\n");
				return 1;
			}
			length = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
			if (length == ULONG_MAX)
				return 1;
			printf("mouse::mouse_test_gesture(%lu)\n", length);
			return mouse_test_gesture((short) length);
		}
	else {
		printf("mouse: %s - no valid function!\n", argv[1]);
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
		printf("timer: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
