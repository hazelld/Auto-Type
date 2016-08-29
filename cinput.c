#include "cinput.h"

/* cinput.c
 *
 * Author:	William Hazell
 *
 * Implementation of the virtual device driver for the 
 * user input device (uinput)
 *
 */

/* Return a struct keypress with proper information */
struct keypress set_keys(int key1, int key2) {
	struct keypress buff = { key1, key2 };
	return buff;
}

/* get_uidev */
struct uinput_user_dev* get_uidev(void) {

	struct uinput_user_dev* uidev;
	uidev = malloc(sizeof(struct uinput_user_dev));
	memset(uidev, 0, sizeof(struct uinput_user_dev));
	strcpy(uidev->name, "Auto-Typer");
	return uidev;
}

/* open_fd */
int open_fd(void) {
	return open("/dev/uinput", O_WRONLY | O_NONBLOCK); 
}

/* destroy_fd */
int destroy_fd (int fd) {
 
	/* If ioctl has an error, we need to still close the fd, 
	 * however we want to preserve errno, so save it before the close,
	 * then restore it after.
	 */
	if (ioctl(fd, UI_DEV_DESTROY) < 0) {
		int old_errno = errno;
		close(fd);
		errno = old_errno;
		return -1;
	}

	if (close(fd) < 0) 
		return -1;

	return 0;
}


/* create */
int create (int fd, struct uinput_user_dev* uidev) {
	
	size_t bytes_written;

	bytes_written = write(fd, uidev, sizeof(struct uinput_user_dev));

	if (bytes_written != sizeof(struct uinput_user_dev))
		return -1;

	if (ioctl(fd, UI_DEV_CREATE) < 0)
		return -1;

	return 0;
}

/* write_event */
int write_event (int fd, struct input_event* event) {

	size_t bytes_written;

	bytes_written = write(fd, event, sizeof(struct input_event));

	if (bytes_written != sizeof(struct input_event))
		return -1;

	return 0;
}

/* enable_event */
int enable_event (int fd, uint16_t ev_type, uint16_t ev_code) {
	
	if (ioctl(fd, UI_SET_EVBIT, ev_type) < 0)
		return -1;

	if (ioctl(fd, UI_SET_KEYBIT, ev_code) < 0)
		return -1;

	return 0;
}

/* emit() */
int emit (int fd, uint16_t ev_type, uint16_t ev_code, int32_t ev_value) {
	
	struct input_event ev;
	memset(&ev, 0, sizeof(struct input_event));
	
	ev.type  = ev_type;
	ev.code  = ev_code;
	ev.value = ev_value;

	return write_event(fd, &ev);
}


/* emit_keyclick() */
int emit_keyclick (int fd, struct keypress* key) {
	
	if (key->key1 == 0)
		return -1;

	/* Shift key is always the second key, so push first */
	if (key->key2 != 0)
		emit(fd, EV_KEY, key->key2, 1);

	/* Send the key press down event, then key release */
	emit(fd, EV_KEY, key->key1, 1);
	emit(fd, EV_KEY, key->key1, 0);   

	if (key->key2 != 0)
		emit(fd, EV_KEY, key->key2, 0);
	
	return 0;
}

/* emit_syn() */
int emit_syn (int fd) {   
	emit(fd, EV_SYN, SYN_REPORT, 0);
	return 0;
}

