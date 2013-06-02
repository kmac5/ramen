// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/track/vis_flow_node.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>

namespace ramen
{
namespace image
{
namespace
{

// util
int ncols = 0;
#define MAXCOLS 60
float colorwheel[MAXCOLS][3];
	
void setcols( float r, float g, float b, int k)
{
	colorwheel[k][0] = r;
	colorwheel[k][1] = g;
	colorwheel[k][2] = b;
}
	
void makecolorwheel()
{
	// relative lengths of color transitions:
	// these are chosen based on perceptual similarity
	// (e.g. one can distinguish more shades between red and yellow 
	//  than between yellow and green)
	int RY = 15;
	int YG = 6;
	int GC = 4;
	int CB = 11;
	int BM = 13;
	int MR = 6;
	ncols = RY + YG + GC + CB + BM + MR;
			
	int i;
	int k = 0;
	for (i = 0; i < RY; i++) setcols( 1.0f, (float) i / RY, 0.0f, k++);
	for (i = 0; i < YG; i++) setcols( 1.0f - (float) i / YG, 1.0f,	0.0f, k++);
	for (i = 0; i < GC; i++) setcols( 1.0f, 1.0f, (float) i / GC, k++);
	for (i = 0; i < CB; i++) setcols( 1.0f,	1.0f - ( float) i / CB, 1.0f, k++);
	for (i = 0; i < BM; i++) setcols( (float) i / BM, 0.0f, 1.0f, k++);
	for (i = 0; i < MR; i++) setcols( 1.0f,	0.0f, 1.0f - (float) i / MR, k++);
}
	
void compute_color( float fx, float fy, float *pix)
{
	if( ncols == 0)
		makecolorwheel();
	
	float rad = std::sqrt( fx * fx + fy * fy);
	float a = std::atan2(-fy, -fx) / M_PI;
	float fk = (a + 1.0) / 2.0 * (ncols-1);
	int k0 = (int)fk;
	int k1 = (k0 + 1) % ncols;
	float f = fk - k0;

	for (int b = 0; b < 3; b++)
	{
		float col0 = colorwheel[k0][b];
		float col1 = colorwheel[k1][b];
		float col = (1 - f) * col0 + f * col1;
			
		if (rad <= 1)
			col = 1 - rad * (1 - col); // increase saturation with radius
		else
			col *= .75; // out of range
	
		pix[2 - b] = col;
	}
}

struct color_flow
{
	color_flow( bool backwards = false) : backwards_( backwards) {}
	
	image::pixel_t operator()( const image::pixel_t& p) const
	{
		float fx, fy;
		
		if( backwards_)
		{
			fx = boost::gil::get_color( p, boost::gil::blue_t());
			fy = boost::gil::get_color( p, boost::gil::alpha_t());
		}
		else
		{
			fx = boost::gil::get_color( p, boost::gil::red_t());
			fy = boost::gil::get_color( p, boost::gil::green_t());
		}
		
		image::pixel_t result( 0, 0, 0, 1);
		compute_color( fx, fy, (float*) &result);
		return result;
	}
	
	bool backwards_;
};

} // unnamed

vis_flow_node_t::vis_flow_node_t() : pointop_node_t() { set_name( "vis_flow");}

void vis_flow_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    boost::gil::tbb_transform_pixels( src, dst, color_flow());
}

// factory
node_t *create_vis_flow_node() { return new vis_flow_node_t();}

const node_metaclass_t *vis_flow_node_t::metaclass() const { return &vis_flow_node_metaclass();}

const node_metaclass_t& vis_flow_node_t::vis_flow_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.vis_flow";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Track";
        info.menu_item = "Visualize Flow";
        info.create = &create_vis_flow_node;
        inited = true;
    }

    return info;
}

#ifndef NDEBUG
	static bool registered = node_factory_t::instance().register_node( vis_flow_node_t::vis_flow_node_metaclass());
#endif

} // namespace
} // namespace
