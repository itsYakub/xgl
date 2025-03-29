#include "xgl.h"

/*	SECTION:
 *		Platform checking
 * */

#if !defined __linux__
# warning "WARN: Platform unsupported"
#endif

/*	SECTION:
 *		Header inclusions
 * */

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

/*	SECTION:
 *		Macro Definition
 * */

#if !defined XGL_MIN
# define XGL_MIN(a, b) ( a < b ? a : b )
#endif

#if !defined XGL_MAX
# define XGL_MAX(a, b) ( a > b ? a : b )
#endif

/*	SECTION:
 *		Local type definitions
 * */

typedef GLXFBConfig				t_fbconf;
typedef XVisualInfo				*t_vinfo;
typedef XSetWindowAttributes	t_swinattr;

typedef GLXContext (* PFNGLXCREATECONTEXTATTRIBSARBPROC) (Display *, GLXFBConfig, GLXContext, Bool, const int *);
PFNGLXCREATECONTEXTATTRIBSARBPROC	glXCreateContextAttribsARB;

/*	SECTION:
 *		Static function declarations
 * */

static int		__xgl_default_win_attr(int *);
static int		__xgl_default_ctx_attr(int *);
static t_fbconf	__xgl_gen_fbconfig(Display *, const int *);

/*	SECTION:
 *		Function definitions
 * */

