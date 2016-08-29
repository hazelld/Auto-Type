#include "auto_type.h"

/* Incase of error, print error message and exit */
#define die(str, args...) do { \
	perror(str); \
	exit(EXIT_FAILURE);\
	} while(0)


/*  enable_all_events:
 *
 *  This function simply calls the enable_event function for
 *  each key that is found in the key_array[]. If there is a 
 *  second key that needs to be activated, then also activate it.
 *  For now this feature is redundent since they are all KEY_LEFTSHIFTs
 *  however, for future purposes it is there.
 *  
 *  Arguments:
 *  
 *	  fd => File descriptor for the uinput module. This should be 
 *			created with the open_fd() function.
 *
 */
int enable_all_events ( int fd ) {
	int i;

	if (enable_event(fd, EV_KEY, KEY_LEFTSHIFT) < 0)
		return -1;
	
	for (i = 0; i < MAX_ASCII_VALUE; i++) {
		
		if ( key_array[i].key1 != 0) {
			if (enable_event(fd, EV_KEY, key_array[i].key1) < 0)
				return -1;
		}

		if ( key_array[i].key2 != 0) 
			if (enable_event(fd, EV_KEY, key_array[i].key2) < 0)
				return -1;   
	}
	return 0;
}


/*	type:
 *		
 *	This function is used to type out the input that is read from a 
 *	file specified.
 */
int type (FILE* fh) {
	
	int fd;
	struct uinput_user_dev* uidev;
	char c;

	/* Perform initial setup */
	uidev = get_uidev();
	fd = open_fd();
	
	if (enable_all_events(fd) < 0)
		die("Could not enable all events");
	
	if (create(fd, uidev) < 0)
		die("Could not create uidev");
	
	/* Timing is weird, if not enough time is given the event won't
	 * register? 
	 *
	 * 50 000 microseconds = 0.05 seconds for reference
	 */	
	int i = 5;
	while (i > 0) {
		printf("Starting typing in %d...\n", i--);
		sleep(1);
	}

	/* Get characters off standard in and emit them */
	while ( fscanf(fh, "%c", &c) == 1) {

		if (c == '\n') {
			usleep(40000);
			continue;
		}

		emit_keyclick(fd, &key_array[(int)c]);
		emit_syn(fd);
		usleep(40000);
	
		if ( key_array[(int)c].key1 == KEY_ENTER )
			usleep(100000);
	}

	destroy_fd(fd);
	free(uidev);
	return 0;
}



