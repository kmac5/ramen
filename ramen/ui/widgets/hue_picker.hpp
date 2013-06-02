// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_WIDGETS_HUE_PICKER_HPP
#define	RAMEN_UI_WIDGETS_HUE_PICKER_HPP

#include<QWidget>
#include<QImage>

namespace ramen
{
namespace ui
{

class hue_picker_t : public QWidget
{
    Q_OBJECT

public:

    hue_picker_t( QWidget *parent = 0);

    QSize sizeHint() const;

    double hue() const;

Q_SIGNALS:

    void hue_changed( double hue);

public Q_SLOTS:

    void set_hue( double h);

protected:

    void paintEvent( QPaintEvent *event);
    void mouseMoveEvent( QMouseEvent *event);
    void mousePressEvent( QMouseEvent *event);
    void mouseReleaseEvent( QMouseEvent *event);
    void resizeEvent( QResizeEvent *event);
    
private:

    double pick_hue( int y) const;

    void update_background();

    QImage background_;
    bool valid_background_;

    double hue_;
};

} // ui
} // ramen

#endif
