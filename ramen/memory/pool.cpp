// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/memory/pool.hpp>

#include<limits>
#include<algorithm>

#include<boost/scoped_array.hpp>

#include<boost/intrusive/list.hpp>
namespace bi = boost::intrusive;

#include<ramen/assert.hpp>

namespace ramen
{
namespace memory
{
	
struct pool_t::implementation_t
{
    const static std::size_t size_tolerance = 128 * 1024; // 128 Kb?

    struct mem_block_t : public bi::list_base_hook<bi::link_mode<bi::safe_link> >
    {
        mem_block_t( std::size_t s) : size( s), free( true) {}

        mem_block_t *split( std::size_t s)
        {
            // preconditions
            RAMEN_ASSERT( free && "mem_block_t::split : trying to split a non free block");
            RAMEN_ASSERT( ( size - s > sizeof( mem_block_t)) && "mem_block_t::split : block can't be split");

            std::size_t old_size = size;

            unsigned char *p = reinterpret_cast<unsigned char *>( this);
            p += ( sizeof( mem_block_t) + s);

            mem_block_t *new_block = new ( p) mem_block_t( size - s - sizeof( mem_block_t));
            size = s;

            // postconditions
            RAMEN_ASSERT( old_size == ( size + new_block->size + sizeof( mem_block_t)) && "mem_block_t::split : block sizes don't match");
            RAMEN_ASSERT( is_contiguous( *new_block) && "mem_block_t::split : non contiguous resulting blocks");

            return new_block;
        }

        bool merge( mem_block_t& other)
        {
            unsigned char *p = reinterpret_cast<unsigned char *>( this);
            unsigned char *q = reinterpret_cast<unsigned char *>( &other);

            RAMEN_ASSERT( p < q && "merge: list posibly not sorted");

            if( !is_contiguous( other))
                return false;

            size += ( sizeof( mem_block_t) + other.size);
            return true;
        }

        bool is_contiguous( mem_block_t& other)
        {
            unsigned char *p0 = reinterpret_cast<unsigned char *>( this);
            unsigned char *p1 = p0 + sizeof( mem_block_t) + size;
            unsigned char *q = reinterpret_cast<unsigned char *>( &other);
            return p1 == q;
        }

        bool overlaps( mem_block_t& other)
        {
            if( this == &other)
                return false;

            unsigned char *p0 = reinterpret_cast<unsigned char*>( this);
            unsigned char *p1 = p0 + sizeof( mem_block_t) + size;

            unsigned char *q0 = reinterpret_cast<unsigned char*>( &other);
            unsigned char *q1 = q0 + sizeof( mem_block_t) + other.size;

            if( q0 < p0 && q1 <= p0)
                return false;

            if( q0 >= p1 && q1 > p1)
                return false;

            return true;
        }

        std::size_t size;
        bool free;

    private:

        // non-copyable
        mem_block_t( const mem_block_t&);
        mem_block_t& operator=( const mem_block_t&);
    };

    implementation_t( std::size_t size_in_bytes) : pool_size( size_in_bytes)
    {
         pool.reset( new unsigned char[ pool_size]);

         // create our first block and add it to the list
         mem_block_t *block = new ( pool.get()) mem_block_t( size_in_bytes - sizeof( mem_block_t));
         free_list_.push_back( *block);
    }

    // iterators
    typedef bi::list<mem_block_t>::iterator iterator;

    iterator begin()    { return free_list_.begin();}
    iterator end()      { return free_list_.end();}

    // public private interface
    unsigned char *allocate( std::size_t size)
    {
        iterator it( find_best_block( size));

        if( it == end())
            return 0;

        RAMEN_ASSERT( it->size >= size && "find best block: block smaller than size requested\n");

        mem_block_t *m = 0;

        if( it->size - size > size_tolerance) // need to split the block
            split_block( it, size);

        m = &*it;
        free_list_.erase( it);
        m->free = false;

        // post conditions
        RAMEN_ASSERT( m->size >= size          && "allocate: returning a block smaller than size");
        RAMEN_ASSERT( list_is_sorted()         && "allocate: list not sorted");
        RAMEN_ASSERT( !overlaps_free_block( m) && "allocate: block overlaps with free blocks");
        RAMEN_ASSERT( block_in_pool( m)        && "allocate: returning block not in pool");

        return reinterpret_cast<unsigned char*>( m + 1);
    }

    void deallocate( unsigned char *p)
    {
        mem_block_t *m = reinterpret_cast<mem_block_t*>( p) - 1;

        // preconditions
        RAMEN_ASSERT( block_in_pool( m)        && "deallocate: block not in pool");
        RAMEN_ASSERT( !m->is_linked()          && "deallocate: block is already linked");
        RAMEN_ASSERT( !m->free                 && "deallocate: trying to free an already freed block");
        RAMEN_ASSERT( !overlaps_free_block( m) && "deallocate: block overlaps with free blocks");

        insert_free_block_sorted( m);
        RAMEN_ASSERT( list_is_sorted() && "insert free block sorted: list not sorted");

        compact_free_list();
    }

