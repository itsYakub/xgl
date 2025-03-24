#include "xgl.h"

#if !defined __linux__
# warning "WARN: Platform unsupported"
#endif
#include <string.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

typedef GLXFBConfig				t_fbconf;
typedef XVisualInfo				*t_vinfo;
typedef XSetWindowAttributes	t_swinattr;
typedef XWindowAttributes		t_winattr;

typedef GLXContext (* PFNGLXCREATECONTEXTATTRIBSARBPROC) (Display *, GLXFBConfig, GLXContext, Bool, const int *);
PFNGLXCREATECONTEXTATTRIBSARBPROC	glXCreateContextAttribsARB;

static int		__xgl_default_win_attr(int *);
static int		__xgl_default_ctx_attr(int *);
static t_fbconf	__xgl_gen_fbconfig(Display *, const int *);

int	xgl_window_init(t_window *xw, unsigned w, unsigned h, const char *t) {
	t_swinattr	_swinattr;
	t_winattr	_winattr;
	t_fbconf	_fbconf;
	t_vinfo		_vi;
	unsigned	_colmap;
	unsigned	_rid;
	int			_attr_win[32 * 2];
	int			_attr_ctx[7];

	if (!xw) {
		return (0);
	}
	glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((GLubyte *) "glXCreateContextAttribsARB");
	xw = memset(xw, 0, sizeof(t_window));
	memset(_attr_win, 0, sizeof(_attr_win));
	__xgl_default_win_attr(_attr_win);
	xw->dsp = XOpenDisplay(0);
	_rid = DefaultRootWindow(xw->dsp);
	_fbconf = __xgl_gen_fbconfig(xw->dsp, _attr_win);
	_vi = glXGetVisualFromFBConfig(xw->dsp, _fbconf);
	_colmap = XCreateColormap(xw->dsp, _rid, _vi->visual, None);
	XGetWindowAttributes(xw->dsp, _rid, &_winattr);
	_swinattr.colormap = _colmap;
	_swinattr.background_pixmap = 0;
	_swinattr.border_pixel = 0; 
	_swinattr.event_mask = CWColormap | CWBorderPixel | CWBackPixel | CWEventMask;
	xw->id = XCreateWindow(
		xw->dsp,
		_rid,
		0, 0,
		w, h, 0,
		_vi->depth,
		InputOutput,
		_vi->visual,
		CWColormap | CWBorderPixel | CWBackPixel | CWEventMask,
		&_swinattr
	);
	XStoreName(xw->dsp, xw->id, t);
	memset(_attr_ctx, 0, sizeof(_attr_ctx));
	__xgl_default_ctx_attr(_attr_ctx);
	xw->ctx = glXCreateContextAttribsARB(xw->dsp, _fbconf, 0, 1, _attr_ctx);
	glXMakeCurrent(xw->dsp, xw->id, xw->ctx);
	XMapWindow(xw->dsp, xw->id);
	xw->s_atoms.atom_quit = XInternAtom(xw->dsp, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(xw->dsp, xw->id, &xw->s_atoms.atom_quit, 1);
	XFree(_vi);
	return (1);
}

int	xgl_window_poll_events(t_window *xw) {
	XEvent	_event;

	while (XPending(xw->dsp)) {
		XNextEvent(xw->dsp, &_event);
		switch (_event.type) {
			case (KeyPress): { } break;
			case (KeyRelease): { } break;
			case (ButtonPress): { } break;
			case (ButtonRelease): { } break;
			case (MotionNotify): { } break;
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
	XUnmapWindow(xw->dsp, xw->id);
	XDestroyWindow(xw->dsp, xw->id);
	XCloseDisplay(xw->dsp);
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

static int		__xgl_default_win_attr(int *attr) {
	int	i;

	i = 0;
	attr[i] = GLX_USE_GL;			attr[i + 1] = 1;				i += 2;
	attr[i] = GLX_DOUBLEBUFFER;		attr[i + 1] = 1;				i += 2;
	attr[i] = GLX_RED_SIZE;			attr[i + 1] = 8;				i += 2;
	attr[i] = GLX_GREEN_SIZE;		attr[i + 1] = 8;				i += 2;
	attr[i] = GLX_BLUE_SIZE;		attr[i + 1] = 8;				i += 2;
	attr[i] = GLX_ALPHA_SIZE;		attr[i + 1] = 8;				i += 2;
	attr[i] = GLX_DEPTH_SIZE;		attr[i + 1] = 24;				i += 2;
	attr[i] = GLX_RENDER_TYPE;		attr[i + 1] = GLX_RGBA_BIT;		i += 2;
	attr[i] = GLX_DRAWABLE_TYPE;	attr[i + 1] = GLX_WINDOW_BIT;	i += 2;
    attr[i] = GLX_X_VISUAL_TYPE;	attr[i + 1] = GLX_TRUE_COLOR;	i += 2;
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
