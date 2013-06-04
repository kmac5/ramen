// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_CLIPBOARD_HPP
#define	RAMEN_ANIM_CLIPBOARD_HPP

#include<vector>
#include<string>
#include<memory>

#include<boost/noncopyable.hpp>
#include<boost/shared_ptr.hpp>

#include<ramen/anim/any_curve.hpp>

#include<ramen/util/edit_distance.hpp>

namespace ramen
{
namespace anim
{
	
class clipboard_t : boost::noncopyable
{
public:

	typedef std::pair<std::string, boost::shared_ptr<any_curve_t> > named_curve_type;

    static clipboard_t& instance();

	bool empty() const;
	void clear();
	
	void begin_copy();
	
	void copy_curve( const std::string& name, const anim::any_curve_ptr_t& c);
	void copy_keys( const std::string& name, const anim::any_curve_ptr_t& c);

	void end_copy();
	
	bool can_paste( const std::string& name, const anim::any_curve_ptr_t& c);
	void paste( const std::string& name, anim::any_curve_ptr_t& c, float frame);
	
	// param spinboxes
	bool can_paste();
	void copy( const float_curve_t& c);
	void paste( float_curve_t& dst);

private:

    clipboard_t();
	~clipboard_t();

	int find_compatible_curve( const std::string& name, const anim::any_curve_ptr_t& c);

	std::vector<named_curve_type> contents_;
	bool copy_curves_mode_;
	bool copying_;
	
    util::edit_distance_t distance_fun_;
};

} // namespace
} // namespace

#endif
