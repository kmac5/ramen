// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MEMORY_LRU_CACHE_INTERFACE_HPP
#define	RAMEN_MEMORY_LRU_CACHE_INTERFACE_HPP

#include<boost/date_time/posix_time/posix_time.hpp>

namespace ramen
{
namespace memory
{

class lru_cache_interface
{
public:

	virtual ~lru_cache_interface() {}

    virtual bool empty() const = 0;
    virtual void clear() = 0;
    virtual void erase_lru() = 0;

	virtual boost::posix_time::ptime lru_time() const = 0;
};

} // namespace
} // namespace

#endif
