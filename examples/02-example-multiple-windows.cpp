#include "../xgl.hpp"

int main(void) {
	xgl::Window	w0(800, 600, "Hello, xgl! (Window: 0)");
	xgl::Window	w1(800, 600, "Hello, xgl! (Window: 1)");
	xgl::Window	w2(800, 600, "Hello, xgl! (Window: 2)");

	while (!w0.should_quit() && !w1.should_quit() && !w2.should_quit()) {
		w0.make_current().poll_events().clear_int(0xff0000ff).swap_buffers();
		w1.make_current().poll_events().clear_int(0x00ff00ff).swap_buffers();
		w2.make_current().poll_events().clear_int(0x0000ffff).swap_buffers();
	}
}
