// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/matte/alpha_ops_node.hpp>

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

struct grad_magnitude_fun
{
    grad_magnitude_fun( const image::const_gray_image_view_t& src, const image::gray_image_view_t& dst, float thereshold) : src_( src), dst_( dst)
    {
        thereshold_ = thereshold;
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
        double sqrt2 = std::sqrt( 2.0);

        for( int j = r.begin(); j < r.end(); ++j)
        {
            image::const_gray_image_view_t::x_iterator top_it( src_.row_begin( std::max( j - 1, 0)));
            image::const_gray_image_view_t::x_iterator center_it( src_.row_begin( j));
            image::const_gray_image_view_t::x_iterator bottom_it( src_.row_begin( std::min( j + 1, (int) src_.height() - 1)));

            image::gray_image_view_t::x_iterator dst_it( dst_.row_begin( j));

            // loop
            for( int i = 0; i < src_.width(); ++i)
            {
                int x0 = adobe::clamp( i - 1, 0, (int) src_.width()-1);
                int x1 = adobe::clamp( i + 1, 0, (int) src_.width()-1);

                float gx = ( center_it[x1][0] - center_it[x0][0]);
                float gy = ( top_it[i][0] - bottom_it[i][0]);
                float gmag = std::sqrt( (double) ( gx * gx) + ( gy * gy)) / sqrt2;
                
                if( gmag < thereshold_)
                    gmag = 0;

                *dst_it++ = gmag;
            }
        }
    }

private:

    const image::const_gray_image_view_t& src_;
    const image::gray_image_view_t& dst_;
    float thereshold_;
};

struct remap_alpha_fun
{
    remap_alpha_fun( float lo, float hi) : alpha_low_( lo), alpha_high_( hi) {}

    image::gray_pixel_t operator()( const image::gray_pixel_t& p) const
    {
	float a = p[0];

	if( a <= alpha_low_)
	    a = 0;
	else
	{
	    if( a >= alpha_high_ || alpha_high_ == alpha_low_)
		a = 1;
	    else
		a = ( a - alpha_low_) / ( alpha_high_ - alpha_low_);
	}

	return image::gray_pixel_t( a);
    }

private:

    float alpha_low_, alpha_high_;
};

} // unnamed

alpha_ops_node_t::alpha_ops_node_t() : areaop_node_t() { set_name( "alpha_ops");}

void alpha_ops_node_t::do_create_params()
{
    std::auto_ptr<bool_param_t> b( new bool_param_t( "Edge detect"));
    b->set_id( "edetect");
    b->set_default_value( false);
    add_param( b);
    
    std::auto_ptr<float_param_t> p( new float_param_t( "Edge Theresh"));
    p->set_id( "etheresh");
    p->set_default_value( 0);
    p->set_range( 0, 1);
    p->set_step( 0.05);
    add_param( p);

    p.reset( new float_param_t( "Grow/Shrink"));
    p->set_id( "shrink");
    p->set_default_value( 0);
    p->set_range( -100, 100);
    add_param( p);

    p.reset( new float_param_t( "Blur"));
    p->set_id( "blur");
    p->set_default_value( 0);
    p->set_range( 0, 200);
    add_param( p);

    p.reset( new float_param_t( "Alpha Low"));
    p->set_id( "alpha_low");
    p->set_default_value( 0);
    p->set_range( 0, 1);
    p->set_step( 0.05);
    add_param( p);

    p.reset( new float_param_t( "Alpha High"));
    p->set_id( "alpha_high");
    p->set_default_value( 1);
    p->set_range( 0, 1);
    p->set_step( 0.05);
    add_param( p);
}

void alpha_ops_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    if( get_value<bool>( param( "edetect")))
	{
        hradius = 1;
		vradius = 1;
	}
    else
	{
        hradius = 0;
		vradius = 0;
	}

    float shrinkx = get_value<float>( param( "shrink")) * proxy_scale().x / aspect_ratio();
    float shrinky = get_value<float>( param( "shrink")) * proxy_scale().y;

    if( shrinkx > 0 || shrinky > 0)
	{
        hradius += std::ceil( ( double) shrinkx);
		vradius += std::ceil( ( double) shrinky);
	}

    float blurx = get_value<float>( param( "blur")) * proxy_scale().x / aspect_ratio() / 2.0f;
    float blury = get_value<float>( param( "blur")) * proxy_scale().y / 2.0f;

    if( blurx != 0 || blury != 0)
	{
        hradius += std::ceil( ( double) blurx + 1) * 2;
        vradius += std::ceil( ( double) blury + 1) * 2;
	}	
}

