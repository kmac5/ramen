// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_EDITOR_TOOLBAR_HPP
#define	RAMEN_UI_ANIM_EDITOR_TOOLBAR_HPP

#include<QWidget>

// avoid a moc error, when including boost
#ifndef QT_MOC_RUN
    #include<ramen/ui/anim/anim_editor_toolbar_fwd.hpp>
    #include<ramen/ui/anim/anim_editor_tool.hpp>
    #include<ramen/ui/widgets/double_spinbox_fwd.hpp>
    #include<ramen/ui/anim/drag_keys_command.hpp>
#endif

class QButtonGroup;
class QToolButton;
class QPushButton;
class QMenu;
class QAction;

namespace ramen
{
namespace ui
{

class anim_editor_toolbar_t : public QWidget
{
    Q_OBJECT

public:
	
    anim_editor_toolbar_t( QWidget *parent = 0);
    ~anim_editor_toolbar_t();

    anim_editor_tool_t *tool();
	
	void selection_changed();
		
private Q_SLOTS:

	void set_tangents_smooth();
	void set_tangents_flat();
	void set_tangents_linear();
	void set_tangents_step();
	
    void value_changed( double value);
    void spinbox_pressed();
    void spinbox_dragged( double value);
    void spinbox_released();
	
	void reverse_selected_keys();
	void negate_selected_keys();
	void sample_selected_keys();
	void smooth_selected_keys();
	void high_pass_selected_keys();
	
private:

    QButtonGroup *bgroup_;
	
	QToolButton *select_;
	QToolButton *scale_;
	
	QToolButton *smooth_tangents_;
	QToolButton *flat_tangents_;
	QToolButton *linear_tangents_;
	QToolButton *step_tangents_;
	
	double_spinbox_t *time_, *val_;

	QPushButton *ops_button_;
	QMenu *ops_menu_;
	QAction *reverse_, *negate_, *smooth_;
	QAction *high_pass_, *sample_;
	
    anim_editor_tool_t *select_tool_;
	
	anim::track_t *selected_track_;
	int selected_key_index_;
	undo::drag_keys_command_t *drag_command_;	
};

} // namespace
} // namespace

#endif
