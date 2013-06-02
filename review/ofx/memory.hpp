// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_OFX_MEMORY_HPP
#define RAMEN_OFX_MEMORY_HPP

#include<cstddef>

#include"ofxCore.h"
#include"ofxImageEffect.h"

#include"ofxhMemory.h"

namespace ramen
{
namespace ofx
{

class memory_instance_t : public OFX::Host::Memory::Instance
{
public:
	
	  memory_instance_t();
	  virtual ~memory_instance_t();

	  virtual bool alloc( std::size_t nBytes);
	  virtual OfxImageMemoryHandle getHandle();
	  virtual void freeMem();
	  virtual void *getPtr();
	  virtual void lock();
	  virtual void unlock();

protected:

	std::size_t _size;
};
	
	
} // ofx
} // ramen

#endif
