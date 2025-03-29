#include "xgl.hpp"

/*	SECTION:
 *		Platform checking
 * */

#if !defined __linux__
# warning "WARN: Platform unsupported"
#endif

/*	SECTION:
 *		Header inclusions
 * */

#include <array>
#include <iostream>
#include <cstring>
#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

/*	SECTION:
 *		Local type definitions
 * */

typedef GLXContext (* PFNGLXCREATECONTEXTATTRIBSARBPROC) (Display *, GLXFBConfig, GLXContext, Bool, const int *);
PFNGLXCREATECONTEXTATTRIBSARBPROC	glXCreateContextAttribsARB;

/*	SECTION:
 *		Static function declarations
 * */

static GLXFBConfig	__xgl_gen_fbconfig(Display *, const int *);

/*	SECTION:
 *		Function definitions
 * */
xgl::Window::Window(unsigned w, unsigned h, const std::string &t) : m_atom_quit(0), m_id(0), m_dsp(nullptr), m_ctx(nullptr), m_quit(false) {
	this->init(w, h, t);
}

xgl::Window::Window(const xgl::Window &other) : 
	m_atom_quit(other.m_atom_quit),
	m_id(other.m_id),
	m_dsp(other.m_dsp),
	m_ctx(other.m_ctx),
	m_quit(other.m_quit) { }

xgl::Window::~Window(void) {
	this->quit();
}

xgl::Window	&xgl::Window::operator=(const xgl::Window &other) {
	this->m_atom_quit = other.m_atom_quit;
	this->m_id = other.m_id;
	this->m_dsp = other.m_dsp;
	this->m_ctx = other.m_ctx;
	this->m_quit = other.m_quit;
	return (*this);
}

xgl::Window	&xgl::Window::init(unsigned w, unsigned h, const std::string &t) {
	XSetWindowAttributes	_swinattr;
	GLXFBConfig				_fbconf;
	XVisualInfo				*_vi;
	std::array<int, 32 * 2>	_attr_win;
	std::array<int, 7>		_attr_ctx;
	int						_winmask;

	/*	Initializing all the values to zero.
	 * */

	_attr_win.fill(0);
	_attr_ctx.fill(0);
	this->default_window_properties(_attr_win);
	this->default_context_properties(_attr_ctx);
	glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC) glXGetProcAddress((GLubyte *) "glXCreateContextAttribsARB");
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

	this->m_dsp = XOpenDisplay(0);
	_fbconf = __xgl_gen_fbconfig(static_cast<Display *>(this->m_dsp), _attr_win.data());
	_vi = glXGetVisualFromFBConfig(static_cast<Display *>(this->m_dsp), _fbconf);
	memset(&_swinattr, 0, sizeof(XSetWindowAttributes));
	_swinattr.colormap = XCreateColormap(static_cast<Display *>(this->m_dsp), DefaultRootWindow(this->m_dsp), _vi->visual, None);
	_swinattr.event_mask = _winmask; 
	this->m_id = XCreateWindow(
		static_cast<Display *>(this->m_dsp),
		DefaultRootWindow(this->m_dsp),
		0, 0,
		w, h, 0,
		_vi->depth,
		InputOutput,
		_vi->visual,
		_winmask,
		&_swinattr
	);
	XStoreName(static_cast<Display *>(this->m_dsp), this->m_id, t.c_str());
	
	/*	STEP 2. Creating a GLX context
	 *	- Load a glX context based on the OpenGL attributes and the best GLXFBConfig;
	 *	- Make the context current;
	 *	- Map the window to the screen;
	 * */

	this->m_ctx = glXCreateContextAttribsARB(static_cast<Display *>(this->m_dsp), _fbconf, 0, 1, _attr_ctx.data());
	glXMakeCurrent(static_cast<Display *>(this->m_dsp), this->m_id, static_cast<GLXContext>(this->m_ctx));
	XMapWindow(static_cast<Display *>(this->m_dsp), this->m_id);

	/*	STEP 3. Client Messages
	 *	- Load a specific X11 atoms;
	 *	- Set the protocols based on the atoms;
	 * */

	this->m_atom_quit = XInternAtom(static_cast<Display *>(this->m_dsp), "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(static_cast<Display *>(this->m_dsp), this->m_id, &this->m_atom_quit, 1);

	/*	STEP 4. Cleanup
	 * */

	XFree(_vi);

	/*	BONUS STEP. Info logging
	 * */
	{
		int	_glx_version_major;
		int	_glx_version_minor;

		glXQueryVersion(static_cast<Display *>(this->m_dsp), &_glx_version_major, &_glx_version_minor);
		std::cout << "[ xgl ] INFO: Window created successfully | Width: " << w << " | Height: " << h << " | ID: " << this->m_id << std::endl; 
		std::cout << "[ xgl ] INFO: glX version: " << _glx_version_major << "." <<  _glx_version_minor << std::endl;
		std::cout << "[ xgl ] INFO: OpenGL version: " <<  glGetString(GL_VERSION) << std::endl;
	}

	return (*this);
}

