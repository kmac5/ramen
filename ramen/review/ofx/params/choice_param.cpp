// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/params/choice_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/util.hpp>

#ifndef NDEBUG
	#include<glog/logging.h>
#endif

namespace ramen
{
namespace ofx
{

extern bool log_param_info;

choice_param_t::choice_param_t( const std::string& name,
			       OFX::Host::Param::Descriptor& descriptor,
			       OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::ChoiceInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus choice_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset)
{
	return missing_host_feature();
}

OfxStatus choice_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range)
{
	return missing_host_feature();
}

OfxStatus choice_param_t::get( int& v) { return get( 1.0, v);}

OfxStatus choice_param_t::get( OfxTime time, int& v)
{
    v = get_value<int>( effect_->node()->param( getName()));
	
	#ifndef NDEBUG
		DLOG( INFO) << "get_value, node = " << effect_->node()->name() << ", param = " << getName() << ", value = " << v;
	#endif
		
    return kOfxStatOK;
}

OfxStatus choice_param_t::set(int v)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
		t = effect_->node()->composition()->frame();

    return set( t, v);
}

OfxStatus choice_param_t::set(OfxTime time, int v)
{
    popup_param_t& param( dynamic_cast<popup_param_t&>( effect_->node()->param( getName())));
    param.set_value( v, param_t::node_edited);
    param.update_widgets();
    return kOfxStatOK;
}

void choice_param_t::setEnabled()
{
    popup_param_t& param( dynamic_cast<popup_param_t&>( effect_->node()->param( getName())));
	param.set_enabled( getEnabled());
}

} // namespace
} // namespace
