#include "../xgl.h"

int main(void) {
	t_window	w0;
	t_window	w1;
	t_window	w2;

	xgl_window_init(&w0, 800, 600, "Hello, xgl! (Window: 0)");
	xgl_window_init(&w1, 800, 600, "Hello, xgl! (Window: 1)");
	xgl_window_init(&w2, 800, 600, "Hello, xgl! (Window: 2)");
	while (
			!xgl_window_should_quit(&w0) ||
			!xgl_window_should_quit(&w1) ||
			!xgl_window_should_quit(&w2)
		) {
		{
			xgl_window_make_current(&w0);
			xgl_window_poll_events(&w0);
			xgl_window_clear_int(0xff0000ff);
			xgl_window_swap_buffers(&w0);
		}
		{
			xgl_window_make_current(&w1);
			xgl_window_poll_events(&w1);
			xgl_window_clear_int(0x00ff00ff);
			xgl_window_swap_buffers(&w1);
		}
		{
			xgl_window_make_current(&w2);
			xgl_window_poll_events(&w2);
			xgl_window_clear_int(0x0000ffff);
			xgl_window_swap_buffers(&w2);
		}
	}
	xgl_window_quit(&w0);
	xgl_window_quit(&w1);
	xgl_window_quit(&w2);
}
