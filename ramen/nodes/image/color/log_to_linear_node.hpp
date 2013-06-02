// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_LOG2LIN_NODE_HPP
#define RAMEN_IMAGE_LOG2LIN_NODE_HPP

#include<ramen/nodes/image/pointop_node.hpp>

#include<IECore/LinearToCineonDataConversion.h>

namespace ramen
{
namespace image
{

class log_to_linear_node_t : public pointop_node_t
{
public:

    static const node_metaclass_t& log_to_linear_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    log_to_linear_node_t();

protected:

    log_to_linear_node_t(const log_to_linear_node_t& other) : pointop_node_t(other) { }
    void operator=( const log_to_linear_node_t&);

private:

    node_t *do_clone() const { return new log_to_linear_node_t(*this);}

    virtual void do_create_params();

    virtual void do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context);

    void init_lut( boost::uint16_t *lut, const IECore::LinearToCineonDataConversion<float,boost::uint16_t>& conv) const;
};

} // namespace
} // namespace

#endif
