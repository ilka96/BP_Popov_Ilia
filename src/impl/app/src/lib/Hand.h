/**
 * @file Hand.h
 * @author popovili@fit.cvut.cz
 * @brief  Library to controll robotic hand
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <time.h> 

/// Structure represents connected device
typedef struct {     

	int dev_addr;
	bool conn_status;
	bool readyForData;

} SArdDev; 

/// Structure responsible for storing communication settings
typedef struct  {

		struct termios tio;
		struct termios stdio;
		struct termios old_stdio;
 		int RTS_flag;
 		int DTR_flag;

} SSerial;

/**
* @brief  Make communication settings and store original settings
* @param SSerial *term  container for communication settings
*/
void init_serial (SSerial *term);

/**
* @brief  Sets I/O baud rate and RTS, DTR flags
* @param int tty_fd  device addres
* @param SSerial *term  container for communication settings
*/
void set_io_speed (int tty_fd, SSerial *term);

/**
* @brief  Open device for communication  
* @param SArdDev* dev  container for connected device
* @param const char* addr  device addres
*/
bool open_device (SArdDev* dev, const char* addr);

/**
* @brief Close device
* @param int tty_fd  device addres
* @param SSerial *term container for communication settings
*/
void close_device (int tty_fd, SSerial *term);

/**
* @brief Return original communication settings
* @param SSerial *term container for communication settings
*/
void destr_serial (SSerial *term);

/**
* @brief Reads data from connected device via serial line
* @param SArdDev dev device
* @param char* buff container for recived data
* @param int size size of buff
* @return bool value, true if reciving was succesfull
*/
bool read_from_device (SArdDev dev, char* buff, int size);

/**
* @brief Reads actual fingers positions from glove controller 
* @param SArdDev dev glove
* @param char* buff container for recived data
* @param int size size of buff
* @param bool wait_glove waiting for respond
* @return bool value, true if reciving was succesfull
*/
bool read_glove_position (SArdDev dev, char* buff, int size, bool wait_glove);

/**
* @brief Send data to device
* @param SArdDev dev glove
* @param char* buff container for sending data
* @param int size size of buff
*/
void write_to_device (SArdDev* dev, char* buff, int size); 

/**
* @brief Resends data between 2 devices
* @param SArdDev* fromArd, sending device
* @param SArdDev* toArd, target device
*/
void resend_data_between_device (SArdDev* fromArd, SArdDev* toArd);

/**
* @brief Sends servo target positions to hand
* @param SArdDev* dev, hand
* @param int states[6], container for servo states
*/
void send_states_to_hand (SArdDev* dev, int states[6]);

/**
* @brief Sends command to open hand
* @param SArdDev* dev, hand
*/
void open_hand (SArdDev* dev);

/**
* @brief Sends command to close hand
* @param SArdDev* dev, hand
*/
void close_hand (SArdDev* dev);
