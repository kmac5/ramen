// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OCIO_CONTEXT_HPP
#define	RAMEN_OCIO_CONTEXT_HPP

#include<vector>
#include<string>
#include<utility>

#include<ramen/assert.hpp>

namespace ramen
{
namespace ocio
{

class context_t
{
public:

	context_t();

	std::size_t size() const;

	const std::pair<std::string, std::string>& operator[]( int i) const
	{
		RAMEN_ASSERT( i >= 0 && i < size());
		return pairs_[i];
	}

	bool set_key_and_value( int index, const std::string& key, const std::string& value);

private:

	std::vector<std::pair<std::string, std::string> > pairs_;
};

} // ocio
} // ramen

#endif
