// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/color/channel_mixer_node.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb.hpp>

#include<ramen/params/float_param.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct channel_mixer_fun
{
    channel_mixer_fun( float mr, float mg, float mb)
    {
		mr_ = mr / (mr + mg + mb);
		mg_ = mg / (mr + mg + mb);
		mb_ = mb / (mr + mg + mb);
    }

    image::pixel_t operator()( const image::pixel_t& src) const
    {
		float val = (src[0] * mr_) + (src[1] * mg_) + (src[2] * mb_);
		return image::pixel_t( val, val, val, src[3]);
    }

private:

    float mr_, mg_, mb_;
};

} // unnamed

channel_mixer_node_t::channel_mixer_node_t() : pointop_node_t() { set_name("ch mix");}

void channel_mixer_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Red"));
    p->set_id( "red");
    p->set_default_value( 0.212671f);
    p->set_range( 0, 1);
    p->set_step( 0.05);
    add_param( p);

    p.reset( new float_param_t( "Green"));
    p->set_id( "green");
    p->set_default_value( 0.715160f);
    p->set_range( 0, 1);
    p->set_step( 0.05);
    add_param( p);

    p.reset( new float_param_t( "Blue"));
    p->set_id( "blue");
    p->set_default_value( 0.072169f);
    p->set_range( 0, 1);
    p->set_step( 0.05);
    add_param( p);
}

void channel_mixer_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context)
{
    boost::gil::tbb_transform_pixels( src, dst, channel_mixer_fun( get_value<float>( param( "red")),
                                                                     get_value<float>( param( "green")),
                                                                     get_value<float>( param( "blue"))));
}

// factory
node_t *create_channel_mixer_node() { return new channel_mixer_node_t();}

const node_metaclass_t *channel_mixer_node_t::metaclass() const { return &channel_mixer_node_metaclass();}

const node_metaclass_t& channel_mixer_node_t::channel_mixer_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.channel_mixer";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Color";
        info.menu_item = "Channel Mixer";
		info.help = "Converts the input image to gray scale";
        info.create = &create_channel_mixer_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( channel_mixer_node_t::channel_mixer_node_metaclass());

} // namespace
} // namespace