bool alpha_ops_node_t::expand_defined() const { return true;}

void alpha_ops_node_t::do_process( const render::context_t& context)
{
    using namespace boost::gil;

    Imath::Box2i area( ImathExt::intersect( input_as<image_node_t>()->defined(), defined()));

    if( area.isEmpty())
		return;
	
    // copy the rgb channels
    copy_pixels( input_as<image_node_t>()->const_subimage_view( area), subimage_view( area));

	int width = defined().size().x + 1;
	int height = defined().size().y + 1;
	
    image::buffer_t img0( width, height, 1);
    fill_pixels( img0.gray_view(), image::gray_pixel_t( 0));

    image::buffer_t img1( width, height, 1);
    fill_pixels( img1.gray_view(), image::gray_pixel_t( 0));

    image::buffer_t tmp( height, width, 1);
	
    // copy the alpha channel to img0
    image::gray_image_view_t src( img0.gray_view());
    image::gray_image_view_t dst( img0.gray_view());

    copy_pixels( nth_channel_view( input_as<image_node_t>()->const_subimage_view( area), 3),
                boost::gil::subimage_view( src, area.min.x - defined().min.x, 
										   area.min.y - defined().min.y, area.size().x + 1, area.size().y + 1));

    bool detect_edges = get_value<bool>( param( "edetect"));

    if( detect_edges)
    {
        src = img0.gray_view();
        dst = img1.gray_view();

        float theresh = get_value<float>( param( "etheresh"));

        tbb::parallel_for( tbb::blocked_range<int>( 0, dst.height()), grad_magnitude_fun( src, dst, theresh), tbb::auto_partitioner());
    }

	float shrinkx = get_value<float>( param( "shrink")) / context.subsample * proxy_scale().x / aspect_ratio();
    float shrinky = get_value<float>( param( "shrink")) / context.subsample * proxy_scale().y;

    if( shrinkx != 0 || shrinky != 0)
    {
        if( detect_edges)
        {
            src = img1.gray_view();
            dst = img0.gray_view();
        }
        else
        {
            src = img0.gray_view();
            dst = img1.gray_view();
        }

        image::dilate( src, tmp.gray_view(), dst, shrinkx, shrinky);
    }

    float blurx = get_value<float>( param( "blur")) / ( 2 * context.subsample) * proxy_scale().x / aspect_ratio();
    float blury = get_value<float>( param( "blur")) / ( 2 * context.subsample) * proxy_scale().y;

    if( blurx != 0 || blury != 0)
    {
        if( detect_edges)
        {
            src = img1.gray_view();
            dst = img0.gray_view();
        }
        else
        {
            src = img0.gray_view();
            dst = img1.gray_view();
        }

        if( shrinkx != 0 || shrinky != 0)
            std::swap( src, dst);

        image::box_blur_gray( src, tmp.gray_view(), dst, blurx, blury, 2);
    }

    // alpha levels
    boost::gil::tbb_transform_pixels( dst, dst, remap_alpha_fun( get_value<float>( param( "alpha_low")), get_value<float>( param( "alpha_high"))));

    // final
    copy_pixels( dst, nth_channel_view( image_view(), 3));
}

// factory
node_t *create_alpha_ops_node() { return new alpha_ops_node_t();}

const node_metaclass_t *alpha_ops_node_t::metaclass() const { return &alpha_ops_node_metaclass();}

const node_metaclass_t& alpha_ops_node_t::alpha_ops_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.alpha_ops";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Matte";
        info.menu_item = "Alpha Ops";
        info.create = &create_alpha_ops_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( alpha_ops_node_t::alpha_ops_node_metaclass());

} // namespace
} // namespace
