// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_KEYER3D_NODE_HPP
#define RAMEN_IMAGE_KEYER3D_NODE_HPP

#include<ramen/nodes/image/key/keyer3d/keyer3d_node_fwd.hpp>

#include<ramen/nodes/image/keyer_node.hpp>

#include<ramen/nodes/image/key/keyer3d/toolbar_fwd.hpp>

#include<ramen/nodes/image/key/keyer3d/km_keyer.hpp>

#include<ramen/params/color_param.hpp>
#include<ramen/params/float_param.hpp>

namespace ramen
{
namespace image
{

class keyer3d_node_t : public keyer_node_t
{
public:

    static const node_metaclass_t& keyer3d_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    keyer3d_node_t();

    virtual std::auto_ptr<QWidget> create_toolbar();
	const keyer3d::toolbar_t& toolbar() const	{ return *toolbar_;}
	keyer3d::toolbar_t& toolbar()				{ return *toolbar_;}

	const keyer3d::km_keyer_t& keyer() const	{ return keyer_;}
	keyer3d::km_keyer_t& keyer()				{ return keyer_;}
	
	bool add_tolerance( const Imath::Color3f& col);
	bool add_tolerance( const std::vector<Imath::Color3f>& s);

	bool remove_tolerance( const Imath::Color3f& col);
	bool remove_tolerance( const std::vector<Imath::Color3f>& s);
	
	void add_softness( const Imath::Color3f& col, float amount);

	void update_clusters_params();
	
protected:

    keyer3d_node_t( const keyer3d_node_t& other);
    void operator=( const keyer3d_node_t&);

private:

    node_t *do_clone() const { return new keyer3d_node_t(*this);}

    virtual void do_create_params();
	
	virtual void do_create_manipulators();

    virtual void do_connected( node_t *src, int port);
	
	virtual void do_begin_active();
	virtual void do_end_active();

	virtual void do_calc_hash_str( const render::context_t& context);
	
    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_defined( const render::context_t& context);
	
    virtual void do_process( const render::context_t& context);
		
	virtual void do_read( const serialization::yaml_node_t& node, const std::pair<int,int>& version);	
	virtual void do_write( serialization::yaml_oarchive_t& out) const;	
	
	void create_cluster_params( int i);
		
	keyer3d::toolbar_t *toolbar_;

	static const int max_clusters = 5;
	
	keyer3d::km_keyer_t keyer_;
	
	#ifndef NDEBUG
		float_param_t *knum_;
		color_param_t *kcenter_[ max_clusters];
		float_param_t *ktol_[ max_clusters];
		float_param_t *ksoft_[ max_clusters];
	#endif
};

} // namespace
} // namespace

#endif
