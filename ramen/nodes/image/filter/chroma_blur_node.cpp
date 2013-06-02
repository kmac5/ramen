// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/filter/chroma_blur_node.hpp>

#include<tbb/parallel_for.h>
#include<tbb/blocked_range.h>

#include<OpenEXR/ImathVec.h>

#include<ramen/params/float_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct apply_chroma_joint_bilateral
{
    apply_chroma_joint_bilateral( const image::const_image_view_t& src, const image::image_view_t& dst,
                                    const image::const_gray_image_view_t& filter, int xoff, int yoff, float csigma) : src_( src), dst_( dst), filter_( filter)
    {
		xoff_ = xoff;
		yoff_ = yoff;
        color_sigma_ = csigma;
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
        float color_sigma_mult = 0.5f / ( color_sigma_ * color_sigma_);

        for (int y = r.begin(); y < r.end(); y++)
        {
            for (int x = 0; x < src_.width(); x++)
            {
                float total_weight = 0;

                for( int dy = -yoff_; dy <= yoff_; dy++)
                {
                    int imy = y + dy;

                    if( imy < 0)
						continue;
					
                    if( imy >= src_.height())
						break;

                    int filtery = dy + yoff_;
                    
                    for( int dx = -xoff_; dx <= xoff_; dx++)
                    {
                        int imx = x + dx;

                        if( imx < 0)
							continue;
						
                        if( imx >= src_.width())
							break;

                        int filterx = dx + xoff_;
                        float weight = filter_( filterx, filtery)[0];

                        float diff = src_( imx, imy)[0] - src_( x, y)[0];
                        float lum_distance = diff * diff * color_sigma_mult;

                        weight *= std::exp( (double) -lum_distance);
                        total_weight += weight;

                        dst_( x, y)[1] += weight * src_( imx, imy)[1];
                        dst_( x, y)[2] += weight * src_( imx, imy)[2];
                    }
                }

                dst_( x, y)[0] = src_( x, y)[0];
                dst_( x, y)[1] /= total_weight;
                dst_( x, y)[2] /= total_weight;
                dst_( x, y)[3] = src_( x, y)[3];
            }
        }
    }

private:

    const image::const_image_view_t& src_;
    const image::image_view_t& dst_;
	
    const image::const_gray_image_view_t& filter_;
	int xoff_, yoff_;
	
    float color_sigma_;
};

} // unnamed

chroma_blur_node_t::chroma_blur_node_t() : areaop_node_t() { set_name( "chroma_blur");}

void chroma_blur_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> q( new float_param_t( "Spatial"));
    q->set_id( "spatial");
    q->set_range( 0, 10);
    q->set_step( 0.25);
    q->set_default_value( 1);
    add_param( q);

    q.reset( new float_param_t( "Color"));
    q->set_id( "color");
    q->set_range( 0, 20);
    q->set_step( 0.05);
    q->set_default_value( 0.075);
    add_param( q);
}

void chroma_blur_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    Imath::V2f stddev;
	stddev.y = get_value<float>( param( "spatial"));
	
	stddev.x = stddev.y / aspect_ratio() * proxy_scale().x;
	stddev.y = stddev.y * proxy_scale().y;

	int sizex = (int)( stddev.x * 6 + 1) | 1;

	if( sizex == 1)
		sizex = 3;

	int sizey = (int)( stddev.y * 6 + 1) | 1;

	if( sizey == 1)
		sizey = 3;

    hradius = ( sizex - 1) / 2;
    vradius = ( sizey - 1) / 2;
}

void chroma_blur_node_t::do_process( const render::context_t& context)
{
    Imath::Box2i area( Imath::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
		return;

    image::const_image_view_t src( input_as<image_node_t>()->const_subimage_view( area));
    image::image_view_t dst( subimage_view( area));

    image::image_t tmp( src.width(), src.height());
    image::convert_rgb_to_yuv( src, boost::gil::view( tmp));

    // create the kernel
    Imath::V2f stddev;
	stddev.y = get_value<float>( param( "spatial"));
    stddev.x = stddev.y / aspect_ratio() / context.subsample * proxy_scale().x;
    stddev.y = stddev.y / context.subsample * proxy_scale().y;
	
    int filter_width  = stddev.x ? ( (int)( stddev.x * 6 + 1) | 1) : 1;
    int filter_height = stddev.y ? ( (int)( stddev.y * 6 + 1) | 1) : 1;
	
    image::gray_image_t filter_img( filter_width, filter_height);
    image::gray_image_view_t filter( boost::gil::view( filter_img));

	for (int y = 0; y < filter_height; ++y)
	{
		for (int x = 0; x < filter_width; ++x)
		{
			float dx = ( x - filter_width  / 2) / filter_width;
			float dy = ( y - filter_height / 2) / filter_height;
			float d2 = dx * dx + dy * dy;
			filter( x, y)[0] = std::exp( (double) -d2 / 2);
		}
	}
	
	int xoff = ( filter_width  - 1) / 2;
	int yoff = ( filter_height - 1) / 2;

    tbb::parallel_for( tbb::blocked_range<int>( 0, src.height()),
                        apply_chroma_joint_bilateral( boost::gil::const_view( tmp), dst, filter, 
													  xoff, yoff, get_value<float>( param( "color"))),
                        tbb::auto_partitioner());

    image::convert_yuv_to_rgb( dst, dst);
}

// factory
node_t *create_chroma_blur_node() { return new chroma_blur_node_t();}

const node_metaclass_t *chroma_blur_node_t::metaclass() const { return &chroma_blur_node_metaclass();}

const node_metaclass_t& chroma_blur_node_t::chroma_blur_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
	    info.id = "image.builtin.chroma_blur";
		info.major_version = 1;
		info.minor_version = 0;
        info.menu = "Image";
		info.submenu = "Filter";
		info.menu_item = "Chroma Blur";
        info.help = "Blurs the chrominance channels of the input image "
                    "using a joint bilateral filter. It's useful to "
                    "reduce chroma subsampling and compression artifacts in DV video.";

		info.create = &create_chroma_blur_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( chroma_blur_node_t::chroma_blur_node_metaclass());

} // namespace
} // namespace
