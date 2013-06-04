// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_REF_COUNTED_HPP
#define RAMEN_REF_COUNTED_HPP

#include<boost/noncopyable.hpp>
#include<tbb/atomic.h>

#include<ramen/assert.hpp>

namespace ramen
{

/**
\ingroup utils
\brief base class for reference counted objects
*/
class ref_counted_t : private boost::noncopyable
{
public:

    typedef size_t ref_count_type;

    ref_counted_t();

    inline void add_ref() const { num_refs_++; }

    inline void remove_ref() const
    {
        RAMEN_ASSERT( num_refs_ > 0);

        if( --num_refs_== 0)
            delete this;
    }

    inline ref_count_type ref_count() const { return num_refs_; }

protected:

    virtual ~ref_counted_t();

private:

    mutable tbb::atomic<ref_count_type> num_refs_;
};

inline void intrusive_ptr_add_ref( const ref_counted_t *r) { r->add_ref();}
inline void intrusive_ptr_release( const ref_counted_t *r) { r->remove_ref();}

} // namespace

#endif
