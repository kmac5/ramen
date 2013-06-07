// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/dialogs/apply_corner_track_dialog.hpp>

#include<vector>

#include<boost/foreach.hpp>

#include<QMessageBox>

#include<ramen/assert.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/nodes/image/track/tracker_node.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>

namespace ramen
{
namespace ui
{

apply_corner_track_dialog_t& apply_corner_track_dialog_t::instance()
{
    static apply_corner_track_dialog_t dialog;
    return dialog;
}

apply_corner_track_dialog_t::apply_corner_track_dialog_t() : QDialog( app().ui()->main_window())
{
    setWindowTitle( "Apply Track");
	ui_.setupUi( this);
}

const image::tracker_node_t *apply_corner_track_dialog_t::exec()
{
	std::vector<const image::tracker_node_t*> trackers;
	ui_.tracker_combo_->clear();
	
	BOOST_FOREACH( const node_t& n, app().document().composition().nodes())
	{
		const image::tracker_node_t *t = dynamic_cast<const image::tracker_node_t*>( &n);
		
		if( t)
		{
			if( !t->ignored())
			{
				ui_.tracker_combo_->addItem( QString::fromStdString( n.name()));
				trackers.push_back( t);
			}
		}
	}

	if( ui_.tracker_combo_->count() == 0)
	{
        QMessageBox::warning( app().ui()->main_window(), "Ramen",
							  "No active tracker nodes in composition");
		return 0;
	}

	int result = QDialog::exec();
	
	if( result == QDialog::Rejected)
		return 0;

	return trackers[ ui_.tracker_combo_->currentIndex()];
}

} // namespace
} // namespace
