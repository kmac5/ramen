// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/matte/trimap_node.hpp>

#include<algorithm>
#include<cmath>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/params/bool_param.hpp>
#include<ramen/params/float_param.hpp>

#include<ramen/image/processing.hpp>
#include<ramen/image/box_blur.hpp>
#include<ramen/image/dilate.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct thereshold_alpha_fun
{
    thereshold_alpha_fun( float lo, float hi) : alpha_low_( lo), alpha_high_( hi) {}

    image::pixel_t operator()( const image::pixel_t& p) const
    {
		float a = boost::gil::get_color( p, boost::gil::alpha_t());
	
		if( a <= alpha_low_)
			a = 0;
		else
		{
			if( a >= alpha_high_ || alpha_high_ == alpha_low_)
				a = 1;
			else
				a = 0.5f;
		}
		
		image::pixel_t result( p);
		boost::gil::get_color( result, boost::gil::alpha_t()) = a;
		return result;
    }

private:

    float alpha_low_, alpha_high_;
};

bool is_unknown( const image::pixel_t& p)
{
	float v = boost::gil::get_color( p, boost::gil::alpha_t());
	
	if( v < 0.025f || v > 0.975f)
		return false;
	
	return true;
}

void set_unknown( image::pixel_t& p)
{
	boost::gil::get_color( p, boost::gil::alpha_t()) = 0.5f;
}

} // unnamed

trimap_node_t::trimap_node_t() : areaop_node_t() { set_name( "trimap");}

void trimap_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Alpha Low"));
    p->set_id( "alpha_low");
    p->set_default_value( 0.025);
    p->set_range( 0, 1);
    p->set_step( 0.05);
    add_param( p);

    p.reset( new float_param_t( "Alpha High"));
    p->set_id( "alpha_high");
    p->set_default_value( 0.975);
    p->set_range( 0, 1);
    p->set_step( 0.05);
    add_param( p);

    p.reset( new float_param_t( "Grow"));
    p->set_id( "grow");
    p->set_default_value( 0);
    p->set_min( 0);
    add_param( p);
}

void trimap_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    float grow = get_value<float>( param( "grow"));
	hradius = std::ceil( ( double) grow);
	vradius = hradius;
}

bool trimap_node_t::expand_defined() const { return true;}

void trimap_node_t::do_process( const render::context_t& context)
{
    using namespace boost::gil;

    Imath::Box2i area( Imath::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
		return;
	
    boost::gil::tbb_transform_pixels( input_as<image_node_t>()->const_subimage_view( area), 
									  subimage_view( area), 
									  thereshold_alpha_fun( get_value<float>( param( "alpha_low")),
															get_value<float>( param( "alpha_high"))));

    int grow = std::ceil( ( double) get_value<float>( param( "grow")));
	
	std::vector<Imath::V2i> pts;
	for( int i = 0; i < grow; ++i)
		grow_trimap( pts);
}

void trimap_node_t::grow_trimap( std::vector<Imath::V2i>& pts)
{
    pts.clear();
	
	image::image_view_t view( image_view());
	
	image::image_view_t::xy_locator src_loc = view.xy_at( 1, 1);
    image::image_view_t::xy_locator::cached_location_t above = src_loc.cache_location( 0,-1);
    image::image_view_t::xy_locator::cached_location_t below = src_loc.cache_location( 0, 1);
    image::image_view_t::xy_locator::cached_location_t right = src_loc.cache_location( 1, 0);
    image::image_view_t::xy_locator::cached_location_t left  = src_loc.cache_location(-1, 0);
	
    for( int y=1; y<view.height()-1; ++y)
    {
        for( int x=1; x<view.width()-1; ++x)
        {
            if( !is_unknown( *src_loc))
            {
                if( is_unknown( src_loc[above]) || is_unknown( src_loc[below]) || is_unknown( src_loc[left]) || is_unknown( src_loc[right]))
                    pts.push_back( Imath::V2i( x, y));
            }
			
            ++src_loc.x();
        }
		
        src_loc += boost::gil::point2<std::ptrdiff_t>( -( view.width() - 2), 1);
    }
	
    for( std::vector<Imath::V2i>::const_iterator it = pts.begin(); it != pts.end(); ++it)
		set_unknown( view( it->x, it->y));
}

// factory
node_t *create_trimap_node() { return new trimap_node_t();}

const node_metaclass_t *trimap_node_t::metaclass() const { return &trimap_node_metaclass();}

const node_metaclass_t& trimap_node_t::trimap_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.trimap";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Matte";
        info.menu_item = "Trimap";
        info.create = &create_trimap_node;
        inited = true;
    }

    return info;
}

#ifndef NDEBUG
	static bool registered = node_factory_t::instance().register_node( trimap_node_t::trimap_node_metaclass());
#endif

} // namespace
} // namespace
