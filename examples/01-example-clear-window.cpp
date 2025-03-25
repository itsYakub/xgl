#include "../xgl.hpp"

int main(void) {
	xgl::Window	window(800, 600, "Hello, xgl!");

	while (!window.should_quit()) {
		window.poll_events().clear(0.9f, 0.2f, 0.2f, 1.0f).swap_buffers();
	}
}
