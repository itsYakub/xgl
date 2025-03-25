#include "../xgl.h"

int main(void) {
	t_window	window;

	xgl_window_init(&window, 800, 600, "Hello, xgl!");
	while (!xgl_window_should_quit(&window)) {
		xgl_window_poll_events(&window);
		xgl_window_clear(0.9f, 0.2f, 0.2f, 1.0f);
		xgl_window_swap_buffers(&window);
	}
	xgl_window_quit(&window);
}
