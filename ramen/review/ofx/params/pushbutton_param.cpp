// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/params/pushbutton_param.hpp>
#include<ramen/params/button_param.hpp>

#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/util.hpp>

namespace ramen
{
namespace ofx
{

pushbutton_param_t::pushbutton_param_t( const std::string& name,
			       OFX::Host::Param::Descriptor& descriptor,
			       OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::PushbuttonInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);	
}

void pushbutton_param_t::setEnabled()
{
    button_param_t& param( dynamic_cast<button_param_t&>( effect_->node()->param( getName())));
	param.set_enabled( getEnabled());
}

} // namespace
} // namespace
