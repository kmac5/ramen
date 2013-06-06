// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_APPLY_TRACK_DIALOG_HPP
#define	RAMEN_APPLY_TRACK_DIALOG_HPP



#include<QDialog>

#include"ui_apply_track.h"

#include<ramen/nodes/image/track/tracker_node_fwd.hpp>

namespace ramen
{
namespace ui
{
	
class apply_track_dialog_t : public QDialog
{
    Q_OBJECT
    
public:

    static apply_track_dialog_t& instance();

	const image::tracker_node_t *exec( image::apply_track_mode& mode, image::apply_track_use& use);
	
private:

    apply_track_dialog_t();

	Ui::apply_track ui_;
};

} // namespace
} // namespace

#endif
