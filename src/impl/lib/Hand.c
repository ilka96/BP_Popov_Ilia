//
// Created by popovili@fit.cvut.cz .
//


#include "Hand.h"

void init_serial (SSerial *term) {

	tcgetattr(STDOUT_FILENO,&term->old_stdio);

	memset(&term->stdio,0,sizeof(term->stdio));
	term->stdio.c_iflag=0;
	term->stdio.c_oflag=0;
 	term->stdio.c_cflag=0;
 	term->stdio.c_lflag=0;
 	term->stdio.c_cc[VMIN]=1;
 	term->stdio.c_cc[VTIME]=0;
	tcsetattr(STDOUT_FILENO,TCSANOW,&term->stdio);
 	tcsetattr(STDOUT_FILENO,TCSAFLUSH,&term->stdio);
 	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // make the reads non-blocking
 
 	memset(&term->tio,0,sizeof(term->tio));

 	term->tio.c_iflag=0;
 	term->tio.c_oflag=0;
 	term->tio.c_cflag=CS8|CREAD|CLOCAL; // 8n1
	term->tio.c_lflag=0;
	term->tio.c_cc[VMIN]=1;
 	term->tio.c_cc[VTIME]=5;
	
}

void set_io_speed (int tty_fd, SSerial *term) {

 	cfsetospeed(&term->tio,B9600); // 9600 baud
 	cfsetispeed(&term->tio,B9600);

	term->RTS_flag = TIOCM_RTS;
 	term->DTR_flag = TIOCM_DTR;
 	ioctl(tty_fd,TIOCMBIS,&term->RTS_flag);
 	ioctl(tty_fd,TIOCMBIS,&term->DTR_flag);

 	tcsetattr(tty_fd,TCSANOW,&term->tio);

}

bool open_device (SArdDev* dev, const char* addr) {

	dev->dev_addr = open(addr, O_RDWR | O_NONBLOCK); 
	if (dev->dev_addr > 0) {
		dev->conn_status = true;
		return true;
	}

	return false;

}

void close_device (int tty_fd, SSerial *term) {

 	ioctl(tty_fd,TIOCMBIC,&term->RTS_flag);
 	ioctl(tty_fd,TIOCMBIC,&term->DTR_flag);
 	close(tty_fd);

}

void destr_serial (SSerial *term) {

	tcsetattr(STDOUT_FILENO,TCSANOW,&term->old_stdio);
	
}

bool read_from_device (SArdDev dev, char* buff, int size) {
	char c;
	int ind = 0;	

if (read(dev.dev_addr, &c, 1) >0) {
	if (c == 'S') {
		buff[ind] = c;
		ind++;
		while (ind <= size) {
			if (read(dev.dev_addr,&c,1)>0) {
				buff[ind] = c;
				ind++;
				if (c == 'E') {
					return true;
				}
			}
		}
		return false;
	}
}
	return false;

}

bool read_glove_position (SArdDev dev, char* buff, int size, bool wait_glove) {

char str[3] = {"RDY"};

	if (!wait_glove)
		write (dev.dev_addr, str, 3);

	return read_from_device (dev, buff, size);

}



void write_to_device (SArdDev *dev, char* buff, int size) {

	write (dev->dev_addr, buff, size);
}

void resend_data_between_device (SArdDev* fromArd, SArdDev* toArd) {

	char b[30];
 	if (read_from_device(*fromArd, b, 30))     {  
				write_to_device(toArd,b,30);
			}
}

void send_states_to_hand (SArdDev* dev, int states[6]) {

	char str[30];
	sprintf(str, "S:%d,%d,%d,%d,%d,%d:E", states[0], states[1], states[2], states[3], states[4], states[5]); 

	write_to_device(dev, str,  sizeof(str));
}


void open_hand (SArdDev* dev) {

	char str[] = "S:255,255,255,255,255,255:E";  
	write_to_device(dev, str, sizeof(str));

}

void close_hand (SArdDev* dev) {

	char str[] = "S:0,0,0,0,0,-1:E";  
	write_to_device(dev, str, sizeof(str));

}


