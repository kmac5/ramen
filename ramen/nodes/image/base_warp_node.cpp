// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/base_warp_node.hpp>

#include<ramen/app/application.hpp>

#include<ramen/params/popup_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{
namespace image
{

base_warp_node_t::base_warp_node_t() : image_node_t(), notify_pending_( false)
{
    add_input_plug( "Front", false, ui::palette_t::instance().color( "front plug"), "Front");
    add_output_plug();
}

base_warp_node_t::base_warp_node_t( const base_warp_node_t& other) : image_node_t( other), notify_pending_( false) {}

void base_warp_node_t::create_default_filter_param()
{
    std::auto_ptr<popup_param_t> r( new popup_param_t( "Filter"));
    r->set_id( "filter");
    r->menu_items() = boost::assign::list_of( "Point")( "Bilinear")( "Catrom")( "Mipmap");
    r->set_default_value( 2);
    r->set_include_in_hash( false);
    add_param( r);
}

base_warp_node_t::filter_type base_warp_node_t::get_filter_type() const
{
    if( interacting())
        return filter_bilinear;

    try
    {
        return ( filter_type) get_value<int>( param( "filter"));
    }
    catch( std::exception& e)
    {
        return filter_bilinear;
    }
}

void base_warp_node_t::do_begin_interaction()
{
    RAMEN_ASSERT( notify_pending_ == false);

    switch( get_filter_type())
    {
        case filter_point:
        case filter_bilinear:
            notify_pending_ = false;
        break;

        default:
            notify_pending_ = true;
        break;
    }
}

void base_warp_node_t::do_end_interaction()
{
    if( notify_pending_)
        set_notify_dirty( true);

    notify_pending_ = false;
}

void base_warp_node_t::do_calc_hash_str( const render::context_t& context)
{
    hash_generator() << ( int) get_filter_type();
    param_set().add_to_hash( hash_generator());
}

Imath::Box2i base_warp_node_t::expand_interest( const Imath::Box2i& box, const render::context_t& context) const
{
    Imath::Box2i roi( box);

    filter_type ftype = get_filter_type();

    switch( ftype)
    {
        case filter_point:
        break;

        case filter_bilinear:
            ++roi.max.x;
            ++roi.max.y;
        break;

        case filter_catrom:
            --roi.min.x;
            --roi.min.y;
            roi.max.x += 2;
            roi.max.y += 2;
        break;

        default: // mipmap like filter
            RAMEN_ASSERT( ftype >= filter_mipmap);

            int pad = std::max( 2 * ( 1 << max_mipmap_levels()), 2);
            roi.min.x -= pad;
            roi.min.y -= pad;
            roi.max.x += pad;
            roi.max.y += pad;
        break;
    }

    return roi;
}

int base_warp_node_t::max_mipmap_levels() const { return 4;}

void base_warp_node_t::make_mipmap( image_node_t *src, mipmap_type& mipmap, std::vector<image::buffer_t>& buffers) const
{
    Imath::Box2i area = src->defined();
    buffers.push_back( src->image());
    mipmap.set_top_level( image::sse2::catrom_sampler_t( area, src->const_subimage_view( area)));

    std::string src_hash_str( src->hash_str());

    for( int i = 1; i < max_mipmap_levels(); ++i)
    {
        area = Imath::scale( area, 0.5f);

        if( area.size().x < mipmap.min_size() || area.size().y < mipmap.min_size())
            return;

        hash::generator_t gen;
        gen << src_hash_str << "mipmap" << i;

        /*
        boost::optional<image::buffer_t> cached( app().memory_manager().find_in_cache( gen.digest(), area));

        if( cached)
        {
            buffers.push_back( cached.get());
            mipmap.add_level( image::sse2::bilinear_sampler_t( area, cached.get().const_rgba_subimage_view( area)));
        }
        else
        */
        {
            image::buffer_t buf( area, 4);
            image::resize_half( buffers.back().const_rgba_view(), buffers.back().bounds(), buf.rgba_view(), area);
            buffers.push_back( buf);
            //app().memory_manager().insert_in_cache( src, gen.digest(), buf);
        }

        mipmap.add_level( image::sse2::bilinear_sampler_t( buffers.back().bounds(), buffers.back().const_rgba_view()));
    }
}

} // namespace
} // namespace
