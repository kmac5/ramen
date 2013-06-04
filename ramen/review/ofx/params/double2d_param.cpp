// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/params/double2d_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/float2_param.hpp>

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

double2d_param_t::double2d_param_t( const std::string& name,
				   OFX::Host::Param::Descriptor& descriptor,
				   OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::Double2DInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus double2d_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset)
{
	return missing_host_feature();
}

OfxStatus double2d_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range)
{
	return missing_host_feature();
}

OfxStatus double2d_param_t::get( double& x, double& y)
{
    Imath::V2f v( get_value<Imath::V2f>( effect_->node()->param( getName())));
    x = v.x; y = v.y;
    return kOfxStatOK;
}

OfxStatus double2d_param_t::get( OfxTime time, double& x, double& y)
{
    Imath::V2f v( get_value_at_frame<Imath::V2f>( effect_->node()->param( getName()), time));
    x = v.x; y = v.y;
	
	#ifndef NDEBUG
		DLOG( INFO) << "get_value, node = " << effect_->node()->name() << ", param = " << getName() << ", time = " << time << ", value = " << x << ", " << y;	
	#endif
		
    return kOfxStatOK;
}

OfxStatus double2d_param_t::set( double x, double y)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
		t = effect_->node()->composition()->frame();

    return set( t, x, y);
}

OfxStatus double2d_param_t::set( OfxTime time, double x, double y)
{
    float2_param_t& param( dynamic_cast<float2_param_t&>( effect_->node()->param( getName())));
    param.set_value_at_frame( Imath::V2f( x, y), time, param_t::node_edited);
    param.update_widgets();
    return kOfxStatOK;
}

OfxStatus double2d_param_t::getNumKeys(unsigned int& nKeys) const
{
	return kOfxStatErrMissingHostFeature;
}

OfxStatus double2d_param_t::getKeyTime(int nth, OfxTime& time) const
{
	return missing_host_feature();
}

OfxStatus double2d_param_t::getKeyIndex(OfxTime time, int direction, int& index) const
{
	return missing_host_feature();
}

OfxStatus double2d_param_t::deleteKey(OfxTime time)
{
	return missing_host_feature();
}

OfxStatus double2d_param_t::deleteAllKeys()
{
	return missing_host_feature();
}

void double2d_param_t::setEnabled()
{
    float2_param_t& param( dynamic_cast<float2_param_t&>( effect_->node()->param( getName())));
	param.set_enabled( getEnabled());
}

} // namespace
} // namespace
