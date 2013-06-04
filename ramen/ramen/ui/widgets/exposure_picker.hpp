// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_WIDGETS_EXPOSURE_PICKER_HPP
#define	RAMEN_UI_WIDGETS_EXPOSURE_PICKER_HPP

#include<QWidget>
#include<QImage>

namespace ramen
{
namespace ui
{

class exposure_picker_t : public QWidget
{
    Q_OBJECT

public:

    exposure_picker_t( QWidget *parent = 0);

    QSize sizeHint() const;

    static double max_exposure();
    double exposure() const;

Q_SIGNALS:

    void exposure_changed( double e);

public Q_SLOTS:

    void set_exposure( double e);

protected:

    void paintEvent( QPaintEvent *event);
    void mouseMoveEvent( QMouseEvent *event);
    void mousePressEvent( QMouseEvent *event);
    void mouseReleaseEvent( QMouseEvent *event);
    void resizeEvent( QResizeEvent *event);
    
private:

    double pick_exposure( int x) const;

    void update_background();

    QImage background_;
    bool valid_background_;
    double exposure_;
};

} // ui
} // ramen

#endif
