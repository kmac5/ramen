// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/cuda/memory_pool.hpp>

#include<list>
#include<limits>

#include<ramen/assert.hpp>

#include<ramen/cuda/manager.hpp>
#include<ramen/cuda/cudart.hpp>

namespace ramen
{
namespace cuda
{
	
struct memory_pool_t::implementation_t
{
    const static std::size_t size_tolerance = 64 * 1024; // 64 Kb?

	struct mem_block_t
	{
		mem_block_t( unsigned char *p, std::size_t s) {}

        void split( std::size_t s)
        {
			RAMEN_ASSERT( s < size);

			ptr = ptr + s;
			size -= s;
        }

        bool merge( const mem_block_t& other)
        {
            if( !is_contiguous( other))
                return false;

            size += other.size;
            return true;
        }

        bool is_contiguous( const mem_block_t& other) const
        {
            return ptr + size == other.ptr;
        }

		unsigned char *ptr;
		std::size_t size;
	};

    typedef std::list<mem_block_t>::iterator iterator;

    iterator begin()	{ return free_list_.begin();}
    iterator end()		{ return free_list_.end();}

    implementation_t( std::size_t size_in_bytes) : pool_size( size_in_bytes), dev_mem_( 0)
	{
		context_lock_t lock;
		dev_mem_ = cuda::cuda_malloc( pool_size);
		free_list_.push_back( mem_block_t( reinterpret_cast<unsigned char*>( dev_mem_), pool_size));
	}

	~implementation_t()
	{
		if( dev_mem_)
		{
			context_lock_t lock;
			cuda::cuda_free( dev_mem_);
		}
	}

    // public private interface
    unsigned char *allocate( std::size_t& size)
	{
		RAMEN_ASSERT( dev_mem_);

        iterator it( find_best_block( size));

        if( it == end())
            return 0;

        RAMEN_ASSERT( it->size >= size && "find best block: block smaller than size requested\n");

        if( it->size - size > size_tolerance)
        {
			// we need to split the block
			unsigned char *ptr = it->ptr;
			it->split( size);
			return ptr;
		}
		else
		{
			unsigned char *ptr = it->ptr;
			size = it->size;
			free_list_.erase( it);
			return ptr;
		}
	}

    void deallocate( unsigned char *p, std::size_t size)
	{
		RAMEN_ASSERT( dev_mem_);
		RAMEN_ASSERT( size);

		mem_block_t block( p, size);
		insert_free_block_sorted( block);
		compact_free_list();
	}

    iterator find_best_block( std::size_t s)
    {
        iterator first_fit = end();
        iterator best_fit = end();
        std::size_t best_size = std::numeric_limits<std::size_t>::max();

        for( iterator it( begin()); it != end(); ++it)
        {
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

    void insert_free_block_sorted( const mem_block_t& m)
    {
        iterator it = begin();

        if( ( it == end()) || ( m.ptr < it->ptr))
        {
            // insert first
            free_list_.push_front( m);
            return;
        }

        if( m.ptr > free_list_.back().ptr)
        {
            // insert last
            free_list_.push_back( m);
            return;
        }

        for( ; it != end(); ++it)
        {
            iterator next = it;
            ++next;

            if( ( m.ptr > it->ptr) && ( m.ptr < next->ptr))
                free_list_.insert( next, m);
        }
    }

    void compact_free_list()
    {
        // std::cout << "compact free list: before = " << free_list_.size();
        std::size_t biggest = 0;

        for( iterator it( begin()); it != end(); ++it)
        {
            while( merge_block( it))
                ;

            biggest = std::max( biggest, it->size);
        }

        // std::cout << " after = " << free_list_.size() << " items\n";
        // std::cout << "biggest block = " << biggest / 1024 / 1024 << " Mb\n";
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

    std::size_t pool_size;

	void *dev_mem_;
	std::list<mem_block_t> free_list_;
};

memory_pool_t::memory_pool_t() {}

memory_pool_t::~memory_pool_t()
{
    // empty constructor to allow auto_ptr
    // use an incomplete type. Don't remove
}

void memory_pool_t::init( std::size_t size)
{
    RAMEN_ASSERT( !pimpl_.get() && "cuda mem_pool_t::init already called");

	if( initialized())
	{
		try
		{
		    pimpl_.reset( new implementation_t( size));
		}
		catch( cuda::error& e)
		{
		}
	}
}

std::size_t memory_pool_t::pool_size() const
{
	if( !pimpl_.get())
		return 0;

    return pimpl_->pool_size;
}

unsigned char *memory_pool_t::allocate( std::size_t& size)
{
	if( !pimpl_.get())
		return 0;

	return pimpl_->allocate( size);
}

void memory_pool_t::deallocate( unsigned char *p, std::size_t size)
{
	if( p)
	{
		RAMEN_ASSERT( pimpl_.get());
		pimpl_->deallocate( p, size);
	}
}

} // namespace
} // namespace
