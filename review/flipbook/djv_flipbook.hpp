// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FLIPBOOK_DJV_FLIPBOOK_HPP
#define RAMEN_FLIPBOOK_DJV_FLIPBOOK_HPP

#include<ramen/flipbook/external_flipbook.hpp>

namespace ramen
{
namespace flipbook
{

class djv_flipbook_t : public external_flipbook_t
{
	Q_OBJECT

    djv_flipbook_t( int frame_rate, const std::string& display_device, const std::string& display_transform);
	
public:
	
	static djv_flipbook_t *create( int frame_rate, const std::string& display_device, const std::string& display_transform);
	
	virtual std::string program() const;
	virtual void arguments( std::vector<std::string>& args) const;
};

} // namespace
} // namespace

#endif
