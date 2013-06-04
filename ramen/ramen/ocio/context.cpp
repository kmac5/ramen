// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/ocio/context.hpp>

namespace ramen
{
namespace ocio
{

context_t::context_t()
{
	pairs_ = std::vector<std::pair<std::string, std::string> >( size(), std::make_pair( std::string(), std::string()));
}

std::size_t context_t::size() const { return 4;}

bool context_t::set_key_and_value( int index, const std::string& key, const std::string& value)
{
	RAMEN_ASSERT( index >= 0 && index < size());

	if( pairs_[index].first != key || pairs_[index].second != value)
	{
		pairs_[index] = std::make_pair( key, value);
		return true;
	}

	return false;
}

} // ocio
} // ramen
