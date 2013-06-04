// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/ofx/ofx_node.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>

#include<ramen/assert.hpp>

#include<ramen/params/composite_param.hpp>

#include<ramen/ofx/plugin_manager.hpp>
#include<ramen/ofx/host.hpp>
#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/clip.hpp>
#include<ramen/ofx/param.hpp>
#include<ramen/ofx/manipulator.hpp>
#include<ramen/ofx/util.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/ui/palette.hpp>

#ifndef NDEBUG
	#include<glog/logging.h>
#endif

namespace ramen
{
namespace image
{
namespace
{

std::string reason_to_string( param_t::change_reason reason)
{
	switch( reason)
	{
		case param_t::user_edited:
			return std::string( "user edited");
	
		case param_t::node_edited:
			return std::string( "node edited");
	
		case param_t::node_loaded:
			return std::string( "node loaded");
	
		case param_t::time_changed:
			return std::string( "time changed");
	}
}

} // unnamed
	
ofx_node_t::ofx_node_t( OFX::Host::ImageEffect::ImageEffectPlugin *plugin) : image_node_t(), descriptor_( 0), effect_( 0)
{
    set_name( plugin->getDescriptor().getLabel());
    id_ = plugin->getIdentifier();
    major_version_ = plugin->getVersionMajor();
    minor_version_ = plugin->getVersionMinor();
	
    plugin_ = plugin;
    get_descriptor();
		
	// create plugs
    all_inputs_optional_ = true;
	
    BOOST_FOREACH( OFX::Host::ImageEffect::ClipDescriptor *clip, descriptor_->getClipsByOrder())
    {
		if( clip->isOutput())
			add_output_plug();
		else
		{
			input_plug_info_t input_info;
	
			if( clip->isOptional())
				input_info.set_color( ui::palette_t::Instance().color("optional plug"));
			else
				all_inputs_optional_ = false;
	
			input_info.set_tooltip( clip->getLongLabel());			
			add_input_plug( input_info, clip->isOptional());
		}
    }
	
    param_changed_connection_ = param_set().param_changed.connect( boost::bind( &ofx_node_t::param_changed, this, _1, _2));
}

ofx_node_t::ofx_node_t( const ofx_node_t& other) : image_node_t( other), id_(other.id_), effect_( 0)
{
    major_version_ = other.major_version_;
    minor_version_ = other.minor_version_;
    plugin_ = other.plugin_;
    descriptor_ = other.descriptor_;
    ofx_context_ = other.ofx_context_;
	all_inputs_optional_ = other.all_inputs_optional_;
    param_changed_connection_ = param_set().param_changed.connect( boost::bind( &ofx_node_t::param_changed, this, _1, _2));
	
	#ifndef NDEBUG
		DLOG( INFO) << "ofx_node_t::ofx_node_t( const ofx_node_t& other) called\n";
	#endif
}

ofx_node_t::~ofx_node_t()
{
    // this is to avoid a crash when the node is deleted.
    // It's a hack. Look for the real problem instead...
	manipulators().clear();
	delete effect_;
}

node_t *ofx_node_t::do_clone() const
{	
	if( effect_)
	{
		ofx::image_effect_t *fx = const_cast<ofx::image_effect_t*>( effect_);
		fx->syncPrivateDataAction();
		
		#ifndef NDEBUG
			DLOG( INFO) << "syncPrivateDataAction, node = " << name();
		#endif
	}

    ofx_node_t *new_node = new ofx_node_t( *this);
    new_node->create_effect_instance();

	if( new_node->effect_)
	{
		OfxStatus stat = effect_->beginInstanceEditAction();
		DLOG( INFO) << "beginInstanceEditAction, node = " << name();

		//if( stat != kOfxStatOK && stat != kOfxStatReplyDefault)
		//	throw std::runtime_error( "Error while calling begin instance edit action");
	}

    return new_node;
}

int ofx_node_t::index_for_clip_name( const std::string& name) const
{
    // This is for some plugins that try to access clips before they are created.
    if( name == "Source")
		return 0;

    if( name == "Output")
		return -1;

	int i = 0;
    BOOST_FOREACH( OFX::Host::ImageEffect::ClipDescriptor *clip, descriptor_->getClipsByOrder())
    {
		if( !clip->isOutput())
		{
			if( name == clip->getName())
				return i;

			++i;
		}
	}

    RAMEN_ASSERT( 0 && "OFX index_for_clip: clip not found");
	return -2;
}

void ofx_node_t::get_descriptor()
{
    RAMEN_ASSERT( plugin_ != 0);
	RAMEN_ASSERT( descriptor_ == 0);

	// get the contexts supported by the plugin
	const std::set<std::string>& contexts( plugin_->getContexts());
	
	if( !ofx_context_.empty())
	{
		// we already have a context, the node was read from a file
		descriptor_ = plugin_->getContext( ofx_context_);
		return;
	}
		
	// try to get a descriptor
	descriptor_ = plugin_->getContext( kOfxImageEffectContextGeneral);

	if( descriptor_)
		ofx_context_ = kOfxImageEffectContextGeneral;
	else
	{
		descriptor_ = plugin_->getContext( kOfxImageEffectContextFilter);

		if( descriptor_)
			ofx_context_ = kOfxImageEffectContextFilter;
		else
		{
			descriptor_ = plugin_->getContext( kOfxImageEffectContextGenerator);

			if( descriptor_)
				ofx_context_ = kOfxImageEffectContextGenerator;
			else
			{
				descriptor_ = plugin_->getContext( kOfxImageEffectContextTransition);

				if( descriptor_)
					ofx_context_ = kOfxImageEffectContextTransition;
			}
		}
	}
	
    if( !descriptor_)
	{
		#ifndef NDEBUG
			DLOG( INFO) << "Couldn't get descriptor for OFX plugin";
		#endif
			
		throw std::runtime_error( "Can't get descriptor for OFX plugin");
	}
}

void ofx_node_t::create_effect_instance()
{
	RAMEN_ASSERT( plugin_);
	RAMEN_ASSERT( descriptor_);
	RAMEN_ASSERT( !effect_ && "OFX: trying to create an effect instance for a node which already has one");

    // create the effect in the same context it was described in.
    effect_ = dynamic_cast<ofx::image_effect_t*>( plugin_->createInstance( ofx_context_, this));
	
	#ifndef NDEBUG
		DLOG( INFO) << "createInstance called. node = " << name() << " effect = " << ( void *) effect_;
	#endif

	if( !effect_)
	{
		#ifndef NDEBUG
			DLOG( INFO) << "createInstance called. node = " << name() << " failed";
		#endif
			
		throw std::runtime_error( std::string( "Couldn't create OFX plugin:") + id_);
	}
	
	OfxStatus stat = effect_->createInstanceAction();
	if( stat != kOfxStatOK && stat != kOfxStatReplyDefault)
		goto cleanup;
	
	if( !effect_->getClipPreferences())
		goto cleanup;
	
	return; // successfully

cleanup:

	delete effect_;
	effect_ = 0;
	
	#ifndef NDEBUG
		DLOG( INFO) << "createInstance called. node = " << name() << " failed";
	#endif
}

void ofx_node_t::do_create_params()
{
	RAMEN_ASSERT( descriptor_);

	BOOST_FOREACH( OFX::Host::Param::Descriptor *d, descriptor_->getParamList())
	{
	    std::auto_ptr<param_t> p( ofx::param_factory_t::create_param( d));

	    if( p.get())
	    {
			if( d->getType() == kOfxParamTypePage)
			    add_param( p);
			else
			{
			    // insert the param where it belong
			    if( d->getParentName() != "")
			    {
					composite_param_t *group = find_param( d->getParentName());
					RAMEN_ASSERT( group);
					group->add_param( p);
			    }
			    else
					add_param( p);
			}
	    }
	}

	create_effect_instance();

	if( effect_)
	{
		OfxStatus stat = effect_->beginInstanceEditAction();	
		if( stat != kOfxStatOK && stat != kOfxStatReplyDefault)
			throw std::runtime_error( "Error while calling begin instance edit action");
	}
}

void ofx_node_t::block_param_changed_signal( bool b)
{
    if( b)
		param_changed_connection_.block();
    else
		param_changed_connection_.unblock();
}

void ofx_node_t::param_changed( param_t *p, param_t::change_reason reason)
{
	if( effect_)
	{
		#ifndef NDEBUG
			DLOG( INFO) << "ofx_node_t::param_changed, node = " << name() << ", param = " << p->id() << ", reason = " << reason_to_string( reason);
		#endif

		float frame = 1.0f;	
		OfxPointD renderscale;

	    renderscale.x = 1.0 / render_context().subsample;
	    renderscale.y = renderscale.x;

		if( composition())
		    frame = composition()->frame();

		bool call_clip_prefs = true;
		
		switch( reason)
		{
			case param_t::user_edited:
				effect_->beginInstanceChangedAction( kOfxChangeUserEdited);		
				effect_->paramInstanceChangedAction( p->id(), kOfxChangeUserEdited, frame, renderscale);
				effect_->endInstanceChangedAction( kOfxChangeUserEdited);
			break;
			
			case param_t::node_edited:
				effect_->beginInstanceChangedAction( kOfxChangePluginEdited);
				effect_->paramInstanceChangedAction( p->id(), kOfxChangePluginEdited, frame, renderscale);
				effect_->endInstanceChangedAction( kOfxChangePluginEdited);
			break;

			case param_t::time_changed:
				effect_->beginInstanceChangedAction( kOfxChangeTime);
				effect_->paramInstanceChangedAction( p->id(), kOfxChangeTime, frame, renderscale);
				effect_->endInstanceChangedAction( kOfxChangeTime);
			break;
			
			default:
				call_clip_prefs = false;
			break;
		}

		if( call_clip_prefs && effect_->isClipPreferencesSlaveParam( p->id()))
		{
		    effect_->getClipPreferences();
			
			#ifndef NDEBUG
				DLOG( INFO) << "getClipPreferences called. node = " << name();
			#endif
		}
	}
}

composite_param_t *ofx_node_t::find_param( const std::string& name)
{
    try
    {
		return dynamic_cast<composite_param_t*>( &( param_set().find( name)));
    }
    catch( std::runtime_error& e)
    {
		return 0;
    }
}

// manipulators
void ofx_node_t::do_create_manipulators()
{
    if( effect_ && effect_->getOverlayInteractMainEntry())
    {
        try
        {
            std::auto_ptr<ofx::manipulator_t> m( new ofx::manipulator_t( this, effect_));
            add_manipulator( m);
        }
        catch( ...)
        {
        }
    }
}

void ofx_node_t::do_connected( node_t *src, int port)
{
    if( effect_)
    {
		effect_->getClipPreferences();
		
		effect_->beginInstanceChangedAction( kOfxChangeUserEdited);
		OfxPointD renderscale;
		float time = 1.0f;

		renderscale.x = 1.0 / render_context().subsample;
	    renderscale.y = renderscale.x;
		
		if( composition())
		    time = composition()->frame();

		for( int i = 0; i < effect_->getNClips(); ++i)
		{
		    ofx::clip_t *clip = dynamic_cast<ofx::clip_t*>( effect_->getNthClip( i));

		    if( clip->port() == port)
				effect_->clipInstanceChangedAction( clip->getName(), kOfxChangeUserEdited, time, renderscale);
		}

		effect_->endInstanceChangedAction( kOfxChangeUserEdited);
    }
}

// rendering
bool ofx_node_t::do_is_valid() const { return effect_;}

bool ofx_node_t::is_frame_varying() const
{
	if( effect_)
	{
	    if( effect_->isFrameVarying() || effect_->temporalAccess())
	        return true;
	}
    
    return false;
}

// areas
void ofx_node_t::do_calc_bounds( const render::context_t& context)
{
    OfxPointD scale;
    scale.x = scale.y = 1.0 / context.subsample;

    OfxRectD area;
    effect_->getRegionOfDefinitionAction( context.frame, scale, area);

    Imath::Box2i box( Imath::V2i( area.x1, area.y1), Imath::V2i( area.x2 - 1, area.y2 - 1));

    // flip the rod vertically, as OFX coordinate system is y up
    box = vertical_flip( box);
    set_bounds( box);
}

void ofx_node_t::do_calc_inputs_interest( const render::context_t& context)
{
	if( !effect_)
		return;
	
    OfxPointD scale;
    scale.x = scale.y = 1.0 / render_context().subsample;
    OfxRectD area;

    Imath::Box2i roi( interest());

    // flip the roi vertically. OFX coordinate system is y up
    roi = vertical_flip( roi);
    ++roi.max.x;
    ++roi.max.y;

    area.x1 = roi.min.x;
    area.y1 = roi.min.y;
    area.x2 = roi.max.x;
    area.y2 = roi.max.y;

    std::map<OFX::Host::ImageEffect::ClipInstance*, OfxRectD> rois;
    effect_->getRegionOfInterestAction( context.frame, scale, area, rois);

    RAMEN_ASSERT( rois.size() == num_inputs());

    for( std::map<OFX::Host::ImageEffect::ClipInstance*, OfxRectD>::const_iterator it( rois.begin()); it != rois.end(); ++it)
    {
		ofx::clip_t *clip = dynamic_cast<ofx::clip_t*>( it->first);
		Imath::Box2i box( Imath::V2i( it->second.x1, it->second.y1), Imath::V2i( it->second.x2 - 1, it->second.y2 - 1));

		// flip again as Ramen coordinate system is y down
		box = vertical_flip( box);

		if( input( clip->port()))
		    input_as<image_node_t>( clip->port())->add_interest( box);
	}
}

void ofx_node_t::do_calc_defined( const render::context_t& context)
{
    if( effect_->supportsTiles())
		set_defined( Imath::intersect( bounds(), interest()));
    else
		set_defined( bounds());
}

void ofx_node_t::do_recursive_process( const render::context_t& context)
{
	#ifndef NDEBUG
		DLOG( INFO) << "\n\nnode " << name() << ", recursive process";
	#endif
		
    // save the rendering context for later use
    render_context_ = context;

    alloc_image();

    // we render the input nodes on demand
    process( context);
}

void ofx_node_t::do_process( const render::context_t& context)
{
    OfxPointD scale;
    scale.x = scale.y = 1.0 / context.subsample;
	
	#ifndef NDEBUG
		DLOG( INFO) << "node " << name() << ", beginRenderAction, frame = " << context.frame << " scale = " << scale;
	#endif
		
    effect_->beginRenderAction( context.frame, context.frame, 1, context.mode == render::interface_render, scale);

    Imath::Box2i area( vertical_flip( defined()));
    OfxRectI render_window;
    render_window.x1 = area.min.x;
    render_window.y1 = area.min.y;
    render_window.x2 = area.max.x + 1;
    render_window.y2 = area.max.y + 1;

	#ifndef NDEBUG
		DLOG( INFO) << "node " << name() << " renderAction. render_window = " << render_window;
	#endif
		
    effect_->renderAction( context.frame, kOfxImageFieldBoth, render_window, scale);
	
	#ifndef NDEBUG
		DLOG( INFO) << "node " << name() << ", endRenderAction, frame = " << context.frame << " scale = " << scale << "\n\n";
	#endif
		
    effect_->endRenderAction( context.frame, context.frame, 1, context.mode == render::interface_render, scale);
}

// util
Imath::Box2i ofx_node_t::vertical_flip( const Imath::Box2i& b) const
{
    float cy = ( format().max.y + format().min.y) / 2.0f;
    return Imath::Box2i( Imath::V2i( b.min.x, -( b.max.y - cy) + cy),
                         Imath::V2i( b.max.x, -( b.min.y - cy) + cy));
}

Imath::V2f ofx_node_t::vertical_flip( const Imath::V2f& p, int subsample) const
{
	float y0 = format().min.y * subsample;
	float y1 = format().max.y * subsample;
	float cy = ( y0 + y1) / 2.0f;
	return Imath::V2f( p.x, -( p.y - cy) + cy);
}

// user interface
const char *ofx_node_t::help_string() const { return 0;}

} // namespace
} // namespace
