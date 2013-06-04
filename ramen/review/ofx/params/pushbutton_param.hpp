// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_OFX_PUSHBUTTON_PARAM_HPP
#define RAMEN_OFX_PUSHBUTTON_PARAM_HPP

#include"ofxCore.h"
#include"ofxParam.h"

#include"ofxhPropertySuite.h"
#include"ofxhParam.h"

namespace ramen
{
namespace ofx
{

class image_effect_t;
	
class pushbutton_param_t : public OFX::Host::Param::PushbuttonInstance
{
public:

    pushbutton_param_t( const std::string& name, OFX::Host::Param::Descriptor& descriptor,
						OFX::Host::Param::SetInstance* instance = 0);

	// ui
	virtual void setEnabled();
	
private:

    image_effect_t *effect_;
};

} // namespace
} // namespace

#endif
