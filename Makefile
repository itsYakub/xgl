CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++11
AR			= ar
AFLAGS		= rcs
SRC			= ./xgl.cpp
OBJ			= $(SRC:.cpp=.obj)
NAME		= libxgl.a

all : release

.PHONY : release debug

release : CXXFLAGS += -s -O3
release : $(NAME)

debug : CXXFLAGS += -ggdb -g3 -O0
debug : $(NAME)

$(NAME) : $(OBJ)
	$(AR) $(AFLAGS) $@ $^

$(OBJ) : %.obj : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY : clean

clean:
	rm -f $(NAME)
	rm -f $(OBJ)
