// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_TRACKER_TOOLBAR_HPP
#define	RAMEN_IMAGE_TRACKER_TOOLBAR_HPP

#include<ramen/nodes/image/track/tracker_toolbar_fwd.hpp>

#include<memory>

#include<QWidget>

#include<ramen/nodes/image/track/tracker_node_fwd.hpp>

namespace ramen
{

class tracker_toolbar_t : public QWidget
{
    Q_OBJECT
    
public:

    tracker_toolbar_t( image::tracker_node_t& parent);

private Q_SLOTS:
	
	void grab_references();
	
	void track_back();
	void track_fwd();
	void track_one_back();
	void track_one_fwd();
	
private:

    image::tracker_node_t& parent_;
};

} // namespace

#endif
