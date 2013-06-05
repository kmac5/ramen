// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/distort/turbulent_displace_node.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/image/processing.hpp>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/noise/global_noise.hpp>
#include<ramen/noise/turbulence.hpp>
#include<ramen/noise/vector_noise_adaptor.hpp>

namespace ramen
{
namespace image
{
namespace
{
enum
{
    border_black  = 0,
    border_tile,
    border_mirror
};

template<class TurbGen>
struct turbulent_warp_fun
{
    typedef Imath::V2f result_type;
    
    turbulent_warp_fun( const TurbGen& tgen, const Imath::V2f& amplitude, 
                        const Imath::V2f& size, const Imath::V2f& shift, const Imath::V2f& depth, 
                        float t, float subsample, float aspect) : tgen_( tgen)
    {
        amplitude_ = 2.0f * amplitude / subsample;
        size_ = size;
        shift_ = shift;
        depth_ = depth;
        time_ = t;
        subsample_ = subsample;
		aspect_ = aspect;
    }
	
    Imath::V2f operator()( const Imath::V2f& p) const
    {
		Imath::V3f q( ( p.x * subsample_ * aspect_ / size_.x) + shift_.x, ( p.y * subsample_ / size_.y) + shift_.y, time_);
        Imath::V2f d( tgen_( q));
		
        return Imath::V2f( p.x + ( amplitude_.x * d.x),
						   p.y + ( amplitude_.y * d.y));
    }

private:

    const TurbGen& tgen_;
    Imath::V2f amplitude_;
    Imath::V2f size_, shift_, depth_;
    float time_;
    float subsample_;
	float aspect_;
};

struct mask_fun
{
    mask_fun( const image::const_image_view_t& mask, const Imath::Box2i& area) : mask_( mask), area_( area) {}

    float operator()( const Imath::V2f& p) const
    {
        using namespace boost::gil;

        if( ( p.x >= area_.min.x) && ( p.x <= area_.max.x))
        {
            if( ( p.y >= area_.min.y) && ( p.y <= area_.max.y))
                return get_color( mask_( p.x - area_.min.x, p.y - area_.min.y), alpha_t());
        }

        return 0.0f;
    }

private:

    const image::const_image_view_t& mask_;
    Imath::Box2i area_;
};

template<class WarpFun, class MaskFun>
struct masked_warp_fun
{
public:

    masked_warp_fun( WarpFun f, MaskFun mf) : f_( f), mf_( mf) {}

    Imath::V2f operator()( const Imath::V2f& p) const
    {
        float mask = mf_( p);

        if( mask != 0.0f)
        {
            Imath::V2f q( f_( p));
            return Imath::lerp( p, q, mask);
        }

        return p;
    }

private:

    WarpFun f_;
    MaskFun mf_;
};

} // unnamed

turbulent_displace_node_t::turbulent_displace_node_t() : distort_node_t()
{
    set_name( "turb_disp");
    add_input_plug( "mask", false, ui::palette_t::instance().color( "matte plug"), "Mask");
}

void turbulent_displace_node_t::do_create_params()
{
	std::auto_ptr<float2_param_t> q( new float2_param_t( "Amplitude"));
	q->set_id( "amplitude");
	q->set_proportional( true);
	q->set_min( 0);
	q->set_default_value( Imath::V2f( 0, 0));
	q->set_step( 0.05f);
	add_param( q);
	
	q.reset( new float2_param_t( "Size"));
	q->set_id( "size");
	q->set_proportional( true);
	q->set_default_value( Imath::V2f( 100, 100));
	q->set_min( 0.01);
	q->set_step( 0.05);
	add_param( q);
	
	q.reset( new float2_param_t( "Shift"));
	q->set_id( "shift");
	q->set_default_value( Imath::V2f( 0, 0));
	q->set_step( 0.05);
	add_param( q);
	
	std::auto_ptr<float_param_t> p( new float_param_t( "Time"));
	p->set_id( "time");
	p->set_default_value( 0);
	p->set_min( 0);
	p->set_step( 0.05);
	add_param( p);
	
	p.reset( new float_param_t( "Octaves"));
	p->set_id( "octaves");
	p->set_static( true);
	p->set_round_to_int( true);
	p->set_default_value( 1);
	p->set_min( 1);
	add_param( p);
	
	p.reset( new float_param_t( "Gain"));
	p->set_id( "gain");
	p->set_default_value( 0.5);
	p->set_range( 0.1f, 1.0f);
	p->set_step( 0.05f);
	add_param( p);
	
	p.reset( new float_param_t( "Lacunarity"));
	p->set_id( "lacunarity");
	p->set_default_value( 1.9997);
	p->set_range( 1, 10);
	p->set_step( 0.25f);
	add_param( p);

    std::auto_ptr<popup_param_t> r( new popup_param_t( "Borders"));
    r->set_id( "borders");
    r->menu_items() = boost::assign::list_of( "Black")( "Tile")( "Mirror");
    add_param( r);	
}

bool turbulent_displace_node_t::do_is_identity() const
{
    Imath::V2f amplitude = get_value<Imath::V2f>( param( "amplitude"));	
	return amplitude == Imath::V2f( 0, 0);
}

void turbulent_displace_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::V2f amplitude = get_value<Imath::V2f>( param( "amplitude"));
    Imath::Box2i b( input_as<image_node_t>( 0)->bounds());

