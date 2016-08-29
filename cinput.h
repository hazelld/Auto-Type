#ifndef _CINPUT_H_
#define _CINPUT_H_

/*	cinput.h
 *
 *	Author: William Hazell
 *
 *	Interface for cinput virtual driver that uses the 
 *	uinput virtual device. Provides functionality for 
 *	writing key & two keypresses ( ie. key and shiftkey )
 *
 */


/*
 *	There are some important definitions in /usr/include/linux/input.h that should
 *	be noted here:
 *
 *	#define EV_KEY		0x01
 *	#define EV_REL		0x02
 *	#define EV_ABS		0x03
 *
 *	EV_KEY represents a key press and release event
 *	EV_REL represents a relative axis event (mouse movements)
 *	EV_ABS represents absolute axis event (touch screen)
 *
 *	For the scope of the current project, the main event that will be used is
 *	the keypress event. To allow the uinput file descriptor to be able to 
 *	receive keycodes they need to be set up using the following:
 *
 *	ret = ioctl(fd, UI_SET_EVBIT, EV_KEY)
 *
 *	fd 			 => File descriptor for uinput
 *	UI_SET_EVBIT => What we are setting (ie for EV_KEY this would be UI_SET_KEYBIT)
 *	EV_KEY		 => The keycode to enable (defined in input.h) ie. KEY_D
 *
 */

/*	
 *	How the communication is done to the uinput fd:
 *
 *	First, we need to create a struct uinput_user_dev, which is defined in 
 *	uinput.h. The definition is as follows (omitting the non-needed stuff):
 *
 *	struct uinput_user_dev {
 *
 *		char name[UINPUT_MAX_NAME_SIZE];
 *		struct input_id id;
 *		int absmax[ABS_MAX + 1];
 *		int absmin[ABS_MIN + 1];
 *	}
 *
 *	name	=> What we are naming the input device
 *	id		=> Internal linux structure that has bustype, vendor id, product id etc..
 *	abs...	=> Defines the min and max values for absolute axis
 *
 *	Once this structure is created, we need to write struct into the uinput fd:
 *	
 *	ret = write(fd, &uinput_user_dev, sizeof(struct ...))
 *	
 *	Then create the device using the UI_DEV_CREATE ioctl request:
 *	ret = ioctl(fd, UI_DEV_CREATE);
 *
 *	Now, fd represents the file descriptor of the new input device.
 *
 *
 * 	To write to this fd, we use the input_event structure that is defined as follows:
 *		
 *	type 	=> Event type (EV_xxx see defines above)
 *	code	=> Key code of the event 
 *	value   => For EV_KEY this is: 1 (press) or 0 (release)
 *
 *
 *	Then we write this event to the fd:
 *	
 *	write(fd, &input_event, sizeof(...));
 *
 *
 *	Input device is then destroyed with:
 *		ioctl(fd, UI_DEV_DESTROY)
 */


/* */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>

/* Bundle the keypresses together */
typedef struct keypress {
	int key1;
	int key2; // if 0 then no second key needed 
} KeyPress;

/* This function allows for easy setting of the values */
struct keypress set_keys (int key1, int key2);

/* emit_keyclick
 *	
 *	This function is the interface for emitting a keypress event
 *	to the uinput device. It is important to note that if it is 
 *	a dual press, with the shift key being one of the clicks, then 
 *	it must be key2 in the struct keypress. This is because it must 
 *	actually be "held" during the press event of the next key.
 *	
 *	Arguments:
 *		fd	=> File descriptor of uinput device
 *		key	=> struct keypress of the key events to be pressed
 *
 *	Returns:
 *		!0 	=> Failed to emit, or improper struct keypress
 *
 */
int emit_keyclick(int fd, struct keypress*);

/* write_event
 *
 * 	Write an event to the given fd. Check to ensure the write
 * 	was successful. Uses the input_event structure with the 
 * 	write() function.
 *
 * 	Arguments:
 * 		fd		=>	File Descriptor of uinput device
 * 		event	=>	Event struct that holds the info about the event
 *
 * 	Returns
 * 	 	!0		=> Failed to write the event. Check errno.
 *
 */