/* Set up the main array */
__attribute__((constructor)) void setup (void) {

	for (int i = 0; i < MAX_ASCII_VALUE; i++) 
		key_array[i] = set_keys(0,0);

	/* Space, dash, dot, slash */
	key_array[32] = set_keys(KEY_SPACE, 0);
	key_array[45] = set_keys(KEY_MINUS, 0);
	key_array[46] = set_keys(KEY_DOT, 0);
	key_array[47] = set_keys(KEY_SLASH, 0);

	/* Numbers then colon */
	key_array[48] = set_keys(KEY_0, 0),
	key_array[49] = set_keys(KEY_1, 0),
	key_array[50] = set_keys(KEY_2, 0),
	key_array[51] = set_keys(KEY_3, 0),
	key_array[52] = set_keys(KEY_4, 0),
	key_array[53] = set_keys(KEY_5, 0),
	key_array[54] = set_keys(KEY_6, 0),
	key_array[55] = set_keys(KEY_7, 0),
	key_array[56] = set_keys(KEY_8, 0),
	key_array[57] = set_keys(KEY_9, 0),
	key_array[58] = set_keys(KEY_SEMICOLON, KEY_LEFTSHIFT),

	/* A - Z*/
	key_array[65] = set_keys(KEY_A, KEY_LEFTSHIFT),
	key_array[66] = set_keys(KEY_B, KEY_LEFTSHIFT),
	key_array[67] = set_keys(KEY_C, KEY_LEFTSHIFT),
	key_array[68] = set_keys(KEY_D, KEY_LEFTSHIFT),
	key_array[69] = set_keys(KEY_E, KEY_LEFTSHIFT),
	key_array[70] = set_keys(KEY_F, KEY_LEFTSHIFT),
	key_array[71] = set_keys(KEY_G, KEY_LEFTSHIFT),
	key_array[72] = set_keys(KEY_H, KEY_LEFTSHIFT),
	key_array[73] = set_keys(KEY_I, KEY_LEFTSHIFT),
	key_array[74] = set_keys(KEY_J, KEY_LEFTSHIFT),
	key_array[75] = set_keys(KEY_K, KEY_LEFTSHIFT),
	key_array[76] = set_keys(KEY_L, KEY_LEFTSHIFT),
	key_array[77] = set_keys(KEY_M, KEY_LEFTSHIFT),
	key_array[78] = set_keys(KEY_N, KEY_LEFTSHIFT),
	key_array[79] = set_keys(KEY_O, KEY_LEFTSHIFT),
	key_array[80] = set_keys(KEY_P, KEY_LEFTSHIFT),
	key_array[81] = set_keys(KEY_Q, KEY_LEFTSHIFT),
	key_array[82] = set_keys(KEY_R, KEY_LEFTSHIFT);
	key_array[83] = set_keys(KEY_S, KEY_LEFTSHIFT);
	key_array[84] = set_keys(KEY_T, KEY_LEFTSHIFT);
	key_array[85] = set_keys(KEY_U, KEY_LEFTSHIFT);
	key_array[86] = set_keys(KEY_V, KEY_LEFTSHIFT);
	key_array[87] = set_keys(KEY_W, KEY_LEFTSHIFT);
	key_array[88] = set_keys(KEY_X, KEY_LEFTSHIFT);
	key_array[89] = set_keys(KEY_Y, KEY_LEFTSHIFT);
	key_array[90] = set_keys(KEY_Z, KEY_LEFTSHIFT);

	/* Under-score */
	key_array[95] = set_keys(KEY_MINUS, KEY_LEFTSHIFT);

	/* a - z*/
	key_array[97] = set_keys(KEY_A, 0);
	key_array[98] = set_keys(KEY_B, 0);
	key_array[99] = set_keys(KEY_C, 0);
	key_array[100] = set_keys(KEY_D, 0);
	key_array[101] = set_keys(KEY_E, 0);
	key_array[102] = set_keys(KEY_F, 0);
	key_array[103] = set_keys(KEY_G, 0);
	key_array[104] = set_keys(KEY_H, 0);
	key_array[105] = set_keys(KEY_I, 0);
	key_array[106] = set_keys(KEY_J, 0);
	key_array[107] = set_keys(KEY_K, 0);
	key_array[108] = set_keys(KEY_L, 0);
	key_array[109] = set_keys(KEY_M, 0);
	key_array[110] = set_keys(KEY_N, 0);
	key_array[111] = set_keys(KEY_O, 0);
	key_array[112] = set_keys(KEY_P, 0);
	key_array[113] = set_keys(KEY_Q, 0);
	key_array[114] = set_keys(KEY_R, 0);
	key_array[115] = set_keys(KEY_S, 0);
	key_array[116] = set_keys(KEY_T, 0);
	key_array[117] = set_keys(KEY_U, 0);
	key_array[118] = set_keys(KEY_V, 0);
	key_array[119] = set_keys(KEY_W, 0);
	key_array[120] = set_keys(KEY_X, 0);
	key_array[121] = set_keys(KEY_Y, 0);
	key_array[122] = set_keys(KEY_Z, 0);

	/* Pipe (Tab specifier) */
	key_array[124] = set_keys(KEY_TAB, 0);
	key_array[125] = set_keys(KEY_ENTER, 0);
}