int	xgl_window_init(t_window *xw, unsigned w, unsigned h, const char *t) {
	t_swinattr	_swinattr;
	t_fbconf	_fbconf;
	t_vinfo		_vi;
	int			_winmask;
	int			_attr_win[32 * 2];
	int			_attr_ctx[7];

	if (!xw) {
		return (0);
	}
	
	/*	Initializing all the values to zero.
	 * */

	xw = memset(xw, 0, sizeof(t_window));
	memset(_attr_win, 0, sizeof(_attr_win));
	memset(_attr_ctx, 0, sizeof(_attr_ctx));
	__xgl_default_win_attr(_attr_win);
	__xgl_default_ctx_attr(_attr_ctx);
	_winmask = 0;
	_winmask |=
		CWColormap
		| CWBorderPixel
		| CWBackPixel
		| CWEventMask
		| KeyPressMask
		| KeyReleaseMask
		| ButtonPressMask
		| ButtonReleaseMask
		| PointerMotionMask;
	glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((GLubyte *) "glXCreateContextAttribsARB");
	
	/*	STEP 1. Creating an X11 Window
	 *	- Connect to X11 server;
	 *	- Generate an array of GLXFBConfigs and choose the best one;
	 *	- Generate an XVisualInfo object based on the best GLXFBConfig;
	 *	- Create a set of window attributes:
	 *		- Generate a colormap for the window;
	 *		- Setup an event mask;
	 *	- Create a window object itself;
	 *	- Set / Store the title of the window;
	 * */

	xw->dsp = XOpenDisplay(0);
	_fbconf = __xgl_gen_fbconfig(xw->dsp, _attr_win);
	_vi = glXGetVisualFromFBConfig(xw->dsp, _fbconf);
	memset(&_swinattr, 0, sizeof(t_swinattr));
	_swinattr.colormap = XCreateColormap(xw->dsp, DefaultRootWindow(xw->dsp), _vi->visual, None);
	_swinattr.event_mask = _winmask;
	xw->id = XCreateWindow(
		xw->dsp,
		DefaultRootWindow(xw->dsp),
		0, 0,
		w, h, 0,
		_vi->depth,
		InputOutput,
		_vi->visual,
		_winmask,
		&_swinattr
	);
	XStoreName(xw->dsp, xw->id, t);
	
	/*	STEP 2. Creating a GLX context
	 *	- Load a glX context based on the OpenGL attributes and the best GLXFBConfig;
	 *	- Make the context current;
	 *	- Map the window to the screen;
	 * */

	xw->ctx = glXCreateContextAttribsARB(xw->dsp, _fbconf, 0, 1, _attr_ctx);
	glXMakeCurrent(xw->dsp, xw->id, xw->ctx);
	XMapWindow(xw->dsp, xw->id);

	/*	STEP 3. Client Messages
	 *	- Load a specific X11 atoms;
	 *	- Set the protocols based on the atoms;
	 * */

	xw->s_atoms.atom_quit = XInternAtom(xw->dsp, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(xw->dsp, xw->id, &xw->s_atoms.atom_quit, 1);

	/*	STEP 4. Cleanup
	 * */

	XFree(_vi);

	/*	BONUS STEP. Info logging
	 * */
	{
		int	_glx_version_major;
		int	_glx_version_minor;

		glXQueryVersion(xw->dsp, &_glx_version_major, &_glx_version_minor);
		fprintf(stdout, "[ xgl ] INFO: Window created successfully | Width: %d | Height: %d | ID: %lu\n", w, h, xw->id); 
		fprintf(stdout, "[ xgl ] INFO: glX version: %d.%d\n", _glx_version_major, _glx_version_minor); 
		fprintf(stdout, "[ xgl ] INFO: OpenGL version: %s\n", glGetString(GL_VERSION)); 
	}

	return (1);
}

int	xgl_window_poll_events(t_window *xw) {
	XEvent	_event;

	while (XPending(xw->dsp)) {
		XNextEvent(xw->dsp, &_event);
		switch (_event.type) {
			case (KeyPress): {
				unsigned	_keysym;

				if (!xw->s_hooks.f_mouse_motion) {
					break;
				}
				_keysym = XkbKeycodeToKeysym(xw->dsp, _event.xkey.keycode, 0, 0);
				xw->s_hooks.f_key(xw->s_hooks.f_key_ptr, _keysym, 1);
			} break;
			case (KeyRelease): {
				unsigned	_keysym;
				
				if (!xw->s_hooks.f_mouse_motion) {
					break;
				}
				_keysym = XkbKeycodeToKeysym(xw->dsp, _event.xkey.keycode, 0, 0);
				xw->s_hooks.f_key(xw->s_hooks.f_key_ptr, _keysym, 0);
			} break;
			case (ButtonPress): {
				if (!xw->s_hooks.f_mouse_motion) {
					break;
				}
				xw->s_hooks.f_mouse(xw->s_hooks.f_mouse_ptr, _event.xbutton.button, 1);
			} break;
			case (ButtonRelease): {
				if (!xw->s_hooks.f_mouse_motion) {
					break;
				}
				xw->s_hooks.f_mouse(xw->s_hooks.f_mouse_ptr, _event.xbutton.button, 0);
			} break;
			case (MotionNotify): {
				if (!xw->s_hooks.f_mouse_motion) {
					break;
				}
				xw->s_hooks.f_mouse_motion(xw->s_hooks.f_mouse_motion_ptr, _event.xmotion.x, _event.xmotion.y);
			} break;
			case (EnterNotify): { } break;
			case (LeaveNotify): { } break;
			case (FocusIn): { } break;
			case (FocusOut): { } break;
			case (KeymapNotify): { } break;
			case (Expose): { } break;
			case (GraphicsExpose): { } break;
			case (NoExpose): { } break;
			case (VisibilityNotify): { } break;
			case (CreateNotify): { } break;
			case (DestroyNotify): { } break;
			case (UnmapNotify): { } break;
			case (MapNotify): { } break;
			case (MapRequest): { } break;
			case (ReparentNotify): { } break;
			case (ConfigureNotify): { } break;
			case (ConfigureRequest): { } break;
			case (GravityNotify): { } break;
			case (ResizeRequest): { } break;
			case (CirculateNotify): { } break;
			case (CirculateRequest): { } break;
			case (PropertyNotify): { } break;
			case (SelectionClear): { } break;
			case (SelectionRequest): { } break;
			case (SelectionNotify): { } break;
			case (ColormapNotify): { } break;

			case (ClientMessage): {
				if ((Atom) _event.xclient.data.l[0] == xw->s_atoms.atom_quit) {
					xw->quit = true;
				}					
			} break;

			case (MappingNotify): { } break;
			case (GenericEvent): { } break;
			default: { } break;
		}
	}
	return (1);
}

int	xgl_window_swap_buffers(t_window *xw) {
	glXSwapBuffers(xw->dsp, xw->id);
	return (1);
}

int	xgl_window_should_quit(t_window *xw) {
	return (xw->quit);
}

int	xgl_window_quit(t_window *xw) {
	glXDestroyContext(xw->dsp, xw->ctx);
	fprintf(stdout, "[ xgl ] INFO: Window ID. %lu: Closing glX context\n", xw->id); 
	XUnmapWindow(xw->dsp, xw->id);
	XDestroyWindow(xw->dsp, xw->id);
	fprintf(stdout, "[ xgl ] INFO: Window ID. %lu: Closing X11 window\n", xw->id); 
	XCloseDisplay(xw->dsp);
	fprintf(stdout, "[ xgl ] INFO: Window ID. %lu: Closing X11 server connection\n", xw->id); 
	return (1);
}

int	xgl_window_clear(float r, float g, float b, float a) {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(r, g, b, a);
	return (1);
}

int	xgl_window_clear_int(unsigned val) {
	float	_r;
	float	_g;
	float	_b;
	float	_a;

#if defined __ORDER_LITTLE_ENDIAN__

	_r = ((val >> 8 * 3) & 0xff) / 255.0f;
	_g = ((val >> 8 * 2) & 0xff) / 255.0f;
	_b = ((val >> 8 * 1) & 0xff) / 255.0f;
	_a = ((val >> 8 * 0) & 0xff) / 255.0f;

#elif defined __ORDER_BIG_ENDIAN__

	_r = ((val >> 8 * 0) & 0xff) / 255.0f;
	_g = ((val >> 8 * 1) & 0xff) / 255.0f;
	_b = ((val >> 8 * 2) & 0xff) / 255.0f;
	_a = ((val >> 8 * 3) & 0xff) / 255.0f;

#elif
# warning "Unsuported endianess"
#endif
	return (xgl_window_clear(_r, _g, _b, _a));
}

int	xgl_window_make_current(t_window *xw) {
	glXMakeCurrent(xw->dsp, xw->id, xw->ctx);
	return (1);
}

int	xgl_window_hook_key(t_window *xw, int (*f)(void *, int, int), void *ptr) {
	xw->s_hooks.f_key = f;
	xw->s_hooks.f_key_ptr = ptr;
	return (1);
}

int	xgl_window_hook_mouse(t_window *xw, int (*f)(void *, int, int), void *ptr) {
	xw->s_hooks.f_mouse = f;
	xw->s_hooks.f_mouse_ptr = ptr;
	return (1);
}

int	xgl_window_hook_mouse_motion(t_window *xw, int (*f)(void *, int, int), void *ptr) {
	xw->s_hooks.f_mouse_motion = f;
	xw->s_hooks.f_mouse_motion_ptr = ptr;
	return (1);
}

/*	SECTION:
 *		Static function definitions
 * */

static int		__xgl_default_win_attr(int *attr) {
	int	i;

	i = 0;
	attr[i++] = GLX_USE_GL;			attr[i++] = 1;
	attr[i++] = GLX_DOUBLEBUFFER;	attr[i++] = 1;
	attr[i++] = GLX_RED_SIZE;		attr[i++] = 8;
	attr[i++] = GLX_GREEN_SIZE;		attr[i++] = 8;
	attr[i++] = GLX_BLUE_SIZE;		attr[i++] = 8;
	attr[i++] = GLX_ALPHA_SIZE;		attr[i++] = 8;
	attr[i++] = GLX_DEPTH_SIZE;		attr[i++] = 24;
	attr[i++] = GLX_RENDER_TYPE;	attr[i++] = GLX_RGBA_BIT;
	attr[i++] = GLX_DRAWABLE_TYPE;	attr[i++] = GLX_WINDOW_BIT;
    attr[i++] = GLX_X_VISUAL_TYPE;	attr[i++] = GLX_TRUE_COLOR;
	return (1);
}
static int		__xgl_default_ctx_attr(int *attr) {
	attr[0] = GLX_CONTEXT_PROFILE_MASK_ARB;		attr[1] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
	attr[2] = GLX_CONTEXT_MAJOR_VERSION_ARB;	attr[3] = 3;
	attr[4] = GLX_CONTEXT_MINOR_VERSION_ARB;	attr[5] = 3;
	return (1);
}

static t_fbconf	__xgl_gen_fbconfig(Display *dsp, const int *attr) {
	t_fbconf*	_fbconf_arr;
	t_fbconf	_fbconf_best;
	t_vinfo		_vi;
	int			_screen;
	int			_fbconf_cnt;
	int			_fbconf_best_index;
	int			_fbconf_sample_buffers;
	int			_fbconf_samples;

	_fbconf_best_index = -1;
	_screen = DefaultScreen(dsp);
	_fbconf_arr = glXChooseFBConfig(dsp, _screen, attr, &_fbconf_cnt);
	if (!_fbconf_arr) {
		return (0);
	}
	for (int i = 0; i < (int) _fbconf_cnt; i++) {
		_vi = glXGetVisualFromFBConfig(dsp, _fbconf_arr[i]);
		if (!_vi) {
			continue;
		}
		glXGetFBConfigAttrib(dsp, _fbconf_arr[i], GLX_SAMPLE_BUFFERS, &_fbconf_sample_buffers);
		glXGetFBConfigAttrib(dsp, _fbconf_arr[i], GLX_SAMPLES, &_fbconf_samples);
		if ((_fbconf_best_index < 0 || _fbconf_sample_buffers) && (!_fbconf_samples && _fbconf_best_index < 0)) {
			_fbconf_best_index = i;
		}
		XFree(_vi);
	}
	if (_fbconf_best_index == -1) {
		XFree(_fbconf_arr);
		return (0);
	}
	_fbconf_best = _fbconf_arr[_fbconf_best_index];
	XFree(_fbconf_arr);
	return (_fbconf_best);
}
