#ifndef __TEST4_H
#define __TEST4_H

/** @defgroup test4 test4
 * @{
 *
 * Functions for testing the kbd code
 */

/**
 * @brief To test packet reception via interrupts
 *
 * Displays the packets received from the mouse
 * Exits after receiving the number of packets specified in argument
 *
 * @param cnt Number of packets to receive and display before exiting
 *
 * @return Return 0 upon success and non-zero otherwise
 */

int mouse_test_packet(unsigned short cnt);



/**
 * @brief To test handling of more than one interrupt
 *
 *  Similar test_packet() except that it
 *  should terminate if no packets are received for idle_time seconds
 *
 * @param idle_time Number of seconds without packets before exiting
 *
 * @return Return 0 upon success and non-zero otherwise
 */

int mouse_test_async(unsigned short idle_time);


/**
 * @brief To test PS/2 remote mode operation
 *
 * Configures the PS/2 to operate in remote mode
 * Periodically requests a packet from the mouse,
 * and displays the packets received from the mouse
 * Exits after receiving the number of packets specified in argument
 *
 * @param period Period in milliseconds
 * @param cnt Number of packets to receive and display before exiting
 *
 * @return Return 0 upon success and non-zero otherwise
 */

int mouse_test_remote(unsigned long period, unsigned short cnt);

/**
 * @brief To test state machine implementation
 *
 *  Similar mouse_test_packet() except that it
 *  should terminate if user moves the mouse continuously
 *    with a positive slope
 *
 * @param length minimum length of movement (mouse units)
 *  in the x-direction. If positive, upwards movement, otherwise
 *  downwards.
 *
 *
 * @return Return 0 upon success and non-zero otherwise
 */


int mouse_test_gesture(short length);


/**
 * @brief To process mouse interrupt
 *
 * Reads the OUT_BUF, and checks if the byte may correspond to the given byte position (0, 1 or 2)
 * Returns 0 if the byte was valid, 1 otherwise.
 *
 * @param byte Byte in the packet to analyze
 * @param nPacketsReceived Pointer to packets counter
 *
 * @return Return 0 upon success and non-zero otherwise
 */

int mouse_int_handler(unsigned long bufferData, unsigned short *nPacketsReceived);


/**
 * @brief To print a packet to the console
 *
 * Writes the given packet to the console in a user friendly way
 *
 * @param packet unsigned char array with the 3 bytes to show
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int print_packet(unsigned char* packet);

/**
 * @brief Subscribes mouse's interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_subscribe_int();

/**
 * @brief Unsubscribes mouse's interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_unsubscribe_int();

int mouse_send_cmd(unsigned long cmd);
int mouse_send_cmd_raw(unsigned long cmd);
int mouse_enable_stream_mode();
int enable_mouse_int();
int disable_mouse_int();


#endif /* __TEST_4.H */
