CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++11
LXXFLAGS	= -lxgl -lX11 -lGL -lGLX
LXXDFLAGS	= -L ..

all : release

.PHONY : release debug

release : CXXFLAGS += -s -O3
release : 00-example-window.out 01-example-clear-window.out 02-example-multiple-windows.out 03-example-input.out

debug : CXXFLAGS += -ggdb -g3 -O0
debug : 00-example-window.out 01-example-clear-window.out 02-example-multiple-windows.out 03-example-input.out

00-example-window.out :
	$(CXX) $(CXXFLAGS) 00-example-window.cpp -o $@ $(LXXFLAGS) $(LXXDFLAGS)

01-example-clear-window.out :
	$(CXX) $(CXXFLAGS) 01-example-clear-window.cpp -o $@ $(LXXFLAGS) $(LXXDFLAGS)

02-example-multiple-windows.out :
	$(CXX) $(CXXFLAGS) 02-example-multiple-windows.cpp -o $@ $(LXXFLAGS) $(LXXDFLAGS)

03-example-input.out :
	$(CXX) $(CXXFLAGS) 03-example-input.cpp -o $@ $(LXXFLAGS) $(LXXDFLAGS)

.PHONY : clean

clean:
	rm -f ./*.out
