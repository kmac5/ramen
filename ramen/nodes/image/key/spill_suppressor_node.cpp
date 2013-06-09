// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/key/spill_suppressor_node.hpp>

#include<ramen/params/combo_group_param.hpp>
#include<ramen/params/popup_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/bool_param.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>

namespace ramen
{
namespace image
{
namespace
{

enum
{
    basic_spill_supress_method = 0,
    rgb_cmy_spill_supress_method
};

struct basic_green_spill
{
    basic_green_spill( float amount) : amount_( amount) {}
    
    image::pixel_t operator()( const image::pixel_t& p) const
    {
		using namespace boost::gil;
	
		image::pixel_t result( p);

		if( get_color( p, green_t()) > get_color( p, red_t()) && get_color( p, green_t()) > get_color( p, blue_t()))
		{
		    get_color( result, green_t()) = ((( get_color( p, red_t()) + get_color( p, blue_t())) / 2.0f) 
											 - get_color( p, green_t())) * amount_ + get_color( p, green_t());
		}
		
		return result;
    }

private:

    float amount_;
};

struct basic_blue_spill
{
    basic_blue_spill( float amount) : amount_( amount) {}

    image::pixel_t operator()( const image::pixel_t& p) const
    {
		using namespace boost::gil;

		image::pixel_t result( p);

		if( get_color( p, blue_t()) > get_color( p, red_t()) && get_color( p, blue_t()) > get_color( p, green_t()))
		{
		    get_color( result, blue_t()) = ((( get_color( p, red_t()) + get_color( p, green_t())) / 2.0f) 
											- get_color( p, blue_t())) * amount_ + get_color( p, blue_t());
		}

		return result;
    }

private:

    float amount_;
};

struct rgb_cmy_spill
{
public:

    rgb_cmy_spill( float amount, int channel_mask) : amount_( amount), channel_mask_( channel_mask) {}

    image::pixel_t operator()( const image::pixel_t& p) const
    {
		using namespace boost::gil;
	
		float r = get_color( p, red_t());
		float g = get_color( p, green_t());
		float b = get_color( p, blue_t());
	
		// red
		if( channel_mask_ & ( 1 << 0))
        {
            float m = std::max( g, b );
            r = r > m ? m : r;
        }

		// green
		if( channel_mask_ & ( 1 << 1))
        {
            float m = std::max( r, b );
            g = g > m ? m : g;
        }

		// blue
		if( channel_mask_ & ( 1 << 2))
        {
            float m = std::max( r, g );
            b = b > m ? m : b;
        }

		// cyan
		if( channel_mask_ & ( 1 << 3))
        {
            float m = std::min( g, b );

            if( m > r )
            {
                g = g - ( m - r );
                b = b - ( m - r );
            }
        }

		// magenta
		if( channel_mask_ & ( 1 << 4))
        {
            float m = std::min( r, b );

            if( m > g )
            {
                r = r - ( m - g );
                b = b - ( m - g );
            }
        }

		// yellow
		if( channel_mask_ & ( 1 << 5))
        {
            float m = std::min( r, g );

            if( m > b )
            {
                r = r - ( m - b );
                g = g - ( m - b );
            }
        }

		return image::pixel_t(	r * amount_ + ( 1.0f - amount_) * get_color( p, red_t()),
								g * amount_ + ( 1.0f - amount_) * get_color( p, green_t()),
								b * amount_ + ( 1.0f - amount_) * get_color( p, blue_t()),
								get_color( p, alpha_t()));
    }

private:

    float amount_;
    int channel_mask_;
};

} // unnamed

spill_suppressor_node_t::spill_suppressor_node_t() : pointop_node_t() { set_name("spill");}

void spill_suppressor_node_t::do_create_params()
{
    std::auto_ptr<combo_group_param_t> top( new combo_group_param_t( "Method"));
    top->set_id( "method");

    {
		std::auto_ptr<composite_param_t> group( new composite_param_t( "Basic"));
		group->set_id( "basic");

		std::auto_ptr<popup_param_t> p( new popup_param_t( "Color"));
		p->set_id( "bcolor");
		p->menu_items() = boost::assign::list_of( "Blue")( "Green");
		group->add_param( p);

		std::auto_ptr<float_param_t> q( new float_param_t( "Amount"));
		q->set_id( "bamount");
		q->set_default_value( 1.0f);
		q->set_range( 0, 5);
		q->set_step( 0.05);
		group->add_param( q);

		top->add_param( group);
    }

    {
		std::auto_ptr<composite_param_t> group( new composite_param_t( "RGB-CMY"));
		group->set_id( "rgb_cmy");

		std::auto_ptr<bool_param_t> p( new bool_param_t( "Red"));
		p->set_id( "dred");
		p->set_default_value( false);
		group->add_param( p);

		p.reset( new bool_param_t( "Green"));
		p->set_id( "dgreen");
		p->set_default_value( false);
		group->add_param( p);
	
		p.reset( new bool_param_t( "Blue"));
		p->set_id( "dblue");
		p->set_default_value( true);
		group->add_param( p);
	
		p.reset( new bool_param_t( "Cyan"));
		p->set_id( "dcyan");
		p->set_default_value( false);
		group->add_param( p);
	
		p.reset( new bool_param_t( "Magenta"));
		p->set_id( "dmagenta");
		p->set_default_value( false);
		group->add_param( p);
	
		p.reset( new bool_param_t( "Yellow"));
		p->set_id( "dyellow");
		p->set_default_value( false);
		group->add_param( p);

		std::auto_ptr<float_param_t> q( new float_param_t( "Amount"));
		q->set_id( "damount");
		q->set_default_value( 1.0f);
		q->set_range( 0, 5);
		q->set_step( 0.05);
		group->add_param( q);

		top->add_param( group);
	}

    add_param( top);
}

void spill_suppressor_node_t::do_process( const image::const_image_view_t& src,
				const image::image_view_t& dst, const render::context_t& context)
{
    // basic
    if( get_value<int>( param( "method")) == basic_spill_supress_method)
    {
		if( get_value<int>( param( "bcolor")))
			boost::gil::tbb_transform_pixels( src, dst, basic_green_spill( get_value<float>( param( "bamount"))));
		else
			boost::gil::tbb_transform_pixels( src, dst, basic_blue_spill( get_value<float>( param( "bamount"))));
    }
    else // rgb_cmy
    {
		int channel_mask = 0;
		if( get_value<bool>( param( "dred"))) channel_mask |= ( 1 << 0);
		if( get_value<bool>( param( "dgreen"))) channel_mask |= ( 1 << 1);
		if( get_value<bool>( param( "dblue"))) channel_mask |= ( 1 << 2);
		if( get_value<bool>( param( "dcyan"))) channel_mask |= ( 1 << 3);
		if( get_value<bool>( param( "dmagenta"))) channel_mask |= ( 1 << 4);
		if( get_value<bool>( param( "dyellow"))) channel_mask |= ( 1 << 5);
		boost::gil::tbb_transform_pixels( src, dst, rgb_cmy_spill( get_value<float>( param( "damount")), channel_mask));
    }
}

// factory

node_t *create_spill_suppressor_node() { return new spill_suppressor_node_t();}

const node_metaclass_t *spill_suppressor_node_t::metaclass() const { return &spill_supressor_node_metaclass();}

const node_metaclass_t& spill_suppressor_node_t::spill_supressor_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.spill_suppress";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Key";
        info.menu_item = "Spill Suppressor";
        info.create = &create_spill_suppressor_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( spill_suppressor_node_t::spill_supressor_node_metaclass());

} // namespace
} // namespace
