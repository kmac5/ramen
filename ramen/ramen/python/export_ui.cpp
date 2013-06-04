// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/python/pyside.hpp>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>
#include<ramen/ui/expression_editor.hpp>
#include<ramen/ui/viewer/viewer_context.hpp>

#include<ramen/image/format.hpp>

#include<ramen/params/aspect_ratio_param.hpp>

#include<ramen/flipbook/factory.hpp>

namespace ramen
{
namespace python
{
namespace
{

void add_image_format_preset( const std::string& name, int w, int h, float a)
{
	image::format_t::init_presets(); // <-- make sure presets are inited
	image::format_t::add_preset( std::make_pair( name, image::format_t( w, h, a)));
}

void add_aspect_ratio_preset( const std::string& name, float a)
{
	aspect_ratio_param_t::init_presets(); // <-- make sure presets are inited
	aspect_ratio_param_t::add_preset( std::make_pair( name, a));
}

class main_window_t
{
public:

	main_window_t() {}

	bpy::object as_QMainWindow()
	{
		return make_pyside_wrapper( reinterpret_cast<QMainWindow*>( app().ui()->main_window()));
	}

    /*
	void add_dock_widget( int area, bpy::object dock)
	{
		QDockWidget *dock_ = pyside_to_cpp<QDockWidget>( dock.ptr());
		app().ui()->main_window()->add_dock_widget( ( Qt::DockWidgetArea) area, dock_);
	}
    */
};

class user_interface_t
{
public:

	user_interface_t() {}

	void update() const
	{
		app().ui()->update();
	}

    void fatal_error( const std::string& msg) const
	{
		app().ui()->fatal_error( msg);
	}

    void error( const std::string& msg) const
	{
		app().ui()->error( msg);
	}

    void inform( const std::string& msg) const
	{
		app().ui()->inform( msg);
	}

	bool question( const std::string& what, bool default_answer) const
	{
		app().ui()->question( what, default_answer);
	}

	main_window_t main_window() { return main_window_t();}

	static user_interface_t get_ui()
	{
		return user_interface_t();
	}
};

/*
struct viewer_strategy_wrap_t : public ui::viewer::viewer_strategy_t
{
public:

	viewer_strategy_wrap_t( PyObject *self, ui::viewer::viewer_context_t *p) : ui::viewer::viewer_strategy_t( p), self_( self)
	{
		Py_INCREF( self_);
	}

	virtual ~viewer_strategy_wrap_t() { Py_DECREF( self_);}

    virtual void do_draw_overlay( const ui::paint_event_t& event) const
	{
		try
		{
			bpy::call_method<void>( self_, "do_draw_overlay", boost::cref( event));
		}
		catch( bpy::error_already_set)
		{
			__do_draw_overlay( event);
		}
	}

	void default_do_draw_overlay( const viewer_strategy_t& this_, const ui::paint_event_t& event) const
	{
		this_.__do_draw_overlay( event);
	}

private:

    PyObject *self_;
};
*/

} // namespace

// prototypes
void export_ui_events();

void export_ui()
{
	bpy::object ui_module( bpy::handle<>( bpy::borrowed( PyImport_AddModule( "_ramen._ui"))));
	bpy::scope().attr( "_ui") = ui_module;
	bpy::scope ui_scope = ui_module;
	
	bpy::class_<main_window_t>( "main_window_t", bpy::no_init)
		.def( "as_QMainWindow", &main_window_t::as_QMainWindow)
		//.def( "add_dock_widget", &main_window_t::add_dock_widget)
		;

	bpy::class_<user_interface_t>( "user_interface_t", bpy::no_init)
		.def( "update", &user_interface_t::update)

		.def( "fatal_error", &user_interface_t::fatal_error)
		.def( "error", &user_interface_t::error)
		.def( "inform", &user_interface_t::inform)
		.def( "question", &user_interface_t::question)

		.def( "main_window", &user_interface_t::main_window)
		;

	bpy::def( "user_interface", &user_interface_t::get_ui);

	bpy::def( "add_image_format_preset", add_image_format_preset);
	bpy::def( "add_aspect_ratio_preset", add_aspect_ratio_preset);

	export_ui_events();

	bpy::class_<ui::expression_editor_callback_suite_t, boost::noncopyable>( "expression_edit_callbacks_suite_t", bpy::no_init)
		.def( "accept", &ui::expression_editor_callback_suite_t::accept)
		.def( "apply", &ui::expression_editor_callback_suite_t::apply)
		.def( "cancel", &ui::expression_editor_callback_suite_t::cancel)
		;

	// viewer
	bpy::object viewer_module( bpy::handle<>( bpy::borrowed( PyImport_AddModule( "_ramen._ui._viewer"))));
	bpy::scope().attr( "_ui._viewer") = viewer_module;
	bpy::scope viewer_scope = viewer_module;

    /*
	bpy::class_<ui::viewer::viewer_context_t, boost::noncopyable>( "viewer_context_t", bpy::no_init)
		;

	bpy::class_<ui::viewer::viewer_strategy_t, boost::noncopyable, std::auto_ptr<viewer_strategy_wrap_t> >( "viewer_strategy_t", bpy::no_init)
		;

	bpy::implicitly_convertible<std::auto_ptr<viewer_strategy_wrap_t>, std::auto_ptr<ui::viewer::viewer_strategy_t> >();
    */
}
	
} // python
} // ramen
