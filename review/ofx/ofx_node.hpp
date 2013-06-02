// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_OFX_NODE_HPP
#define	RAMEN_IMAGE_OFX_NODE_HPP

#include<ramen/nodes/image_node.hpp>

#include<boost/signal.hpp>

#include"ofxCore.h"

#include<ramen/ofx/host_support_fwd.hpp>
#include<ramen/ofx/image_effect.hpp>

namespace ramen
{

class composite_param_t;

namespace image
{

class ofx_node_t : public image_node_t
{
public:
	
    ofx_node_t( OFX::Host::ImageEffect::ImageEffectPlugin *plugin);
    ~ofx_node_t();

	// inputs & clips
	int index_for_clip_name( const std::string& name) const;
	bool all_inputs_optional() const { return all_inputs_optional_;}

	// params
	void block_param_changed_signal( bool b);
	
    const render::context_t& render_context() const { return render_context_;}
	
    // util
    Imath::Box2i vertical_flip( const Imath::Box2i& b) const;
	Imath::V2f vertical_flip( const Imath::V2f& p, int subsample = 1) const;

    // user interface
    virtual const char *help_string() const;

protected:

    ofx_node_t( const ofx_node_t& other);
    void operator=( const ofx_node_t& other);

private:

    virtual node_t *do_clone() const;

    void create_effect_instance();
	
    // params
    virtual void do_create_params();
	void param_changed( param_t *p, param_t::change_reason reason);
	composite_param_t *find_param( const std::string& name);

	// overlay
    virtual void do_create_manipulators();

    void do_connected( node_t *src, int port);
	
	// rendering
	virtual bool do_is_valid() const;
	bool is_frame_varying() const;
	
	void do_calc_bounds( const render::context_t& context);
	void do_calc_inputs_interest( const render::context_t& context);
	void do_calc_defined( const render::context_t& context);
	
    // processing
    virtual void do_recursive_process( const render::context_t& context);
    virtual void do_process( const render::context_t& context);
	
	void get_descriptor();
	
    std::string id_;
    int major_version_;
    int minor_version_;
    std::string ofx_context_;
	bool all_inputs_optional_;

    OFX::Host::ImageEffect::ImageEffectPlugin *plugin_;
    OFX::Host::ImageEffect::Descriptor *descriptor_;

	ofx::image_effect_t *effect_;

	render::context_t render_context_;

	boost::signals::connection param_changed_connection_;
};

} // namespace
} // namespace

#endif
