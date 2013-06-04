// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<boost/python.hpp>
namespace bpy = boost::python;

#include<ramen/nodes/image_node.hpp>

#include<ramen/python/util.hpp>

namespace ramen
{
namespace python
{
namespace
{

/*
std::size_t convert_index( long index, std::size_t len)
{
    long s = len;

    if( index < 0 )
        index += s;

    if( index >= s || index < 0 )
    {
        PyErr_SetString( PyExc_IndexError, "Index out of range");
        boost::python::throw_error_already_set();
    }

    return index;
}

class node_inputs_vector_proxy_t
{
public:

    node_inputs_vector_proxy_t( node_t& n) : n_( n) {}

    std::size_t len() const { return n_.num_inputs();}
    node_ptr_t getitem( long index) { return n_.input( convert_index( index, n_.num_inputs()));}

    static void bind()
    {
        boost::python::class_<node_inputs_vector_proxy_t>( "node_inputs_vector", boost::python::no_init)
            .def( "__len__", &node_inputs_vector_proxy_t::len)
            .def( "__getitem__", &node_inputs_vector_proxy_t::getitem)
            ;
    }

private:

    node_t& n_;
};

class node_outputs_vector_proxy_t
{
public:

    node_outputs_vector_proxy_t( node_t& n) : n_( n) {}

    std::size_t len() const { return n_.num_outputs();}
    node_ptr_t getitem( long index) { return n_.output( convert_index( index, n_.num_outputs()));}

    static void bind()
    {
        boost::python::class_<node_outputs_vector_proxy_t>( "node_outputs_vector", boost::python::no_init)
            .def( "__len__", &node_outputs_vector_proxy_t::len)
            .def( "__getitem__", &node_outputs_vector_proxy_t::getitem)
            ;
    }

private:

    node_t& n_;
};

node_ptr_t input( node_t& n, int i)
{
	if( i >= n.num_inputs())
	{
		PyErr_SetString( PyExc_IndexError, "Index out of range");
		boost::python::throw_error_already_set();
	}

	return node_ptr_t( n.input( i));
}

node_ptr_t output( node_t& n, int i)
{
	if( i >= n.num_outputs())
	{
		PyErr_SetString( PyExc_IndexError, "Index out of range");
		boost::python::throw_error_already_set();
	}

	return node_ptr_t( n.output( i));
}

node_inputs_vector_proxy_t make_ro_inputs_list( node_t& n) { return node_inputs_vector_proxy_t( n);}
node_outputs_vector_proxy_t make_ro_outputs_list( node_t& n) { return node_outputs_vector_proxy_t( n);}

bpy::list get_location( const node_t& n) { return vec_to_list( n.location());}

void set_location( node_t& n, const bpy::list& loc) { n.set_location( list_to_vec2<float>( loc));}

bpy::list get_format( const image_node_t& n) { return box_to_list( n.format());}

// casts

bool is_image_node( node_t& n)
{
	return dynamic_cast<image_node_t*>( &n) != 0;
}

image_node_ptr_t as_image_node( node_t& n)
{
	return image_node_ptr_t( dynamic_cast<image_node_t*>( &n));
}
*/
} // namespace

void export_node()
{

    /*
	node_inputs_vector_proxy_t::bind();
	node_outputs_vector_proxy_t::bind();

	bpy::class_<node_t, bpy::bases<manipulable_t>, boost::noncopyable, node_ptr_t>( "node_t", bpy::no_init)
			.def( "selected", &node_t::selected)

			.def( "autolayout", &node_t::autolayout)
			.def( "set_autolayout", &node_t::set_autolayout)

			.def( "location", get_location)
			.def( "set_location", set_location)

			.def( "ignored", &node_t::ignored)
			.def( "set_ignored", &node_t::set_ignored)

			.def( "num_inputs", &node_t::num_inputs)
			.def( "inputs", make_ro_inputs_list)
			.def( "input", input)

			.def( "has_output", &node_t::has_output_plug)
			.def( "outputs", make_ro_outputs_list)
			.def( "output", output)

			.def( "is_image_node", is_image_node)
			.def( "as_image_node", as_image_node)

			.def( "is_active", &node_t::is_active)
			.def( "is_context", &node_t::is_context)

			//.def( "notify", &node_t::notify)
			;

	bpy::class_<image_node_t, bpy::bases<node_t>, image_node_ptr_t, boost::noncopyable>( "image_node_t", bpy::no_init)
			.def( "format", get_format)
			;
            */
}

} // python
} // ramen
