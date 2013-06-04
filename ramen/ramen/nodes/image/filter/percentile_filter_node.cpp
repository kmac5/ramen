// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/filter/percentile_filter_node.hpp>

#include<ramen/assert.hpp>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/float_param.hpp>

#include<ramen/image/percentile_filter.hpp>

namespace ramen
{
namespace image
{

percentile_filter_node_t::percentile_filter_node_t() : areaop_node_t() { set_name( "percent");}

void percentile_filter_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
    p->set_id( "channels");
    p->menu_items() = boost::assign::list_of( "RGBA")( "RGB")( "Alpha");
    add_param( p);

    std::auto_ptr<float_param_t> q( new float_param_t( "Radius"));
    q->set_id( "radius");
    q->set_default_value( 1);
    q->set_range( 0, 50);
    add_param( q);

    q.reset( new float_param_t( "Percent"));
    q->set_id( "percent");
    q->set_static( true);
    q->set_default_value( 0.5);
    q->set_range( 0, 1);
    add_param( q);
}

void percentile_filter_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    hradius = get_value<float>( param( "radius"));
    vradius = hradius;
}

void percentile_filter_node_t::do_process( const render::context_t& context)
{
    Imath::Box2i area( Imath::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
		return;

    int radius = get_value<float>( param( "radius")) / context.subsample;

    if( radius == 0)
    {
		boost::gil::copy_pixels( input_as<image_node_t>()->const_subimage_view( area), subimage_view( area));
		return;
    }

    switch( get_value<int>( param( "channels")))
    {
		case 0:
			image::percentile_filter_rgba( input_as<image_node_t>()->const_subimage_view( area), 
										   subimage_view( area), radius, 
										   get_value<float>( param( "percent")));
		break;
	
		case 1:
			image::percentile_filter_rgb( input_as<image_node_t>()->const_subimage_view( area),
										  subimage_view( area), radius, 
										  get_value<float>( param( "percent")));
		break;
	
		case 2:
			image::percentile_filter_alpha( input_as<image_node_t>()->const_subimage_view( area),
											subimage_view( area), radius, 
											get_value<float>( param( "percent")));
		break;
	
		default:
			RAMEN_ASSERT( 0);
    }
}

// factory
node_t *create_percentile_filter_node() { return new percentile_filter_node_t();}

const node_metaclass_t *percentile_filter_node_t::metaclass() const { return &percentile_filter_node_metaclass();}

const node_metaclass_t& percentile_filter_node_t::percentile_filter_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.percentile_filter";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Filter";
        info.menu_item = "Percentile Filter";
        info.help = "This node can do median filtering, dilating and eroding "
                    "depending on the Percent param.";

        info.create = &create_percentile_filter_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( percentile_filter_node_t::percentile_filter_node_metaclass());

} // namespace
} // namespace
