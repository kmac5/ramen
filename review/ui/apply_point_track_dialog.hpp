// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_APPLY_POINT_TRACK_DIALOG_HPP
#define	RAMEN_APPLY_POINT_TRACK_DIALOG_HPP



#include<QDialog>

#include"ui_apply_point_track.h"

#include<ramen/nodes/image/track/tracker_node_fwd.hpp>

namespace ramen
{
namespace ui
{
	
class apply_point_track_dialog_t : public QDialog
{
    Q_OBJECT
    
public:

    static apply_point_track_dialog_t& instance();

	const image::tracker_node_t *exec( int& track_num);
	
private:
	
    apply_point_track_dialog_t();

	Ui::apply_point_track ui_;
};

} // namespace
} // namespace

#endif
