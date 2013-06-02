// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/params/double_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/float_param.hpp>

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

double_param_t::double_param_t( const std::string& name,
			       OFX::Host::Param::Descriptor& descriptor,
			       OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::DoubleInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus double_param_t::copy( const OFX::Host::Param::Instance& instance, OfxTime offset)
{
	return missing_host_feature();
}

OfxStatus double_param_t::copy( const OFX::Host::Param::Instance& instance, OfxTime offset, OfxRangeD range)
{
	return missing_host_feature();
}

OfxStatus double_param_t::get( double& v)
{
    v = get_value<float>( effect_->node()->param( getName()));
    return kOfxStatOK;
}

OfxStatus double_param_t::get( OfxTime time, double& v)
{
    v = get_value_at_frame<float>( effect_->node()->param( getName()), time);
	
	#ifndef NDEBUG
		DLOG( INFO) << "get_value, node = " << effect_->node()->name() << ", param = " << getName() << ", time = " << time << ", value = " << v;
    #endif
		
	return kOfxStatOK;
}

OfxStatus double_param_t::set( double v)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
		t = effect_->node()->composition()->frame();

    return set( t, v);
}

OfxStatus double_param_t::set( OfxTime time, double v)
{
    float_param_t& param( dynamic_cast<float_param_t&>( effect_->node()->param( getName())));
    param.set_value_at_frame( v, time, param_t::node_edited);
    param.update_widgets();
    return kOfxStatOK;
}

OfxStatus double_param_t::derive( OfxTime time, double& v)
{
    float_param_t& param( dynamic_cast<float_param_t&>( effect_->node()->param( getName())));
    v = param.derive( time);
    return kOfxStatOK;
}

OfxStatus double_param_t::integrate( OfxTime time1, OfxTime time2, double& v)
{
    float_param_t& param( dynamic_cast<float_param_t&>( effect_->node()->param( getName())));
    v = param.integrate( time1, time2);
    return kOfxStatOK;
}

OfxStatus double_param_t::getNumKeys( unsigned int& nKeys) const
{
	return kOfxStatErrMissingHostFeature;
}

OfxStatus double_param_t::getKeyTime( int nth, OfxTime& time) const
{
	return missing_host_feature();
}

OfxStatus double_param_t::getKeyIndex( OfxTime time, int direction, int& index) const
{
	return missing_host_feature();
}

OfxStatus double_param_t::deleteKey( OfxTime time)
{
	return missing_host_feature();
}

OfxStatus double_param_t::deleteAllKeys()
{
	return missing_host_feature();
}

void double_param_t::setEnabled()
{
    float_param_t& param( dynamic_cast<float_param_t&>( effect_->node()->param( getName())));
	param.set_enabled( getEnabled());
}

} // namespace
} // namespace
