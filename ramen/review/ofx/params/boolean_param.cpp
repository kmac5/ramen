// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/params/boolean_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/bool_param.hpp>

#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/util.hpp>

#ifndef NDEBUG
	#include<glog/logging.h>
#endif

namespace ramen
{
namespace ofx
{

extern bool log_param_info;

boolean_param_t::boolean_param_t( const std::string& name,
			   OFX::Host::Param::Descriptor& descriptor,
			   OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::BooleanInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus boolean_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset)
{
	return missing_host_feature();
}

OfxStatus boolean_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range)
{
	return missing_host_feature();
}

OfxStatus boolean_param_t::get( bool& v) { return get( 1.0, v);}

OfxStatus boolean_param_t::get( OfxTime time, bool& v)
{
    v = get_value<bool>( effect_->node()->param( getName()));
	
	#ifndef NDEBUG
		DLOG( INFO) << "get_value, node = " << effect_->node()->name() << ", param = " << getName() << ", value = " << v;
	#endif

    return kOfxStatOK;
}

OfxStatus boolean_param_t::set( bool v)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
		t = effect_->node()->composition()->frame();

    return set( t, v);
}

OfxStatus boolean_param_t::set( OfxTime time, bool v)
{
    bool_param_t& param( dynamic_cast<bool_param_t&>( effect_->node()->param( getName())));
    param.set_value( v, param_t::node_edited);
    param.update_widgets();
    return kOfxStatOK;
}

void boolean_param_t::setEnabled()
{
    bool_param_t& param( dynamic_cast<bool_param_t&>( effect_->node()->param( getName())));
	param.set_enabled( getEnabled());
}

} // namespace
} // namespace
