// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_EDITOR_HPP
#define	RAMEN_UI_ANIM_EDITOR_HPP

#include<ramen/ui/anim/anim_editor_fwd.hpp>

#include<set>
#include<memory>

#include<QObject>

// avoid a moc error, when including boost
#ifndef QT_MOC_RUN
    #include<boost/shared_ptr.hpp>

    #include<ramen/nodes/node_fwd.hpp>

    #include<ramen/anim/track_fwd.hpp>
    #include<ramen/anim/keyframe.hpp>
    #include<ramen/anim/any_curve.hpp>

    #include<ramen/ui/anim/track_model_fwd.hpp>
    #include<ramen/ui/anim/anim_editor_command.hpp>
    #include<ramen/ui/anim/anim_editor_toolbar_fwd.hpp>
    #include<ramen/ui/anim/anim_curves_view_fwd.hpp>
#endif

class QWidget;
class QSplitter;
class QTreeView;
class QItemSelection;
class QAction;

namespace ramen
{
namespace ui
{

class anim_editor_t : public QObject
{
    Q_OBJECT

public:

    anim_editor_t();

    void set_active_node( node_t *n);
	void clear_all();

    // tracks
	boost::shared_ptr<track_model_t> track_model() { return current_;}
    void recreate_tracks( node_t *n);
	
    const std::set<anim::track_t*>& active_tracks() const	{ return active_tracks_;}
    std::set<anim::track_t*>& active_tracks()				{ return active_tracks_;}

	void node_renamed( node_t *n);
	
    void update();

    // selection
    bool any_keyframe_selected() const;
	bool any_float_keyframe_selected() const;
	bool any_keyframe_selected( const anim::any_curve_ptr_t& c) const;
    void deselect_all();
	void get_selected_keyframe( anim::track_t*& track, int& key_index);

    // undo
    void create_command();
	void set_command( undo::anim_editor_command_t *command);
    undo::anim_editor_command_t *command() { return command_.get();}
    void push_command();
    void clear_command();

	// edit
	void delete_selected_keyframes();
	void set_autotangents( anim::keyframe_t::auto_tangent_method m, bool selected_only);
	void set_extrapolation( anim::extrapolation_method m);
	
	// operations
	void reverse_keyframes();
	void negate_keyframes();
	void sample_keyframes();
	void smooth_keyframes( float filter_size, bool resample);
	void high_pass_keyframes( float filter_size, bool resample);
	
    QWidget *widget()                   { return window_;}
    anim_curves_view_t& curves_view()   { return *view_;}
    anim_editor_toolbar_t& toolbar()    { return *toolbar_;}

private Q_SLOTS:

    void update_selection( const QItemSelection& selected, const QItemSelection& deselected);
	
	void item_collapsed( const QModelIndex & index);
	void item_expanded( const QModelIndex & index);
	
	void show_context_menu( const QPoint& p);

	void copy_curves();
	void copy_keyframes();
	void paste();
	
	void select_all();

	void set_tangents_smooth();
	void set_tangents_flat();
	void set_tangents_linear();
	void set_tangents_step();
	
	void set_extrapolation_constant();
	void set_extrapolation_linear();
	void set_extrapolation_repeat();

	void import_curves();
	void export_curves();

private:

    void insert_in_active_tracks( anim::track_t *t);

    boost::shared_ptr<track_model_t> current_;
    boost::shared_ptr<track_model_t> default_model_;

    std::set<anim::track_t*> active_tracks_;

    std::auto_ptr<undo::anim_editor_command_t> command_;
	
	QWidget *window_;
	
	anim_editor_toolbar_t *toolbar_;
	QSplitter *split_;
	QTreeView *tree_;
	anim_curves_view_t *view_;
		
	QAction *copy_curves_, *copy_keys_, *paste_;
	QAction *select_all_;
	QAction *const_extrap_, *lin_extrap_, *repeat_extrap_;
	QAction *import_, *export_;
};

} // namespace
} // namespace

#endif