int write_event(int fd, struct input_event* event);

/* enable_events
 *
 * 	Before events can be used, they must be enabled through the 
 * 	ioctl interface. For now, only the UI_SET_KEYBIT will be set.
 * 	Eventually we can set the UI_SET_ABSBIT to allow for mouse movement
 * 	events to be created.
 *
 * 	Arguments:
 * 		fd		=>	File descriptor of uinput dev
 * 		ev_type =>	Set the EV type. May be the following
 * 						EV_KEY -> Key events
 * 						EV_ABS -> Absolute axis event
 * 						EV_REL -> Relative axis event
 * 		ev_code	=>	The code that should be enabled. These are defined in 
 * 					linux/input.h
 * 	
 * 	Returns:
 * 		!0 		=> Error occured trying to enable the event. Check errno.
 *
 */
int enable_event(int fd, uint16_t ev_type, uint16_t ev_code);


/* open_fd:
 * 
 * 	This function opens a file descriptor for the uinput module. 
 * 	Note it only checks for /dev/uinput, however depending on the 
 * 	linux distro this may be called something else. Eventually
 * 	this should be changed to check for the variations.
 *
 *	Returns:
 *		int => File descriptor for uinput dev
 */
int open_fd (void);

/* destroy_fd:
 *
 * 	Safely destroy the fd that is opened in the open_fd function.
 * 	If there is an error with the closing function, the errno is 
 * 	preserved and the fd is forced closed with close().
 *
 * 	Arguments:
 * 		fd 	=> The fd to try and close
 *
 * 	Returns:
 * 		0 	=> Successfully closed the fd
 * 		!0	=> Did not successfully close the fd. Check errno
 *
 */
int destroy_fd (int fd);

/* create:
 *
 * 	This function writes the information for struct uinput_user_dev 
 * 	to the uinput module. This needs to happen prior to writing everything
 * 	else. This tells uinput information about the interface that is using it.
 *	
 *	Arguments:
 *		fd		=>	File descriptor opened with open_fd()
 *		uidev	=> 	User device struct created with get_uidev()
 *
 *	Returns:
 *		0	 	=> Successfully wrote struct info to uinput
 *		!0		=> Could not write to uinput. Check errno
 */
int create (int fd, struct uinput_user_dev* uidev);

/* emit_syn:
 *
 * 	This function is used emit a synchronization event that is needed
 * 	to allow the keypress event to be registered. When creating events
 * 	through the emit_keyclick() interface this is automatically done. If
 * 	using the emit() function directly then this must be called after.
 *	
 *	Arguments:
 *		fd 	=>	File descriptor for the uinput device
 */
int emit_syn (int fd);


/* emit
 *
 * 	Handles the emitting of an event by setting up the input_event structure
 * 	based on the arguments provided. This attempts to write the requested 
 * 	event to the uinput device.
 *
 * 	Arguments:
 * 		fd		=>	File descriptor for the uinput device
 * 		ev_type	=>	Type of event (ie. EV_KEY, EV_ABS) as defined in linux/input.h
 * 		ev_code	=>	Event code to write to uinput
 * 		ev_value=>	Specific value, for example 1 for keypress, 0 for release. The
 * 					values for EV_ABS and EV_REL are different
 * 	
 * 	Returns the result of the write_event() call
 */
int emit (int fd, uint16_t ev_type, uint16_t ev_code, int32_t ev_value);


/*  get_uidev:
 *
 *  This function creates the uinput_user_dev structure in memory. 
 *  For now, set to default values, however eventually make this more
 *  of a constructor.
 *
 *  Returns:
 *	  struct uinput_user_dev* => Note caller is responsible for freeing
 */
struct uinput_user_dev* get_uidev(void);


#endif /* _CINPUT_H_*/