    if( !b.isEmpty())
    {
        b.min.x -= amplitude.x;
        b.min.y -= amplitude.y;
        b.max.x += amplitude.x;
        b.max.y += amplitude.y;
        
        if( input( 1))
        {
            Imath::Box2i b2( input_as<image_node_t>( 1)->bounds());
            b2.min.x -= amplitude.x;
            b2.min.y -= amplitude.y;
            b2.max.x += amplitude.x;
            b2.max.y += amplitude.y;
            b2.extendBy( input_as<image_node_t>( 0)->bounds());
            b = ImathExt::intersect( b, b2);
        }
    }

    set_bounds( b);
}

void turbulent_displace_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    Imath::V2f amplitude = get_value<Imath::V2f>( param( "amplitude"));

    Imath::Box2i roi( interest());
    
    if( input( 1))
        input_as<image_node_t>( 1)->add_interest( roi);

	image_node_t *in = input_as<image_node_t>( 0);
	
	if( get_value<int>( param( "borders")) == border_black)
	{
	    roi.min.x -= amplitude.x;
	    roi.min.y -= amplitude.y;
	    roi.max.x += amplitude.x;
	    roi.max.y += amplitude.y;
	    in->add_interest( roi);
	}
	else
		in->add_interest( in->format());
}

void turbulent_displace_node_t::do_process( const render::context_t& context)
{	
	int octaves = get_value<float>( param( "octaves"));
	float lac = get_value<float>( param( "lacunarity"));
	float gain = get_value<float>( param( "gain"));
	
    // warp
	typedef noise::simplex_noise_t noise_gen_type;
	typedef noise::vector_noise_adaptor2_t<noise_gen_type, noise_gen_type> vector_noise_type;
	typedef noise::turbulence_t<vector_noise_type> turbulence_type;
	
	vector_noise_type vgen( noise::global_noise, noise::global_noise1);
	turbulence_type tgen( vgen, octaves, gain, lac);

	Imath::V2f amp( get_value<Imath::V2f>( param( "amplitude")));
	Imath::V2f size( get_value<Imath::V2f>( param( "size")));
	Imath::V2f shift( get_value<Imath::V2f>( param( "shift")));
	
    turbulent_warp_fun<turbulence_type> f( tgen, amp, size, shift,
										    Imath::V2f( 0, 2),
										    get_value<float>( param( "time")),
										    context.subsample, aspect_ratio());

    image_node_t *in  = input_as<image_node_t>( 0);
    image_node_t *msk = input_as<image_node_t>( 1);

    if( msk)
    {
        mask_fun m( msk->const_image_view(), msk->defined());
        masked_warp_fun<turbulent_warp_fun<turbulence_type>, mask_fun> mf( f, m);

        switch( get_value<int>( param( "borders")))
        {
			case border_black:
				image::warp_bilinear( in->defined(), in->const_image_view(), defined(), image_view(), mf, false, false);
			break;
	
			case border_tile:
				image::warp_bilinear_tile( in->defined(), in->const_image_view(), defined(), image_view(), mf, false, false);
			break;
	
			case border_mirror:
				image::warp_bilinear_mirror( in->defined(), in->const_image_view(), defined(), image_view(), mf, false, false);
			break;
        }
    }
    else
    {
        switch( get_value<int>( param( "borders")))
        {
			case border_black:
				image::warp_bilinear( in->defined(), in->const_image_view(), defined(), image_view(), f, false, false);
			break;
	
			case border_tile:
				image::warp_bilinear_tile( in->defined(), in->const_image_view(), defined(), image_view(), f, false, false);
			break;
	
			case border_mirror:
				image::warp_bilinear_mirror( in->defined(), in->const_image_view(), defined(), image_view(), f, false, false);
			break;
        }
    }
}

// factory
node_t *create_turbulent_displace_node() { return new turbulent_displace_node_t();}

const node_metaclass_t *turbulent_displace_node_t::metaclass() const { return &turbulent_displace_node_metaclass();}

const node_metaclass_t& turbulent_displace_node_t::turbulent_displace_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
		info.id = "image.builtin.turb_displace";
		info.major_version = 1;
		info.minor_version = 0;
		info.menu = "Image";
		info.submenu = "Distort";
		info.menu_item = "Turbulent Displace";
        info.help = "Displace the input image, using a fractal noise texture.";
		info.create = &create_turbulent_displace_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( turbulent_displace_node_t::turbulent_displace_node_metaclass());

} // namespace
} // namespace
