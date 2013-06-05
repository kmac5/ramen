// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/color/color_match_node.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>

namespace ramen
{
namespace image
{
namespace
{

// this colorspace is also called lab, but it's not the same as the commonly used lab
struct convert_rgb_to_lab_fun
{
    image::pixel_t operator()( const image::pixel_t& p) const
    {
        using namespace boost::gil;

        float red   = get_color( p, red_t());
        float green = get_color( p, green_t());
        float blue  = get_color( p, blue_t());
        float alpha = get_color( p, alpha_t());

        float l = ( 0.3811f * red) + ( 0.5783f * green) + ( 0.0402f * blue);
        float m = ( 0.1967f * red) + ( 0.7244f * green) + ( 0.0782f * blue);
        float s = ( 0.0241f * red) + ( 0.1288f * green) + ( 0.8444f * blue);

        l = std::pow( (double) l, 0.1);
        m = std::pow( (double) m, 0.1);
        s = std::pow( (double) s, 0.1);

        return image::pixel_t( ( 0.57735f * l) + ( 0.57735f * m) + ( 0.57735f * s),
                               ( 0.40825f * l) + ( 0.40825f * m) - ( 0.81650f * s),
                               ( 0.70711f * l) - ( 0.70711f * m), alpha);
    }
};

struct convert_lab_to_rgb_fun
{
    image::pixel_t operator()( const image::pixel_t& p) const
    {
        using namespace boost::gil;

        float ll    = get_color( p, red_t());
        float la    = get_color( p, green_t());
        float lb    = get_color( p, blue_t());
        float alpha = get_color( p, alpha_t());

        float l = ( 0.57735f * ll) + ( 0.40825f * la) + ( 0.70711f * lb);
        float m = ( 0.57735f * ll) + ( 0.40825f * la) - ( 0.70711f * lb);
        float s = ( 0.57735f * ll) - ( 0.81650f * la);

        l = std::pow( (double) l, 10.0);
        m = std::pow( (double) m, 10.0);
        s = std::pow( (double) s, 10.0);

        return image::pixel_t(  ( 4.4686f * l) - ( 3.5886f * m) + ( 0.1196f * s),
                               -( 1.2197f * l) + ( 2.3830f * m) - ( 0.1626f * s),
                                ( 0.0585f * l) - ( 0.2610f * m) + ( 1.2056f * s), alpha);
    }
};

void cm_calc_mean_and_stddev( const image::const_image_view_t& img, Imath::V3f& mean, Imath::V3f& std)
{
    using namespace boost::gil;

    Imath::V3d m( 0, 0, 0);

    for( int y=0;y<img.height();++y)
    {
		image::const_image_view_t::x_iterator src_it = img.row_begin( y);

        for( int x=0;x<img.width();++x)
		{
		    m.x += get_color( *src_it, red_t());
		    m.y += get_color( *src_it, green_t());
		    m.z += get_color( *src_it, blue_t());
            ++src_it;
		}
    }

    m /= img.width() * img.height();
    mean = Imath::V3f( m.x, m.y, m.z);

    Imath::V3d var( 0, 0, 0);

    for( int y=0;y<img.height();++y)
    {
		image::const_image_view_t::x_iterator src_it = img.row_begin( y);

        for( int x=0;x<img.width();++x)
		{
            float r = get_color( *src_it, red_t());
            float g = get_color( *src_it, green_t());
            float b = get_color( *src_it, blue_t());

		    var.x += ( r - m.x) * ( r - m.x);
		    var.y += ( g - m.y) * ( g - m.y);
		    var.z += ( b - m.z) * ( b - m.z);
            ++src_it;
		}
    }

    var /= img.width() * img.height();
    std = Imath::V3f( std::sqrt( (double) var.x), std::sqrt( (double) var.y), std::sqrt( (double) var.z));
}

struct transfer_color_fun
{
    transfer_color_fun( const Imath::V3f& src_mean, const Imath::V3f& src_dev, const Imath::V3f& dst_mean, const Imath::V3f& dst_dev)
    {
        src_mean_   = src_mean;
        dst_mean_   = dst_mean;
        gain_.x = ( src_dev.x != 0) ? dst_dev.x / src_dev.x : 0;
        gain_.y = ( src_dev.y != 0) ? dst_dev.y / src_dev.y : 0;
        gain_.z = ( src_dev.z != 0) ? dst_dev.z / src_dev.z : 0;
    }

