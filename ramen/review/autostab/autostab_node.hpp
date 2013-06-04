// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_AUTOSTAB_NODE_HPP
#define RAMEN_IMAGE_AUTOSTAB_NODE_HPP

#include<ramen/nodes/image/track/cv_autoalign_node.hpp>

namespace ramen
{
namespace image
{

class autostab_node_t : public cv_autoalign_node_t
{
public:

    static const node_metaclass_t& autostab_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    autostab_node_t();

    virtual void cloned();
	
protected:

    autostab_node_t( const autostab_node_t& other);
    void operator=( const autostab_node_t&);

private:

    node_t *do_clone() const { return new autostab_node_t( *this);}

    virtual void do_create_params();
    virtual void do_create_manipulators();

    void param_changed( param_t *p);

	virtual void do_end_active();
	
	void track();
	void align_points( int frame, const std::vector<cv::KeyPoint>& ref_pts, const std::vector<cv::KeyPoint>& pts, 
						const std::vector<cv::DMatch>& matches);
	
    virtual Imath::M33d do_calc_transform_matrix_at_frame( float frame, int subsample = 1) const;
};

} // namespace
} // namespace

#endif
