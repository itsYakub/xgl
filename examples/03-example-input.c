#include "../xgl.h"

#include <stdio.h>
#include <X11/keysym.h>

static int	__xgl_key_hook(void *dat, int key, int state) {
	(void) dat;
	printf("info: Key: %d | State: %d\n", key, state);
	return (1);
}

static int	__xgl_mouse_hook(void *dat, int button, int state) {
	(void) dat;
	printf("info: Button: %d | State: %d\n", button, state);
	return (1);
}

static int	__xgl_mouse_motion_hook(void *dat, int x, int y) {
	(void) dat;
	printf("info: Mouse position: %d, %d\n", x, y);
	return (1);
}

int main(void) {
	t_window	window;

	xgl_window_init(&window, 800, 600, "Hello, xgl!");
	xgl_window_hook_key(&window, __xgl_key_hook, NULL);
	xgl_window_hook_mouse(&window, __xgl_mouse_hook, NULL);
	xgl_window_hook_mouse_motion(&window, __xgl_mouse_motion_hook, NULL);
	while (!xgl_window_should_quit(&window)) {
		xgl_window_poll_events(&window);
		xgl_window_swap_buffers(&window);
	}
	xgl_window_quit(&window);
}
