#ifndef _AUTO_TYPE_H_
#define _AUTO_TYPE_H_

#define _BSD_SOURCE // For usleep

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  /* For usleep */
#include "cinput.h"

#define MAX_ASCII_VALUE 126 //Currently ~ is the largest ascii value we use 

/*
 *	In search of an easy way to map the actual characters into 
 *	their key-press alternatives and allow for ease of use I have decided to 
 *	do the following:
 *
 *	Each character will be mapped into this global array based on it's ascii values,
 *	and will hold the needed key(s) that mimick the key being pressed. That way, 
 *	when character 'a' is read, it can be written to the fd by setting the input_event
 *	struct's code value to: 
 *
 *	input_event.code = array['a'];
 *	
 */

/* Global map array */
struct keypress key_array[MAX_ASCII_VALUE];

int type (FILE* fh);

/*
 *	Set up the key array prior to start of program
 */
__attribute__((constructor)) void setup (void);


#endif /* _AUTO_TYPE_H_ */
