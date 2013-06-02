// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_OFX_STRING_PARAM_HPP
#define RAMEN_OFX_STRING_PARAM_HPP

#include"ofxCore.h"
#include"ofxParam.h"

#include"ofxhPropertySuite.h"
#include"ofxhParam.h"

namespace ramen
{
namespace ofx
{

class image_effect_t;

class string_param_t : public OFX::Host::Param::StringInstance
{
public:

    string_param_t( const std::string& name, OFX::Host::Param::Descriptor& descriptor, OFX::Host::Param::SetInstance* instance = 0);

    OfxStatus copy( const Instance &instance, OfxTime offset);
    OfxStatus copy( const Instance &instance, OfxTime offset, OfxRangeD range);

    OfxStatus get( std::string& s);
    OfxStatus get( OfxTime time, std::string& s);
    OfxStatus set( const char *s);
    OfxStatus set( OfxTime time, const char *s);

	// ui
	virtual void setEnabled();
	
private:

    image_effect_t *effect_;
};

} // namespace
} // namespace

#endif