    // private private implementation

    void insert_free_block_sorted( mem_block_t *m)
    {
        m->free = true;
        iterator it = begin();

        if( ( it == end()) || ( m < &*it))
        {
            // insert first
            free_list_.push_front( *m);
            return;
        }

        if( m > &free_list_.back())
        {
            // insert last
            free_list_.push_back( *m);
            return;
        }

        for( ; it != end(); ++it)
        {
            iterator next = it;
            ++next;

            if( ( m > &*it) && ( m < &*next))
                free_list_.insert( next, *m);
        }
    }

    iterator find_best_block( std::size_t s)
    {
        iterator first_fit = end();
        iterator best_fit = end();
        std::size_t best_size = std::numeric_limits<std::size_t>::max();

        for( iterator it( begin()); it != end(); ++it)
        {
            RAMEN_ASSERT( it->free && "find best block: non free block found");

            if( s <= it->size)
            {
                if( first_fit == end())
                    first_fit = it;

                if( it->size - s <= size_tolerance)
                {
                    if( it->size < best_size)
                    {
                        best_fit = it;
                        best_size = it->size;
                    }
                }
            }
        }

        if( best_fit == end())
            best_fit = first_fit;

        return best_fit;
    }

    void split_block( iterator it, std::size_t s)
    {
        // preconditions
        RAMEN_ASSERT( it != end() && "split block: it == end()");
        RAMEN_ASSERT( it->free    && "split block: trying to split a used block");

        mem_block_t *new_block = it->split( s);

        iterator next = it;
        ++next;

        if( next == end())
            free_list_.push_back( *new_block);
        else
            free_list_.insert( next, *new_block);

        // postconditions
        RAMEN_ASSERT( list_is_sorted()            && "split block: list not sorted");
        RAMEN_ASSERT( block_in_pool( &*it)        && "split block: block not in pool");
        RAMEN_ASSERT( block_in_pool( new_block)   && "split block: block not in pool");
    }

    void compact_free_list()
    {
        std::size_t biggest = 0;

        for( iterator it( begin()); it != end(); ++it)
        {
            while( merge_block( it))
                ;

            biggest = std::max( biggest, it->size);
        }
    }

    bool merge_block( iterator it)
    {
        iterator next = it;
        ++next;

        if( next != end())
        {
            if( it->merge( *next))
            {
                free_list_.erase( next);
                return true;
            }
        }

        return false;
    }

    // testing methods
    bool item_in_list( mem_block_t *m)
    {
        for( iterator it( begin()); it != end(); ++it)
        {
            if( m == &*it)
                return true;
        }

        return false;
    }

    bool list_is_sorted()
    {
        for( iterator it( begin()); it != end(); ++it)
        {
            iterator next = it;
            ++next;

            if( next == end())
                break;

            if( &*it >= &*next)
                return false;
        }

        return true;
    }

    bool overlaps_free_block( mem_block_t *m)
    {
        for( iterator it( begin()); it != end(); ++it)
        {
            if( m->overlaps( *it))
                return true;
        }

        return false;
    }

    bool overlaping_free_blocks()
    {
        for( iterator it1( begin()); it1 != end(); ++it1)
        {
            for( iterator it2( begin()); it2 != end(); ++it2)
            {
                if( it1->overlaps( *it2))
                    return true;
            }
        }

        return false;
    }

    bool block_in_pool( mem_block_t *m)
    {
        unsigned char *p = reinterpret_cast<unsigned char*>( m);

        if( p < pool.get())
            return false;

        p += ( sizeof( mem_block_t) + m->size);

        if( p > ( pool.get() + pool_size))
            return false;

        return true;
    }

    boost::scoped_array<unsigned char> pool;
    std::size_t pool_size;

    // our list of free blocks
    bi::list<mem_block_t> free_list_;
};

pool_t::pool_t() {}

pool_t::~pool_t()
{
    // empty constructor to allow auto_ptr
    // use an incomplete type. Don't remove
}

void pool_t::init( std::size_t size)
{
    RAMEN_ASSERT( !pimpl_.get() && "image_mem_pool_t::init already called");
    pimpl_.reset( new implementation_t( size));
	allocated_ = 0;
}

std::size_t pool_t::pool_size() const
{
    RAMEN_ASSERT( pimpl_.get() && "image_mem_pool_t::init not called");
    return pimpl_->pool_size;
}

unsigned char *pool_t::allocate( std::size_t& size)
{
    RAMEN_ASSERT( pimpl_.get() && "image_mem_pool_t::init not called");
	
	unsigned char *p = pimpl_->allocate( size);
	
	if( p)
		allocated_ += size;
	
	return p;
}

void pool_t::deallocate( unsigned char *p, std::size_t size)
{
    RAMEN_ASSERT( pimpl_.get() && "image_mem_pool_t::init not called");
    pimpl_->deallocate( p);
	allocated_ -= size;
}

} // namespace
} // namespace
