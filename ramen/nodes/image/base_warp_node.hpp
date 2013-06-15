// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_BASE_WARP_NODE_HPP
#define RAMEN_BASE_WARP_NODE_HPP

#include<ramen/nodes/image_node.hpp>

#include<ramen/image/mipmap_sampler.hpp>
#include<ramen/image/sse2/samplers.hpp>

namespace ramen
{
namespace image
{

class RAMEN_API base_warp_node_t : public image_node_t
{
public:

    enum filter_type
    {
        filter_point = 0,
        filter_bilinear,
        filter_catrom,
		filter_mipmap
    };

	typedef image::mipmap_sampler_t<image::sse2::bilinear_sampler_t,
									image::sse2::catrom_sampler_t> mipmap_type;

protected:

    base_warp_node_t();
    base_warp_node_t( const base_warp_node_t& other);
    void operator=( const base_warp_node_t&);

	virtual void do_calc_hash_str( const render::context_t& context);

	// filters
	void create_default_filter_param();
    virtual filter_type get_filter_type() const;

	Imath::Box2i expand_interest( const Imath::Box2i& box, const render::context_t& context) const;

	// mipmaps

	int max_mipmap_levels() const;
	void make_mipmap( image_node_t *src, mipmap_type& mipmap, std::vector<image::buffer_t>& buffers) const;

	bool notify_pending_;

private:

	// interaction
	virtual void do_begin_interaction();
	virtual void do_end_interaction();
};

} // image
} // ramen

#endif
