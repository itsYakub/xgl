#include "../xgl.hpp"

int main(void) {
	xgl::Window	window(800, 600, "Hello, xgl!");

	while (!window.should_quit()) {
		window.poll_events();
	}
}
