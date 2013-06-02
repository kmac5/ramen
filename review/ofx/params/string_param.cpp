// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/params/string_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/string_param.hpp>

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

string_param_t::string_param_t( const std::string& name,
			       OFX::Host::Param::Descriptor& descriptor,
			       OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::StringInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus string_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset)
{
	return missing_host_feature();
}

OfxStatus string_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range)
{
	return missing_host_feature();
}

OfxStatus string_param_t::get( std::string& s) { return get( 1.0, s);}

OfxStatus string_param_t::get( OfxTime time, std::string& s)
{
    s = get_value<std::string>( effect_->node()->param( getName()));
	
	#ifndef NDEBUG
		DLOG( INFO) << "get_value, node = " << effect_->node()->name() << ", param = " << getName() << ", value = " << s;
	#endif
		
    return kOfxStatOK;
}

OfxStatus string_param_t::set( const char *s)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
		t = effect_->node()->composition()->frame();

    return set( t, s);
}

OfxStatus string_param_t::set(OfxTime time, const char *s)
{
    ramen::string_param_t& param( dynamic_cast<ramen::string_param_t&>( effect_->node()->param( getName())));
    param.set_value( std::string( s), param_t::node_edited);
    param.update_widgets();
    return kOfxStatOK;
}

void string_param_t::setEnabled()
{
    ramen::string_param_t& param( dynamic_cast<ramen::string_param_t&>( effect_->node()->param( getName())));
	param.set_enabled( getEnabled());
}

} // namespace
} // namespace
