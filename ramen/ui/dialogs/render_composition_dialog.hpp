// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RENDER_COMPOSITION_DIALOG_HPP
#define	RAMEN_RENDER_COMPOSITION_DIALOG_HPP

#include<QDialog>

#include"ui_render_composition.h"

namespace ramen
{
namespace ui
{

class render_composition_dialog_t : public QDialog
{
    Q_OBJECT
    
public:

    static render_composition_dialog_t& instance();

    int start_frame() const;
    int end_frame() const;
	int proxy_level() const;	
    int resolution() const;
	
    bool selected_only() const;
    int mblur_extra_samples() const;
    float mblur_shutter_factor() const;

    void set_frame_range( int lo, int hi);
    void set_any_output_selected( bool b);
    void set_mblur_settings( int samples, float factor);

private:

    render_composition_dialog_t();

	Ui::render_composition ui_;
};

} // namespace
} // namespace

#endif
