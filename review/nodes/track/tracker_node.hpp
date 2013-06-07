// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_TRACKER_NODE_HPP
#define	RAMEN_IMAGE_TRACKER_NODE_HPP

#include<ramen/nodes/image/track/tracker_node_fwd.hpp>

#include<ramen/nodes/image_node.hpp>

#include<vector>

#include<boost/optional.hpp>

#include<ramen/params/param_fwd.hpp>
#include<ramen/nodes/image/track/ncc_tracker_fwd.hpp>
#include<ramen/nodes/image/track/tracker_toolbar_fwd.hpp>

namespace ramen
{
namespace image
{

class tracker_node_t : public image_node_t
{
public:

    static const node_metaclass_t& tracker_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    tracker_node_t();
	~tracker_node_t();

    virtual std::auto_ptr<QWidget> create_toolbar();
		
	int num_trackers() const;
	
	const std::vector<track::ncc_tracker_t*>& trackers() const	{ return trackers_;}
	std::vector<track::ncc_tracker_t*>& trackers()				{ return trackers_;}
	
	bool any_tracker_active() const;
	
	// apply track
	boost::optional<Imath::V2f> tracker_pos( int index, float frame) const;
	
	void apply_track( float start_frame, float end_frame, 
					  apply_track_mode mode, apply_track_use use, const Imath::V2f& center,
					  float2_param_t *trans, float_param_t *rot, float2_param_t *scale) const;

protected:

    tracker_node_t( const tracker_node_t& other);
    void operator=( const tracker_node_t&);

private:

	friend class ramen::tracker_toolbar_t;
	
    virtual node_t *do_clone() const { return new tracker_node_t( *this);}
	
	virtual void do_connected( node_t *src, int port);
	void input_changed( node_t *n);
	
	virtual void do_create_params();
    void param_changed( param_t *p, param_t::change_reason reason);
	
	virtual void do_create_manipulators();

	virtual bool do_is_identity() const;

	virtual void do_begin_active();
    virtual void do_end_active();
	
	void create_trackers();

	// references
	void grab_references();
	
	// tracking	
	void start_tracking();
	void end_tracking();
	
	void track_forward();
	void track_backwards();
	void track_one_forward();
	void track_one_backwards();
	
	void do_track_one( float next_frame);
	
	image::buffer_t get_input_frame( float frame);
	
	// apply track
	float angle_between_vectors( const Imath::V2f& v0, const Imath::V2f& v1) const;
	void get_trs( float frame, Imath::V2f& t, float& r, float& s) const;
	
	boost::signals2::connection input_connection_;
	
	std::vector<track::ncc_tracker_t*> trackers_;
	bool tracking_;
};

} // namespace
} // namespace

#endif
