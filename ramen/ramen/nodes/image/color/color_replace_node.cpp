// Copyright (c) 2010 Sergey Dudkin

#include<ramen/nodes/image/color/color_replace_node.hpp>

#include <OpenEXR/ImathFun.h>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/color_param.hpp>
#include<ramen/params/bool_param.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/image/color.hpp>
#include<ramen/image/channel_range.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct rgb_color_replace_fun
{
   rgb_color_replace_fun( const Imath::Color4f& src_color, const Imath::Color4f& dst_color, float tolerance, float softness, bool alpha_out)
   {
	  r_range_.center = src_color.r;
	  r_range_.tol_lo = r_range_.tol_hi = tolerance;
	  r_range_.soft_lo = r_range_.soft_hi = softness; 
		  
	  g_range_.center = src_color.g;
	  g_range_.tol_lo = g_range_.tol_hi = tolerance;
	  g_range_.soft_lo = g_range_.soft_hi = softness;
		  
	  b_range_.center = src_color.b;
	  b_range_.tol_lo = b_range_.tol_hi = tolerance;
	  b_range_.soft_lo = b_range_.soft_hi = softness;
	
	  dst_ = image::pixel_t( dst_color.r, dst_color.g, dst_color.b, 1.0f);
	  dst_lum_ = image::luminance( dst_);
	  alpha_out_ = alpha_out;
   }
	   
   image::pixel_t operator()( const image::pixel_t& src) const
   {
	  using namespace boost::gil;
		  
	  float r = get_color( src, red_t());
	  float rmask = r_range_( r);
		  
	  float g = get_color( src, green_t());
	  float gmask = g_range_( g);
		  
	  float b = get_color( src, blue_t());
	  float bmask = b_range_( b);
		  
	  float final_mask = rmask * gmask * bmask;
	
	  float src_lum = image::luminance( src);
	  float lum_factor;
	
	  if( dst_lum_ != 0.0f)
		  lum_factor = src_lum / dst_lum_;
	  else
		  lum_factor = 1.0f;
	
	  image::pixel_t result;
	  get_color( result, red_t())   = Imath::lerp( r, get_color( dst_, red_t())  * lum_factor, final_mask);
	  get_color( result, green_t()) = Imath::lerp( g, get_color( dst_, green_t())* lum_factor, final_mask);
	  get_color( result, blue_t())  = Imath::lerp( b, get_color( dst_, blue_t()) * lum_factor, final_mask);

	  if( alpha_out_)
		  get_color( result, alpha_t()) = final_mask;
	  else
		  get_color( result, alpha_t()) = get_color( src, alpha_t());
	
	  return result;
   }
	
private:
	
	image::channel_range_t r_range_, g_range_, b_range_;
	image::pixel_t dst_;
	float dst_lum_;
	bool alpha_out_;
};
	
struct hsv_color_replace_fun
{
   hsv_color_replace_fun( const Imath::Color4f& src_color, const Imath::Color4f& dst_color, float tolerance, float softness, 
						  bool keep_luminance, bool alpha_out)
   {
	   Imath::Color4f hsv_src = image::rgb_to_hsv( src_color);
	   
      h_range_.center  = hsv_src.r;
      h_range_.tol_lo  = h_range_.tol_hi = tolerance;
      h_range_.soft_lo = h_range_.soft_hi = softness; 
      
      s_range_.center  = hsv_src.g;
      s_range_.tol_lo  = s_range_.tol_hi = tolerance;
      s_range_.soft_lo = s_range_.soft_hi = softness;
      
      v_range_.center  = hsv_src.b;
      v_range_.tol_lo  = v_range_.tol_hi = tolerance;
      v_range_.soft_lo = v_range_.soft_hi = softness;

      dst_ = image::rgb_to_hsv( image::pixel_t( dst_color.r, dst_color.g, dst_color.b, 1.0f));
	  keep_luminance_ = keep_luminance;
      alpha_out_ = alpha_out;
   }
   
