#if !defined _xgl_h_
# define _xgl_h_

/*	SECTION:
 *		Platform checking
 * */

# if !defined __linux__
#  warning "WARN: Platform unsupported"
# endif

/*	SECTION:
 *		Header inclusions
 * */

# if !defined __cplusplus
#  include <stdbool.h>
# endif

# if defined __cplusplus

extern "C" {

# endif

/*	SECTION:
 *		Type definitions
 * */

struct s_window {
	long unsigned	id;
	void			*dsp;
	void			*ctx;
	bool			quit;

	struct {
		long unsigned	atom_quit;
	} s_atoms;
};

typedef struct s_window	t_window;

/*	SECTION:
 *		Function declarations
 * */

int	xgl_window_init(t_window *, unsigned, unsigned, const char *);
int	xgl_window_poll_events(t_window *);
int	xgl_window_swap_buffers(t_window *);
int	xgl_window_should_quit(t_window *);
int	xgl_window_quit(t_window *);
int	xgl_window_clear(float, float, float, float);
int	xgl_window_clear_int(unsigned);
int	xgl_window_make_current(t_window *);

# if defined __cplusplus

}

# endif
#endif
