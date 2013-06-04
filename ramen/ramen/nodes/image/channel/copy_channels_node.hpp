// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_COPY_CHANNELS_NODE_HPP
#define	RAMEN_IMAGE_COPY_CHANNELS_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{
namespace image
{

class copy_channels_node_t : public image_node_t
{
public:

    static const node_metaclass_t& copy_channels_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    copy_channels_node_t();
    
    virtual bool use_cache( const render::context_t& context) const { return false;}

protected:

    copy_channels_node_t( const copy_channels_node_t& other) : image_node_t( other) {}
    void operator=( const copy_channels_node_t&);

private:

    virtual node_t *do_clone() const { return new copy_channels_node_t( *this);}

    virtual void do_create_params();

    virtual void do_calc_bounds( const render::context_t& context);

    virtual void do_process( const render::context_t& context);

    void copy_channel( const image::const_image_view_t& src, int src_ch, const image::image_view_t& dst , int dst_ch);
};

} // namespace
} // namespace

#endif
