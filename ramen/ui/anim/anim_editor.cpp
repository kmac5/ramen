// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/anim_editor.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<QHBoxLayout>
#include<QSplitter>
#include<QTreeView>
#include<QComboBox>
#include<QAction>
#include<QMenu>
#include<QCursor>

#include<ramen/nodes/node.hpp>

#include<ramen/anim/track.hpp>
#include<ramen/anim/float_curve.hpp>
#include<ramen/anim/clipboard.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/track_model.hpp>
#include<ramen/ui/anim/anim_curves_view.hpp>
#include<ramen/ui/anim/anim_editor_toolbar.hpp>
#include<ramen/ui/anim/select_keys_visitors.hpp>
#include<ramen/ui/anim/edit_keys_visitors.hpp>
#include<ramen/ui/anim/edit_tangents_visitors.hpp>

#ifndef NDEBUG
	#include<iostream>
#endif

namespace ramen
{
namespace ui
{

namespace
{

struct track_model_deleter
{
    void operator()( track_model_t *m) const { m->deleteLater();}
};

#ifndef NDEBUG
	void print_full_name( anim::track_t *t) { std::cout << t->full_name() << " -> " << t->curve_name() << "\n";}
#endif

} // unnamed

anim_editor_t::anim_editor_t() : window_( 0)
{
    window_ = new QWidget();
    window_->setWindowTitle( "Curve editor");

    QVBoxLayout *vlayout = new QVBoxLayout();

    vlayout->setContentsMargins( 0, 0, 0, 0);

    toolbar_ = new anim_editor_toolbar_t();
    vlayout->addWidget( toolbar_);

    split_ = new QSplitter();

    tree_ = new QTreeView();
	tree_->setSelectionMode( QAbstractItemView::ExtendedSelection);
	tree_->setContextMenuPolicy( Qt::CustomContextMenu);
    tree_->setModel( 0);
	connect( tree_, SIGNAL( customContextMenuRequested( const QPoint&)), this, SLOT( show_context_menu( QPoint)));
	connect( tree_, SIGNAL( expanded( QModelIndex)), this, SLOT( item_expanded( QModelIndex)));
	connect( tree_, SIGNAL( collapsed( QModelIndex)), this, SLOT( item_collapsed( QModelIndex)));
    split_->addWidget( tree_);

    view_ = new anim_curves_view_t();
    split_->addWidget( view_);

    vlayout->addWidget( split_);

    default_model_.reset( new track_model_t(), track_model_deleter());
    tree_->setModel( default_model_.get());

    window_->setLayout( vlayout);

	// actions
	copy_keys_ = new QAction( "Copy Keyframes", window_);
	copy_keys_->setShortcut( QKeySequence( "Ctrl+C"));
	copy_keys_->setShortcutContext( Qt::WidgetWithChildrenShortcut);
	window_->addAction( copy_keys_);
	connect( copy_keys_, SIGNAL( triggered()), this, SLOT( copy_keyframes()));

	copy_curves_ = new QAction( "Copy Curves", window_);
	copy_curves_->setShortcut( QKeySequence( "Ctrl+Shift+C"));
	copy_curves_->setShortcutContext( Qt::WidgetWithChildrenShortcut);
	window_->addAction( copy_curves_);
	connect( copy_curves_, SIGNAL( triggered()), this, SLOT( copy_curves()));
	
	paste_ = new QAction( "Paste", this);
	paste_->setShortcut( QKeySequence( "Ctrl+V"));
	paste_->setShortcutContext( Qt::WidgetWithChildrenShortcut);
	window_->addAction( paste_);
	connect( paste_, SIGNAL( triggered()), this, SLOT( paste()));
	
	select_all_ = new QAction( "Select all", window_);
	select_all_->setShortcut( QKeySequence( "Ctrl+A"));
	select_all_->setShortcutContext( Qt::WidgetWithChildrenShortcut);
	window_->addAction( select_all_);
	connect( select_all_, SIGNAL( triggered()), this, SLOT( select_all()));
		
	const_extrap_ = new QAction( "Constant", this);
	connect( const_extrap_, SIGNAL( triggered()), this, SLOT( set_extrapolation_constant()));
	
	lin_extrap_ = new QAction( "Linear", this);
	connect( lin_extrap_, SIGNAL( triggered()), this, SLOT( set_extrapolation_linear()));

	repeat_extrap_ = new QAction( "Repeat", this);
	connect( repeat_extrap_, SIGNAL( triggered()), this, SLOT( set_extrapolation_repeat()));
	
	import_ = new QAction( "Import...", this);
	connect( import_, SIGNAL( triggered()), this, SLOT( import_curves()));
	
	export_ = new QAction( "Export...", this);
	connect( import_, SIGNAL( triggered()), this, SLOT( export_curves()));
}

void anim_editor_t::set_active_node( node_t *n)
{
    boost::shared_ptr<track_model_t> old = current_;
    QItemSelectionModel *sel_model = tree_->selectionModel();

    active_tracks_.clear();

    if( n != 0)
    {
        current_.reset( new track_model_t(), track_model_deleter());
        n->create_tracks( current_->root_track());
		current_->node_track()->make_full_names();
		
		#ifndef NDEBUG
			//anim::for_each_leaf_track( current_->root_track(), print_full_name);
		#endif

        tree_->setModel( current_.get());

        QItemSelectionModel *new_sel = tree_->selectionModel();
        connect( new_sel, SIGNAL( selectionChanged( const QItemSelection&, const QItemSelection&)),
		this, SLOT( update_selection( const QItemSelection&, const QItemSelection&)));
    }
    else
    {
        tree_->setModel( default_model_.get());
        current_.reset();
    }

    delete sel_model;

    if( !app().quitting())
	{
		toolbar().selection_changed();
        view_->update();
	}
}

void anim_editor_t::node_renamed( node_t *n)
{
	current_->node_track()->set_name( n->name());
	update();
}

void anim_editor_t::clear_all()
{
}

void anim_editor_t::recreate_tracks( node_t *n)
{
    if( n == app().ui()->active_node())
    {
        set_active_node( 0);
        set_active_node( n);
    }
}

void anim_editor_t::update_selection( const QItemSelection& selected, const QItemSelection& deselected)
{
    active_tracks_.clear();

    QItemSelectionModel *selection = tree_->selectionModel();
    QModelIndexList indexes = selection->selectedIndexes();

    if( indexes.empty())
        return;

	for( int i = 0; i < indexes.size(); ++i)
	{
		anim::track_t *track = static_cast<anim::track_t*>( indexes[i].internalPointer());

		if( track)
			anim::for_each_leaf_track( track, boost::bind( &anim_editor_t::insert_in_active_tracks, this, _1));
	}
	
	toolbar().selection_changed();
    view_->update();
}

void anim_editor_t::item_collapsed( const QModelIndex & index) {}
void anim_editor_t::item_expanded( const QModelIndex & index)  {}

void anim_editor_t::show_context_menu( const QPoint& p)
{
	if( active_tracks_.empty())
		return;

	// bool any_key = any_keyframe_selected();
	// copy_keys_->setEnabled( any_key);
	
	paste_->setEnabled( !anim::clipboard_t::instance().empty());
	import_->setEnabled( false);
	export_->setEnabled( false);

	QMenu menu( tree_);
	menu.addAction( copy_keys_);
	menu.addAction( copy_curves_);
	menu.addAction( paste_);
	menu.addSeparator();
	
	QMenu ext_menu( "Extrapolation", tree_);
	ext_menu.addAction( const_extrap_);
	ext_menu.addAction( lin_extrap_);
	ext_menu.addAction( repeat_extrap_);
	menu.addMenu( &ext_menu);
	
	menu.addSeparator();
	menu.addAction( import_);
	menu.addAction( export_);

	menu.exec( QCursor::pos());
}

void anim_editor_t::update()
{
    if( !app().quitting())
    {
        // update controls here.
        tree_->update();
        view_->update();
    }
}

void anim_editor_t::insert_in_active_tracks( anim::track_t *t)
{
    if( t->curve())
        active_tracks_.insert( t);
}

bool anim_editor_t::any_keyframe_selected() const
{
    any_selected_visitor v;
    BOOST_FOREACH( const anim::track_t *t, active_tracks())
    {
        boost::apply_visitor( v, t->curve().get());
		if( v.any_selected)
			return true;
    }

    return false;
}

bool anim_editor_t::any_float_keyframe_selected() const
{
    any_selected_visitor v;
    BOOST_FOREACH( const anim::track_t *t, active_tracks())
    {
		const anim::any_curve_ptr_t& c( t->curve().get());
		
		if( c.which() == 0)
		{
	        boost::apply_visitor( v, c);
			if( v.any_selected)
				return true;
		}
    }

    return false;
}

bool anim_editor_t::any_keyframe_selected( const anim::any_curve_ptr_t& c) const
{
    any_selected_visitor v;
	boost::apply_visitor( v, c);
	return v.any_selected;
}

void anim_editor_t::deselect_all()
{
    deselect_all_visitor v;
    BOOST_FOREACH( const anim::track_t *t, active_tracks())
        boost::apply_visitor( v, t->curve().get());
}

void anim_editor_t::get_selected_keyframe( anim::track_t*& track, int& key_index)
{
	track = 0;
	key_index = -1;
	
    BOOST_FOREACH( anim::track_t *t, active_tracks())
	{
		count_selected_visitor v;
		boost::apply_visitor( v, t->curve().get());
		
		if( v.count == 1)
		{
			if( !track)
				track = t;
			else
			{
				track = 0;
				return;
			}
		}
	}
	
	if( track != 0)
	{
		get_selected_keyframe_visitor v;
		boost::apply_visitor( v, track->curve().get());
		
		if( v.index != -1)
			key_index = v.index;
		else
			track = 0;
	}
}

// undo
void anim_editor_t::create_command()
{
	node_t *n = app().ui()->active_node();
	RAMEN_ASSERT( n);

    command_.reset( new undo::anim_editor_command_t( n, current_));
}

void anim_editor_t::set_command( undo::anim_editor_command_t *command)
{
	RAMEN_ASSERT( command_.get() == 0);
	RAMEN_ASSERT( app().ui()->active_node());

	command_.reset( command);
}

void anim_editor_t::push_command()
{
    if( command_.get())
	{
		if( !command_->empty())
		{
			app().document().undo_stack().push_back( command_);
			app().ui()->update();
		}
		else
			clear_command();
	}
}

void anim_editor_t::clear_command() { command_.reset();}

void anim_editor_t::delete_selected_keyframes()
{
	if( app().ui()->anim_editor().any_keyframe_selected())
	{
		create_command();
		
		delete_selected_keys_visitor v;
		BOOST_FOREACH( anim::track_t *t, active_tracks())
		{
			anim::any_curve_ptr_t& c( t->curve().get());

			if( any_keyframe_selected( c))
			{
				command()->add_track( t);
				boost::apply_visitor( v, c);
			}
		}

		command()->call_notify_for_tracks();
		command()->notify_nodes();
		push_command();
		toolbar().selection_changed();
		app().ui()->update();
		update();
	}
}

void anim_editor_t::set_autotangents( anim::keyframe_t::auto_tangent_method m, bool selected_only)
{
	if( selected_only && !app().ui()->anim_editor().any_keyframe_selected())
		return;

	create_command();

	can_set_autotangents_visitor v0( m);
	set_autotangents_visitor v1( m);
		
	BOOST_FOREACH( anim::track_t *t, active_tracks())
	{
		anim::any_curve_ptr_t& c( t->curve().get());

		if( selected_only && !any_keyframe_selected( c))
			continue;

		boost::apply_visitor( v0, c);
			
		if( v0.result)
		{
			command()->add_track( t);
			boost::apply_visitor( v1, c);
		}
	}

	command()->call_notify_for_tracks();
	command()->notify_nodes();		
	push_command();
	app().ui()->update();
	update();
}

void anim_editor_t::set_extrapolation( anim::extrapolation_method m)
{
	create_command();

	can_set_extrapolation_visitor v0( m);
	set_extrapolation_visitor v1( m);

	BOOST_FOREACH( anim::track_t *t, active_tracks())
	{
		anim::any_curve_ptr_t& c( t->curve().get());

		boost::apply_visitor( v0, c);
			
		if( v0.result)
		{
			command()->add_track( t);
			boost::apply_visitor( v1, c);
		}
	}

	command()->call_notify_for_tracks();
	command()->notify_nodes();
	push_command();
	app().ui()->update();
	update();
}

void anim_editor_t::reverse_keyframes()
{
	create_command();

	BOOST_FOREACH( anim::track_t *t, active_tracks())
	{
		anim::any_curve_ptr_t& c( t->curve().get());
		
		if( any_keyframe_selected( c))
		{
			reverse_selected_keys_visitor v;
			command()->add_track( t);
			boost::apply_visitor( v, c);
		}
	}

	command()->call_notify_for_tracks();
	command()->notify_nodes();
	push_command();
	app().ui()->update();
	update();
}

void anim_editor_t::negate_keyframes()
{
	create_command();

	BOOST_FOREACH( anim::track_t *t, active_tracks())
	{
		anim::any_curve_ptr_t& c( t->curve().get());
		
		if( c.which() == 0 && any_keyframe_selected( c))
		{
			negate_selected_keys_visitor v;
			command()->add_track( t);
			boost::apply_visitor( v, c);
		}
	}

	command()->call_notify_for_tracks();
	command()->notify_nodes();
	push_command();
	app().ui()->update();
	update();
}

void anim_editor_t::sample_keyframes()
{
	create_command();

	BOOST_FOREACH( anim::track_t *t, active_tracks())
	{
		anim::any_curve_ptr_t& c( t->curve().get());
		
		if( c.which() == 0 && any_keyframe_selected( c))
		{
			sample_selected_keys_visitor v;
			command()->add_track( t);
			boost::apply_visitor( v, c);
		}
	}

	command()->call_notify_for_tracks();
	command()->notify_nodes();
	push_command();
	app().ui()->update();
	update();
}

void anim_editor_t::smooth_keyframes( float filter_size, bool resample)
{
	create_command();

	BOOST_FOREACH( anim::track_t *t, active_tracks())
	{
		anim::any_curve_ptr_t& c( t->curve().get());
		
		if( c.which() == 0 && any_keyframe_selected( c))
		{
			smooth_selected_keys_visitor v( filter_size, resample);
			command()->add_track( t);
			boost::apply_visitor( v, c);
		}
	}

	command()->call_notify_for_tracks();
	command()->notify_nodes();
	push_command();
	app().ui()->update();
	update();
}

void anim_editor_t::high_pass_keyframes( float filter_size, bool resample)
{
	create_command();

	BOOST_FOREACH( anim::track_t *t, active_tracks())
	{
		anim::any_curve_ptr_t& c( t->curve().get());
		
		if( c.which() == 0 && any_keyframe_selected( c))
		{
			high_pass_selected_keys_visitor v( filter_size, resample);
			command()->add_track( t);
			boost::apply_visitor( v, c);
		}
	}

	command()->call_notify_for_tracks();
	command()->notify_nodes();
	push_command();
	app().ui()->update();
	update();
}

// slots

void anim_editor_t::set_tangents_smooth()
{
	set_autotangents( anim::keyframe_t::tangent_smooth, true);
}

void anim_editor_t::set_tangents_flat()
{
	set_autotangents( anim::keyframe_t::tangent_flat, true);
}

void anim_editor_t::set_tangents_linear()
{
	set_autotangents( anim::keyframe_t::tangent_linear, true);
}

void anim_editor_t::set_tangents_step()
{
	set_autotangents( anim::keyframe_t::tangent_step, true);
}

void anim_editor_t::copy_curves()
{
	if( active_tracks().empty())
		return;
	
	anim::clipboard_t::instance().begin_copy();
	
	BOOST_FOREACH( anim::track_t *t, active_tracks())
	{
		anim::clipboard_t::instance().copy_curve( t->curve_name(), t->curve().get());
	}

	anim::clipboard_t::instance().end_copy();
}

void anim_editor_t::copy_keyframes()
{
	if( active_tracks().empty() || !any_keyframe_selected())
		return;

	anim::clipboard_t::instance().begin_copy();
	
	BOOST_FOREACH( anim::track_t *t, active_tracks())
	{
		anim::clipboard_t::instance().copy_keys( t->curve_name(), t->curve().get());
	}

	anim::clipboard_t::instance().end_copy();
}

void anim_editor_t::paste()
{
	if( active_tracks().empty() || anim::clipboard_t::instance().empty())
		return;
	
	create_command();

	float frame = app().document().composition().frame();
	
	BOOST_FOREACH( anim::track_t *t, active_tracks())
	{
		anim::any_curve_ptr_t& c( t->curve().get());
			
		if( anim::clipboard_t::instance().can_paste( t->curve_name(), c))
		{
			command()->add_track( t);
			anim::clipboard_t::instance().paste( t->curve_name(), c, frame);
		}
	}

	command()->call_notify_for_tracks();
	command()->notify_nodes();
	push_command();
	app().ui()->update();
	update();
}

void anim_editor_t::select_all()
{
    select_all_visitor v;
    BOOST_FOREACH( const anim::track_t *t, active_tracks())
        boost::apply_visitor( v, t->curve().get());

	toolbar().selection_changed();
	update();	
}

void anim_editor_t::set_extrapolation_constant()
{
	set_extrapolation( anim::extrapolate_constant);
}

void anim_editor_t::set_extrapolation_linear()
{
	set_extrapolation( anim::extrapolate_linear);
}

void anim_editor_t::set_extrapolation_repeat()
{
	set_extrapolation( anim::extrapolate_repeat);
}

void anim_editor_t::import_curves() {}
void anim_editor_t::export_curves() {}

} // namespace
} // namespace