    image::pixel_t operator()( const image::pixel_t& p) const
    {
        using namespace boost::gil;

        float r = get_color( p, red_t());
        float g = get_color( p, green_t());
        float b = get_color( p, blue_t());
        float a = get_color( p, alpha_t());

        return image::pixel_t( (( r - src_mean_.x) * gain_.x) + dst_mean_.x,
                               (( g - src_mean_.y) * gain_.y) + dst_mean_.y,
                               (( b - src_mean_.z) * gain_.z) + dst_mean_.z, a);
    }

private:

    Imath::V3f src_mean_, dst_mean_, gain_;
};

} // unnamed

color_match_node_t::color_match_node_t()
{
    set_name( "color_match");
    add_input_plug( "front", false, ui::palette_t::instance().color( "front plug"), "Front");
    add_input_plug( "target", false, ui::palette_t::instance().color( "front plug"), "Target Color");
    add_input_plug( "source", true, ui::palette_t::instance().color( "back plug") , "Source Color");
    add_output_plug();
}

void color_match_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    // we need all the image of the second input
    input_as<image_node_t>( 1)->add_interest( input_as<image_node_t>( 1)->format());

    if( input( 2))
    {
        input_as<image_node_t>( 0)->add_interest( interest());
        input_as<image_node_t>( 2)->add_interest( input_as<image_node_t>( 2)->format());
    }
    else
        input_as<image_node_t>( 0)->add_interest( input_as<image_node_t>( 0)->format());
}

void color_match_node_t::do_calc_defined( const render::context_t& context)
{
    if( input( 2))
    {
        Imath::Box2i def( input_as<image_node_t>( 0)->defined());
        set_defined( ImathExt::intersect( def, interest()));
    }
    else
        set_defined( format());
}

void color_match_node_t::do_process( const render::context_t& context)
{
    Imath::V3f src_mean( 0, 0, 0), src_dev( 0, 0, 0);
    Imath::V3f dst_mean( 0, 0, 0), dst_dev( 0, 0, 0);

    Imath::Box2i area = ImathExt::intersect( defined(), input_as<image_node_t>( 0)->defined());

    if( area.isEmpty())
        return;

    // process input 1
    {
        image::const_image_view_t src_view( input_as<image_node_t>( 1)->const_subimage_view( input_as<image_node_t>( 1)->format()));
        image::image_t tmp( src_view.width(), src_view.height());
        boost::gil::tbb_transform_pixels( src_view, boost::gil::view( tmp), convert_rgb_to_lab_fun());
        cm_calc_mean_and_stddev( boost::gil::const_view( tmp), dst_mean, dst_dev);
    }

    if( input( 2))
    {
        image::const_image_view_t src_view( input_as<image_node_t>( 2)->const_subimage_view( input_as<image_node_t>( 2)->format()));
        image::image_t tmp( src_view.width(), src_view.height());
        boost::gil::tbb_transform_pixels( src_view, boost::gil::view( tmp), convert_rgb_to_lab_fun());
        cm_calc_mean_and_stddev( boost::gil::const_view( tmp), src_mean, src_dev);
    }

    boost::gil::tbb_transform_pixels( input_as<image_node_t>( 0)->const_subimage_view( area), subimage_view( area), convert_rgb_to_lab_fun());

    if( !input( 2))
        cm_calc_mean_and_stddev( const_subimage_view( area), src_mean, src_dev);

    boost::gil::tbb_transform_pixels( const_subimage_view( area), subimage_view( area), transfer_color_fun( src_mean, src_dev, dst_mean, dst_dev));
    boost::gil::tbb_transform_pixels( const_subimage_view( area), subimage_view( area), convert_lab_to_rgb_fun());
}

// factory
node_t *create_color_match_node() { return new color_match_node_t();}

const node_metaclass_t *color_match_node_t::metaclass() const { return &color_match_node_metaclass();}

const node_metaclass_t& color_match_node_t::color_match_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.color_match";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Color";
        info.menu_item = "Color Match";
        info.create = &create_color_match_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( color_match_node_t::color_match_node_metaclass());

} // namespace
} // namespace
