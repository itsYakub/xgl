#if !defined _xgl_hpp_
# define _xgl_hpp_

/*	SECTION:
 *		Platform checking
 * */

# if !defined __linux__
#  warning "WARN: Platform unsupported"
# endif

# include <array>
# include <string>

namespace xgl {

	class Window {
		private:
			long unsigned	m_atom_quit;
			long unsigned	m_id;
			void			*m_dsp;
			void			*m_ctx;
			bool			m_quit;

			int				(*f_key)(void *, int, int);
			int				(*f_mouse)(void *, int, int);
			int				(*f_mouse_motion)(void *, int, int);
			void			*f_key_ptr;
			void			*f_mouse_ptr;
			void			*f_mouse_motion_ptr;

		public:
			Window(unsigned, unsigned, const std::string &);
			Window(const xgl::Window &);
			~Window(void);
			
			Window	&operator=(const xgl::Window &);

			Window	&init(unsigned, unsigned, const std::string &);
			Window	&poll_events(void);
			Window	&swap_buffers(void);
			bool	should_quit(void) const;
			Window	&quit(void);
			Window	&clear(float, float, float, float);
			Window	&clear_int(unsigned);
			Window	&make_current(void);
			Window	&hook_key(int (*)(void *, int, int), void *);
			Window	&hook_mouse(int (*)(void *, int, int), void *);
			Window	&hook_mouse_motion(int (*)(void *, int, int), void *);
			
		private:
			void	default_window_properties(std::array<int, 32 * 2> &);
			void	default_context_properties(std::array<int, 7> &);
	};

}

#endif
