#include "../xgl.hpp"

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
	xgl::Window	window(800, 600, "Hello, xgl!");

	window.
		hook_key(__xgl_key_hook, nullptr).
		hook_mouse(__xgl_mouse_hook, nullptr).
		hook_mouse_motion(__xgl_mouse_motion_hook, nullptr);
	while (!window.should_quit()) {
		window.poll_events();
	}
}
