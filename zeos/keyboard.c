#include <keyboard.h>
#include <interrupt.h>
#include <io.h>

int keyboard_routine() {
	unsigned char key;
	key = inb(KEYBOARD_PORT);
	if (!(key && MASK_KEY)) {
		key = key && 0x7F;
		printc_xy(0, 0, char_map[key]);
		return 1; //todo ok
	}
	//TODO: codigo de error (errno.h)
	return -1;
}
