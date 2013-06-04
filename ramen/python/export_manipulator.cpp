// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<memory>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/manipulators/manipulator.hpp>

namespace ramen
{
namespace python
{
namespace
{

} // unnamed

/*
struct manipulator_wrap_t : public manipulator_t
{
public:

	manipulator_wrap_t( PyObject *self) : self_( self) { Py_INCREF( self_);}
	virtual ~manipulator_wrap_t() { Py_DECREF( self_);}

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

	void default_do_draw_overlay( const manipulator_t& this_, const ui::paint_event_t& event) const
	{
		this_.__do_draw_overlay( event);
	}

private:

    PyObject *self_;
};
*/

void export_manipulator()
{
    /*
	bpy::class_<manipulator_t, boost::noncopyable, std::auto_ptr<manipulator_wrap_t> >( "manipulator_t", bpy::no_init)
			.def( "draw_overlay", &manipulator_t::draw_overlay)
			.def( "do_draw_overlay", &manipulator_wrap_t::default_do_draw_overlay)
			;

	bpy::implicitly_convertible<std::auto_ptr<manipulator_wrap_t>, std::auto_ptr<manipulator_t> >();
    */
}
	
} // python
} // ramen