xgl::Window	&xgl::Window::poll_events(void) {
	XEvent	_event;

	while (XPending(static_cast<Display *>(this->m_dsp))) {
		XNextEvent(static_cast<Display *>(this->m_dsp), &_event);
		switch (_event.type) {
			case (KeyPress): {
				unsigned	_keysym;

				if (!this->f_key) {
					break;
				}
				_keysym = XkbKeycodeToKeysym(static_cast<Display *>(this->m_dsp), _event.xkey.keycode, 0, 0);
				this->f_key(this->f_key_ptr, _keysym, 1);
			} break;
			case (KeyRelease): {
				unsigned	_keysym;
				
				if (!this->f_key) {
					break;
				}
				_keysym = XkbKeycodeToKeysym(static_cast<Display *>(this->m_dsp), _event.xkey.keycode, 0, 0);
				this->f_key(this->f_key_ptr, _keysym, 0);
			} break;
			case (ButtonPress): {
				if (!this->f_mouse) {
					break;
				}
				this->f_mouse(this->f_mouse_ptr, _event.xbutton.button, 1);
			} break;
			case (ButtonRelease): {
				if (!this->f_mouse) {
					break;
				}
				this->f_mouse(this->f_mouse_ptr, _event.xbutton.button, 0);
			} break;
			case (MotionNotify): {
				if (!this->f_mouse_motion) {
					break;
				}
				this->f_mouse_motion(this->f_mouse_motion_ptr, _event.xmotion.x, _event.xmotion.y);
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
				if ((Atom) _event.xclient.data.l[0] == this->m_atom_quit) {
					this->m_quit = true;
				}					
			} break;

			case (MappingNotify): { } break;
			case (GenericEvent): { } break;
			default: { } break;
		}
	}

	return (*this);
}

xgl::Window	&xgl::Window::swap_buffers(void) {
	glXSwapBuffers(static_cast<Display *>(this->m_dsp), this->m_id);
	return (*this);
}

bool	xgl::Window::should_quit(void) const {
	return (this->m_quit);
}

xgl::Window	&xgl::Window::quit(void) {
	glXDestroyContext(static_cast<Display *>(this->m_dsp), static_cast<GLXContext>(this->m_ctx));
	std::cout << "[ xgl ] INFO: Window ID. " << this->m_id << ": Closing glX context" << std::endl; 
	XUnmapWindow(static_cast<Display *>(this->m_dsp), this->m_id);
	XDestroyWindow(static_cast<Display *>(this->m_dsp), this->m_id);
	std::cout << "[ xgl ] INFO: Window ID. " << this->m_id << ": Closing X11 window" << std::endl; 
	XCloseDisplay(static_cast<Display *>(this->m_dsp));
	std::cout << "[ xgl ] INFO: Window ID. " << this->m_id << ": Closing X11 server connection" << std::endl; 
	return (*this);
}

xgl::Window	&xgl::Window::clear(float r, float g, float b, float a) {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(r, g, b, a);
	return (*this);
}

xgl::Window	&xgl::Window::clear_int(unsigned val) {
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
	return (this->clear(_r, _g, _b, _a));
}

xgl::Window	&xgl::Window::make_current(void) {
	glXMakeCurrent(static_cast<Display *>(this->m_dsp), this->m_id, static_cast<GLXContext>(this->m_ctx));
	return (*this);
}

xgl::Window	&xgl::Window::hook_key(int (*f)(void *, int, int), void *ptr) {
	this->f_key = f;
	this->f_key_ptr = ptr;
	return (*this);
}

xgl::Window	&xgl::Window::hook_mouse(int (*f)(void *, int, int), void *ptr) {
	this->f_mouse = f;
	this->f_mouse_ptr = ptr;
	return (*this);
}

xgl::Window	&xgl::Window::hook_mouse_motion(int (*f)(void *, int, int), void *ptr) {
	this->f_mouse_motion = f;
	this->f_mouse_motion_ptr = ptr;
	return (*this);
}

/*	SECTION:
 *		Static function definitions
 * */

void	xgl::Window::default_window_properties(std::array<int, 32 * 2> &attr) {
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
}

void	xgl::Window::default_context_properties(std::array<int, 7> &attr) {
	attr[0] = GLX_CONTEXT_PROFILE_MASK_ARB;		attr[1] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;
	attr[2] = GLX_CONTEXT_MAJOR_VERSION_ARB;	attr[3] = 3;
	attr[4] = GLX_CONTEXT_MINOR_VERSION_ARB;	attr[5] = 3;
}

static GLXFBConfig	__xgl_gen_fbconfig(Display *dsp, const int *attr) {
	GLXFBConfig*	_fbconf_arr;
	GLXFBConfig		_fbconf_best;
	XVisualInfo		*_vi;
	int				_screen;
	int				_fbconf_cnt;
	int				_fbconf_best_index;
	int				_fbconf_sample_buffers;
	int				_fbconf_samples;

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
