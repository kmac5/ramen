// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_OFX_CHOICE_PARAM_HPP
#define RAMEN_OFX_CHOICE_PARAM_HPP

#include"ofxCore.h"
#include"ofxParam.h"

#include"ofxhPropertySuite.h"
#include"ofxhParam.h"

namespace ramen
{
namespace ofx
{

class image_effect_t;

class choice_param_t : public OFX::Host::Param::ChoiceInstance
{
public:

    choice_param_t( const std::string& name, OFX::Host::Param::Descriptor& descriptor, OFX::Host::Param::SetInstance* instance = 0);

    OfxStatus copy( const Instance &instance, OfxTime offset);
    OfxStatus copy( const Instance &instance, OfxTime offset, OfxRangeD range);

    OfxStatus get(int&);
    OfxStatus get(OfxTime time, int&);
    OfxStatus set(int);
    OfxStatus set(OfxTime time, int);

	// ui
	virtual void setEnabled();
	
private:

    image_effect_t *effect_;
};

} // namespace
} // namespace

#endif
