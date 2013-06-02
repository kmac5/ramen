// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_COMPOSITION_VIEW_LAYOUT_HPP
#define	RAMEN_UI_COMPOSITION_VIEW_LAYOUT_HPP

#include<boost/optional.hpp>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathRandom.h>

#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{
namespace ui
{

class composition_view_layout_t
{
public:

    composition_view_layout_t();

	void set_world( const Imath::Box2f& w);

    void set_interest_point( const Imath::V2f& p);

    void place_node( node_t *n) const;
    void place_node_near_node( node_t *n, node_t *other) const;

private:

	Imath::Box2f get_node_bbox( node_t *n) const;
	bool box_intersects_any_node( const Imath::Box2f& box) const;
	
	void do_place_node( node_t *n, const Imath::V2f& p) const;
	
	Imath::Box2f world_;
	boost::optional<Imath::V2f> interest_point_;

	mutable Imath::Rand48 rng_;
};

} // namespace
} // namespace

#endif
