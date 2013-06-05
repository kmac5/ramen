// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/layer/key_mix_layer_node.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform3.hpp>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/bool_param.hpp>
#include<ramen/params/float_param.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct key_mix_layer_mode_fun
{
    key_mix_layer_mode_fun( float opacity) : opacity_( opacity) {}

    image::pixel_t operator()( const image::pixel_t& back, const image::pixel_t& front, const image::pixel_t& matte) const
    {
		using namespace boost::gil;
	
		float a = get_color( matte, alpha_t()) * opacity_;
		float a_inv = 1.0f - a;
	
		return image::pixel_t(  ( get_color( front, red_t())   * a)  + ( get_color( back, red_t())   * a_inv),
								( get_color( front, green_t()) * a)  + ( get_color( back, green_t()) * a_inv),
								( get_color( front, blue_t())  * a)  + ( get_color( back, blue_t())  * a_inv),
								( get_color( front, alpha_t()) * a)  + ( get_color( back, alpha_t()) * a_inv));
    }

private:

    float opacity_;
};

} // unnamed

key_mix_layer_node_t::key_mix_layer_node_t() : base_layer_node_t()
{
    set_name( "key_mix");
    add_input_plug( "back", false, ui::palette_t::instance().color( "back plug"), "Back");
    add_input_plug( "front", false, ui::palette_t::instance().color( "front plug"), "Front");
    add_input_plug( "matte", false, ui::palette_t::instance().color( "matte plug"), "Matte");
    add_output_plug();
}

void key_mix_layer_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Mode"));
    p->set_id( "mode");
    p->menu_items() = boost::assign::list_of( "Background")( "Foreground");
    add_param( p);

    std::auto_ptr<float_param_t> q( new float_param_t( "Opacity"));
    q->set_id( "opacity");
    q->set_range( 0, 1);
    q->set_default_value( 1);
    q->set_step( 0.01);
    add_param( q);
}

void key_mix_layer_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::Box2i bbox( input_as<image_node_t>( 0)->bounds());
    bbox.extendBy( ImathExt::intersect( input_as<image_node_t>( 1)->bounds(), input_as<image_node_t>( 2)->bounds()));
    set_bounds( bbox);
}

void key_mix_layer_node_t::do_process( const render::context_t& context)
{
    image_node_t *bg = input_as<image_node_t>( 0);
    image_node_t *fg = input_as<image_node_t>( 1);
    image_node_t *mt = input_as<image_node_t>( 2);

    Imath::Box2i bg_area = ImathExt::intersect( bg->defined(), defined());

    if( !bg_area.isEmpty())
    {
        render_input( 0, context);
		boost::gil::copy_pixels( bg->const_subimage_view( bg_area), subimage_view( bg_area));
        release_input_image( 0);
    }

    Imath::Box2i comp_area( ImathExt::intersect( ImathExt::intersect( fg->defined(), mt->defined()), defined()));

    if( !comp_area.isEmpty())
    {
        render_input( 1, context);

        // image buffer contains a shared ptr to
        // the actual pixels. Saving it to a local variable,
        // is equivalent to locking the pixels.
        // This could be improved, as I think it's not really clear.
        // There is also a lock method in buffer_t that it's
        // currently unused...
        image::buffer_t fg_img = fg->image();

        render_input( 2, context);
        image::buffer_t alpha_img = mt->image();

		boost::gil::tbb_transform3_pixels( const_subimage_view( comp_area),
											fg->const_subimage_view( comp_area),
											mt->const_subimage_view( comp_area),
											subimage_view( comp_area),
											key_mix_layer_mode_fun( get_value<float>( param( "opacity"))));
        release_input_image( 1);
        release_input_image( 2);
    }
}

// factory

node_t *create_key_mix_layer_node() { return new key_mix_layer_node_t();}

const node_metaclass_t *key_mix_layer_node_t::metaclass() const { return &key_mix_layer_node_metaclass();}

const node_metaclass_t& key_mix_layer_node_t::key_mix_layer_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.keymix";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Layer";
        info.menu_item = "KeyMix";
        info.create = &create_key_mix_layer_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( key_mix_layer_node_t::key_mix_layer_node_metaclass());

} // namespace
} // namespace
