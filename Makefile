CC		= cc
CFLAGS	= -Wall -Wextra -Werror -std=c99
AR		= ar
AFLAGS	= rcs
SRC		= ./xgl.c
OBJ		= $(SRC:.c=.o)
LFLAGS	= -lX11
NAME	= libxgl.a

all : release

.PHONY : release debug

release : CFLAGS += -s -O3
release : $(NAME)

debug : CFLAGS += -ggdb -g3 -O0
debug : $(NAME)

$(NAME) : $(OBJ)
	$(AR) $(AFLAGS) $@ $^

$(OBJ) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean

clean:
	rm -f $(NAME)
	rm -f $(OBJ)
