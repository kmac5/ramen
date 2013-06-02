// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/memory.hpp>

#include<ramen/app/application.hpp>

#ifndef NDEBUG
	#include<glog/logging.h>
#endif

namespace ramen
{
namespace ofx
{
	
memory_instance_t::memory_instance_t() : OFX::Host::Memory::Instance(), _size( 0)
{
}

memory_instance_t::~memory_instance_t() { freeMem();}

bool memory_instance_t::alloc( std::size_t nBytes)
{
	if( !_locked)
	{
		freeMem();
		
		_size = nBytes;
		memory::manager_t::image_allocator_type& alloc( application_t::Instance().memory_manager().image_allocator());
		_ptr = reinterpret_cast<char *>( alloc.allocate( _size));
		
		#ifndef NDEBUG
			DLOG( INFO) << "memory_instance_t::alloc, this = " << ( void *) this << " size = " << _size << " ptr = " << ( void *) _ptr;
		#endif
			
		return true;
	}

	#ifndef NDEBUG
		DLOG( INFO) << "memory_instance_t::alloc, this = " << ( void *) this << " size = " << nBytes << ". failed, memory was locked";
	#endif
		
	return false;
}

OfxImageMemoryHandle memory_instance_t::getHandle() { return OFX::Host::Memory::Instance::getHandle();}

void memory_instance_t::freeMem()
{
	if( _ptr)
	{
		#ifndef NDEBUG
			DLOG( INFO) << "memory_instance_t::freeMem, this = " << ( void *) this << " size = " << _size << " ptr = " << ( void *) _ptr;
		#endif
			
		memory::manager_t::image_allocator_type& alloc( application_t::Instance().memory_manager().image_allocator());
		alloc.deallocate( reinterpret_cast<unsigned char*>( _ptr), _size);
		_ptr = 0;
		_size = 0;
	}
}

void *memory_instance_t::getPtr()	{ return OFX::Host::Memory::Instance::getPtr();}
void memory_instance_t::lock()		{ OFX::Host::Memory::Instance::lock();}
void memory_instance_t::unlock()	{ OFX::Host::Memory::Instance::unlock();}

} // ofx
} // ramen
