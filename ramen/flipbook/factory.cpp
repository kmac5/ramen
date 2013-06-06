// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/flipbook/factory.hpp>

#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>

#include<ramen/assert.hpp>

#include<ramen/app/application.hpp>

#include<ramen/flipbook/internal_flipbook.hpp>

#include<iostream>

namespace ramen
{
namespace flipbook
{

factory_t& factory_t::instance()
{
    static factory_t f;
    return f;
}

factory_t::factory_t()
{
	register_flipbook( "internal", &internal_flipbook_t::create);
}

factory_t::~factory_t()
{
	if( !flipbooks_dir_.empty())
	{
		std::cout << "remove_all( " << flipbooks_dir_.string() << ");\n";
		//boost::filesystem::remove_all( flipbooks_dir_);
	}
}

bool factory_t::register_flipbook( const std::string& id, const create_fun_type& f)
{
	RAMEN_ASSERT( find_flipbook( id) == -1);
	flipbooks_.push_back( std::make_pair( id, f));
	return true;
}

flipbook_t *factory_t::create( int frame_rate, const std::string& display_device,
								  const std::string& display_transform) const
{ 
	return create( app().preferences().default_flipbook(), frame_rate, display_device, display_transform);
}

flipbook_t *factory_t::create( const std::string& id, int frame_rate,
								  const std::string& display_device, const std::string& display_transform) const
{ 
	int index = find_flipbook( id);
	
	if( index == -1)
		index = 0;

	return flipbooks()[index].second( frame_rate, display_device, display_transform);
}

const boost::filesystem::path& factory_t::flipbooks_dir() const
{
	if( flipbooks_dir_.empty())
		flipbooks_dir_ = app().preferences().tmp_dir() / "flipbooks";
	
	return flipbooks_dir_;
}

int factory_t::find_flipbook( const std::string& id) const
{
	for( int i = 0; i < flipbooks().size(); ++i)
	{
		if( flipbooks()[i].first == id)
			return i;
	}
	
	return -1;
}

} // namespace
} // namespace
