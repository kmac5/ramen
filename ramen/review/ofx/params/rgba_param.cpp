// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/params/rgba_param.hpp>

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

rgba_color_param_t::rgba_color_param_t( const std::string& name,
				       OFX::Host::Param::Descriptor& descriptor,
				       OFX::Host::Param::SetInstance* instance) : OFX::Host::Param::RGBAInstance( descriptor, instance)
{
    effect_ = dynamic_cast<image_effect_t*>( instance);
}

OfxStatus rgba_color_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset)
{
    return missing_host_feature();
}

OfxStatus rgba_color_param_t::copy( const OFX::Host::Param::Instance &instance, OfxTime offset, OfxRangeD range)
{
    return missing_host_feature();
}

OfxStatus rgba_color_param_t::get( double& r, double& g, double& b, double& a)
{
    Imath::Color4f x( get_value<Imath::Color4f>( effect_->node()->param( getName())));
    r = x.r; g = x.g; b = x.b; a = x.a;
    return kOfxStatOK;
}

OfxStatus rgba_color_param_t::get( OfxTime time, double& r, double& g, double& b, double& a)
{
    Imath::Color4f x( get_value_at_frame<Imath::Color4f>( effect_->node()->param( getName()), time));
    r = x.r; g = x.g; b = x.b; a = x.a;
	
	#ifndef NDEBUG
		DLOG( INFO) << "get_value, node = " << effect_->node()->name() << ", param = " << getName()
					<< ", time = " << time << ", value = " << r << ", " << g << ", " << b << ", " << a;
	#endif
		
    return kOfxStatOK;
}

OfxStatus rgba_color_param_t::set( double r, double g, double b, double a)
{
    OfxTime t = 1.0;

    if( effect_->node()->composition())
		t = effect_->node()->composition()->frame();

    return set( t, r, g, b, a);
}

OfxStatus rgba_color_param_t::set( OfxTime time, double r, double g, double b, double a)
{
    color_param_t& param( dynamic_cast<color_param_t&>( effect_->node()->param( getName())));
    param.set_value_at_frame( Imath::Color4f( r, g, b, a), time, param_t::node_edited);
    param.update_widgets();
    return kOfxStatOK;
}

OfxStatus rgba_color_param_t::getNumKeys( unsigned int& nKeys) const
{
    return missing_host_feature();
}

OfxStatus rgba_color_param_t::getKeyTime( int nth, OfxTime& time) const
{
    return missing_host_feature();
}

OfxStatus rgba_color_param_t::getKeyIndex( OfxTime time, int direction, int& index) const
{
    return missing_host_feature();
}

OfxStatus rgba_color_param_t::deleteKey( OfxTime time)
{
    return missing_host_feature();
}

OfxStatus rgba_color_param_t::deleteAllKeys()
{
    return missing_host_feature();
}

void rgba_color_param_t::setEnabled()
{
    color_param_t& param( dynamic_cast<color_param_t&>( effect_->node()->param( getName())));
	param.set_enabled( getEnabled());
}

} // namespace
} // namespace
