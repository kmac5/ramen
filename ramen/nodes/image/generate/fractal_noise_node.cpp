// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/generate/fractal_noise_node.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/algorithm/clamp.hpp>

#include<ramen/assert.hpp>

#include<ramen/noise/simplex_noise.hpp>
#include<ramen/noise/perlin.hpp>
#include<ramen/noise/turbulence.hpp>

#include<ramen/nodes/node_factory.hpp>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/bool_param.hpp>

namespace ramen
{
namespace image
{
namespace
{

enum
{
	classic_noise = 0,
	simplex_noise,
};
	
template<class TurbGen>
struct fractal_noise_fun
{
    fractal_noise_fun( const TurbGen& tgen, const Imath::V2f& size, const Imath::V2f& translate, float t, 
                        float contrast, float brightness, bool invert, float aspect = 1.0f) : tgen_( tgen)
    {
		RAMEN_ASSERT( aspect > 0.0f);
		
        size_ = size;
		translate_ = translate;
        time_ = t;
        contrast_ = contrast;
        brightness_ = brightness;
        invert_ = invert;
		aspect_ = aspect;
    }

    image::pixel_t operator()( const Imath::V2i& p) const
    {
        float val = tgen_( ( p.x * aspect_ / size_.x) + ( translate_.x / aspect_),
						   ( p.y / size_.y) + translate_.y,
						   time_);
		
        val = (val + 1.0f) * 0.5f;
        val = Imath::lerp( 0.5f, val, contrast_);
        val *= brightness_;
        val = clamp( val, 0.0f, 1.0f);
        val = invert_ ? 1.0f - val : val;
        return image::pixel_t( val, val, val, val);
    }

private:
	
	const TurbGen& tgen_;
    Imath::V2f size_, translate_;
    float time_, contrast_;
    float brightness_;
    bool invert_;
	float aspect_;
};

} // namespace

fractal_noise_node_t::fractal_noise_node_t() : generator_node_t() { set_name( "fractal_noise");}

void fractal_noise_node_t::do_create_params()
{
    generator_node_t::do_create_params();

    std::auto_ptr<popup_param_t> type( new popup_param_t( "Noise"));
    type->set_id( "noise_type");
    type->menu_items() = boost::assign::list_of( "Classic")( "Simplex");
    add_param( type);

    type.reset( new popup_param_t( "Type"));
    type->set_id( "type");
    type->menu_items() = boost::assign::list_of( "Fbm")( "Turbulence");
    add_param( type);

    std::auto_ptr<float2_param_t> p2( new float2_param_t( "Size"));
    p2->set_id( "size");
	p2->set_numeric_type( numeric_param_t::relative_size_xy);
    p2->set_proportional( true);
    p2->set_default_value( Imath::V2f( 0.05f, 0.05f));
    add_param( p2);

    p2.reset( new float2_param_t( "Translate"));
    p2->set_id( "translate");
    p2->set_default_value( Imath::V2f( 0, 0));
    p2->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p2);
	
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
    p->set_default_value( 4);
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
    p->set_default_value( 2);
    p->set_range( 1, 10);
    p->set_step( 0.25f);
    add_param( p);

    p.reset( new float_param_t( "Contrast"));
    p->set_id( "contrast");
    p->set_default_value( 1);
    p->set_range( 0, 10);
    p->set_step( 0.1f);
    add_param( p);

    p.reset( new float_param_t( "Brightness"));
    p->set_id( "brightness");
    p->set_default_value( 1);
    p->set_range( 0, 10);
    p->set_step( 0.1f);
    add_param( p);

    std::auto_ptr<bool_param_t>b( new bool_param_t( "Invert"));
    b->set_id( "invert");
    b->set_default_value( false);
    add_param( b);
}

void fractal_noise_node_t::do_process( const render::context_t& context)
{
	switch( get_value<int>( param( "noise_type")))
	{
		case classic_noise:
		{
			noise::perlin_t ngen;

			noise::turbulence_t<noise::perlin_t> tgen( ngen, get_value<float>( param( "octaves")),
													  get_value<float>( param( "gain")),
													  get_value<float>( param( "lacunarity")),
													  get_value<int>( param( "type")));
			
			fractal_noise_fun<noise::turbulence_t<noise::perlin_t> > f( tgen,
																		get_absolute_value<Imath::V2f>( param( "size")) / context.subsample,
																		get_absolute_value<Imath::V2f>( param( "translate")) / context.subsample,
																		get_value<float>( param( "time")),
																		get_value<float>( param( "contrast")),
																		get_value<float>( param( "brightness")),
																		get_value<bool>( param( "invert")), 
																		aspect_ratio());
			generate_pixels( f);
		}
		break;
		
		case simplex_noise:
		{
			noise::simplex_noise_t ngen;

			noise::turbulence_t<noise::simplex_noise_t> tgen( ngen, get_value<float>( param( "octaves")),
															  get_value<float>( param( "gain")),
															  get_value<float>( param( "lacunarity")),
															  get_value<int>( param( "type")));
			
			fractal_noise_fun<noise::turbulence_t<noise::simplex_noise_t> > f( tgen,
																		get_absolute_value<Imath::V2f>( param( "size")) / context.subsample,
																		get_absolute_value<Imath::V2f>( param( "translate")) / context.subsample,
																		get_value<float>( param( "time")),
																		get_value<float>( param( "contrast")),
																		get_value<float>( param( "brightness")),
																		get_value<bool>( param( "invert")), 
																		aspect_ratio());
			generate_pixels( f);
		}
		break;
	}
}

// factory
node_t *create_fractal_noise_node() { return new fractal_noise_node_t();}

const node_metaclass_t *fractal_noise_node_t::metaclass() const { return &fractal_noise_node_metaclass();}

const node_metaclass_t& fractal_noise_node_t::fractal_noise_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.fractal_noise";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Input";
        info.menu_item = "Fractal Noise";
        info.create = &create_fractal_noise_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( fractal_noise_node_t::fractal_noise_node_metaclass());

} // namespace
} // namespace
