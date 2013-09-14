#ifndef __mouse_h__
#define __mouse_h__

typedef struct {
	int x;
	int y;

	unsigned char state;
} mouse_t;

extern mouse_t *mouse_get ();

#endif