   image::pixel_t operator()( const image::pixel_t& src) const
   {
      using namespace boost::gil;
      
	  image::pixel_t hsv_src = image::rgb_to_hsv( src);
	  
      float h = get_color( hsv_src, red_t());
      float hmask = h_range_( h);
      
      float s = get_color( hsv_src, green_t());
      float smask = s_range_( s);
      
      float v = get_color( hsv_src, blue_t());
      float vmask = v_range_( v);
      
      float final_mask = hmask * smask * vmask;

      image::pixel_t result;
      get_color( result, red_t())   = Imath::lerp( h, (float) get_color( dst_, red_t()), final_mask);
      get_color( result, green_t()) = Imath::lerp( s, (float) get_color( dst_, green_t()), final_mask);
	  
	  if( keep_luminance_)
	      get_color( result, blue_t())  = v;
	  else
	      get_color( result, blue_t())  = Imath::lerp( v, (float) get_color( dst_, blue_t()), final_mask);
	  
	  result = image::hsv_to_rgb( result);

      if( alpha_out_)
          get_color( result, alpha_t()) = final_mask;
      else
          get_color( result, alpha_t()) = get_color( src, alpha_t());

      return result;
   }

private:

    image::channel_range_t h_range_, s_range_, v_range_;
    image::pixel_t dst_;
    bool alpha_out_;
	bool keep_luminance_;
};

} //unnamed

color_replace_node_t::color_replace_node_t() : pointop_node_t() { set_name("col_replace");}

void color_replace_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> cs( new popup_param_t( "Colorspace"));
    cs->set_id( "colorspace");
    cs->menu_items() = boost::assign::list_of( "HSV")( "RGB");
	add_param( cs);
	
    std::auto_ptr<color_param_t> source( new color_param_t( "Source Color"));
    source->set_id( "src_color");
    source->set_is_rgba( false);
    source->set_default_value( Imath::Color4f( 0, 0, 0, 1));
    add_param( source);

    std::auto_ptr<color_param_t> replace(new color_param_t( "Replace Color"));
    replace->set_id( "dst_color");
    replace->set_is_rgba( false);
    replace->set_default_value( Imath::Color4f( 0, 0, 0, 1));
    add_param( replace);

    std::auto_ptr<float_param_t> tolerance( new float_param_t( "Tolerance"));
    tolerance->set_id( "tolerance");
    tolerance->set_default_value( 0);
    tolerance->set_min( 0);
    tolerance->set_step( 0.01);
    add_param( tolerance);

    std::auto_ptr<float_param_t> softness( new float_param_t( "Softness"));
    softness->set_id( "softness");
    softness->set_default_value( 0);
    softness->set_min( 0);
    softness->set_step( 0.01);
    add_param( softness);

    std::auto_ptr<bool_param_t> keep_lum( new bool_param_t( "Keep Luminance"));
    keep_lum->set_id( "keep_lum");
    keep_lum->set_default_value( true);
    add_param( keep_lum);
	
    std::auto_ptr<bool_param_t> output_alpha( new bool_param_t( "Output Alpha"));
    output_alpha->set_id( "alpha_out");
    output_alpha->set_default_value( false);
    add_param( output_alpha);
}

void color_replace_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
	if( get_value<int>( param( "colorspace")))
	{
		boost::gil::tbb_transform_pixels( src, dst, rgb_color_replace_fun( get_value<Imath::Color4f>( param( "src_color")),
																			get_value<Imath::Color4f>( param( "dst_color")),
																			get_value<float>( param("tolerance")),
																			get_value<float>( param("softness")),
																			 get_value<bool>( param( "alpha_out"))));		
	}
	else
	{
	   boost::gil::tbb_transform_pixels( src, dst, hsv_color_replace_fun( get_value<Imath::Color4f>( param( "src_color")),
																		   get_value<Imath::Color4f>( param( "dst_color")),
																		   get_value<float>( param("tolerance")),
																		   get_value<float>( param("softness")),
																		   get_value<bool>( param( "keep_lum")),
																			get_value<bool>( param( "alpha_out"))));
   }
}

//factory
node_t* create_color_replace_node() { return new color_replace_node_t();}

const node_metaclass_t *color_replace_node_t::metaclass() const { return &color_replace_node_metaclass();}

const node_metaclass_t& color_replace_node_t::color_replace_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id="image.builtin.color_replace";
        info.major_version=1;
        info.minor_version=0;
		info.menu = "Image";
        info.submenu="Color";
        info.menu_item="Color Replace";
        info.create=&create_color_replace_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( color_replace_node_t::color_replace_node_metaclass());

} // namespace
} // namespace
