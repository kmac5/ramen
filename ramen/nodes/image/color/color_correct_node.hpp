// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_COLOR_CORRECT_NODE_HPP
#define	RAMEN_IMAGE_COLOR_CORRECT_NODE_HPP

#include<ramen/nodes/image/pointop_node.hpp>

#include<ramen/params/composite_param.hpp>

namespace ramen
{
namespace image
{

class color_correct_node_t : public pointop_node_t
{
public:

    static const node_metaclass_t& color_correct_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    color_correct_node_t();

protected:

    color_correct_node_t( const color_correct_node_t& other) : pointop_node_t( other) {}
    void operator=( const color_correct_node_t&);

protected:

    void create_params_inside_param( composite_param_t *g, const std::string& id_prefix);

private:

    node_t *do_clone() const { return new color_correct_node_t( *this);}

    virtual void do_create_params();

    std::auto_ptr<composite_param_t> create_param_tab( const std::string& name, const std::string& id,
							const std::string& id_prefix);

    virtual void do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context);
};

} // namespace
} // namespace

#endif
