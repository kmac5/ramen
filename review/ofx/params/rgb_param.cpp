// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/params/rgb_param.hpp>

#include<ramen/app/composition.hpp>
#include<ramen/params/color_param.hpp>

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

rgb_color_param_t::rgb_color_param_t( const std::string& name,
								     OFX::Host::Param::Descriptor& descriptor,
								     OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::RGBInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus rgb_color_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset)
{
    return missing_host_feature();
}

OfxStatus rgb_color_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range)
{
    return missing_host_feature();
}

OfxStatus rgb_color_param_t::get( double& r, double& g, double& b)
{
    Imath::Color4f x( get_value<Imath::Color4f>( effect_->node()->param( getName())));
    r = x.r; g = x.g; b = x.b;
    return kOfxStatOK;
}

OfxStatus rgb_color_param_t::get( OfxTime time, double& r, double& g, double& b)
{
    Imath::Color4f x( get_value_at_frame<Imath::Color4f>( effect_->node()->param( getName()), time));
    r = x.r; g = x.g; b = x.b;
	
	#ifndef NDEBUG
		DLOG( INFO) << "get_value, node = " << effect_->node()->name() << ", param = " << getName()
					<< ", time = " << time << ", value = " << r << ", " << g << ", " << b;
	#endif
		
    return kOfxStatOK;
}

OfxStatus rgb_color_param_t::set( double r, double g, double b)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
		t = effect_->node()->composition()->frame();

    return set( t, r, g, b);
}

OfxStatus rgb_color_param_t::set( OfxTime time, double r, double g, double b)
{
    color_param_t& param( dynamic_cast<color_param_t&>( effect_->node()->param( getName())));
    param.set_value_at_frame( Imath::Color4f( r, g, b, 1), time, param_t::node_edited);
    param.update_widgets();
    return kOfxStatOK;
}

OfxStatus rgb_color_param_t::getNumKeys( unsigned int& nKeys) const
{
    return missing_host_feature();
}

OfxStatus rgb_color_param_t::getKeyTime( int nth, OfxTime& time) const
{
    return missing_host_feature();
}

OfxStatus rgb_color_param_t::getKeyIndex( OfxTime time, int direction, int& index) const
{
    return missing_host_feature();
}

OfxStatus rgb_color_param_t::deleteKey( OfxTime time)
{
    return missing_host_feature();
}

OfxStatus rgb_color_param_t::deleteAllKeys()
{
    return missing_host_feature();
}

void rgb_color_param_t::setEnabled()
{
    color_param_t& param( dynamic_cast<color_param_t&>( effect_->node()->param( getName())));
	param.set_enabled( getEnabled());
}

} // namespace
} // namespace
