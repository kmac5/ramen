// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_TIME_CONTROLS_HPP
#define	RAMEN_UI_TIME_CONTROLS_HPP

#include<ramen/ui/time_controls_fwd.hpp>

#include<map>

#include<boost/noncopyable.hpp>
#include<boost/signal.hpp>

#include<QWidget>

class QToolButton;
class QPushButton;

namespace ramen
{
namespace ui
{

class time_controls_t : public QObject
{
    Q_OBJECT

public:

    time_controls_t();

    QWidget *widget() { return window_;}

    int width() const;
    int height() const;

    bool eventFilter( QObject *watched, QEvent *event);

    void update();

public Q_SLOTS:

    void goto_start();
    void prev_frame();
    void prev_key();
    void play_back();

    void play_fwd();
    void next_key();
    void next_frame();
    void goto_end();

    void make_flipbook();
	void set_autokey( bool b);

private:

    void stop_playing();

    QWidget *window_;

    QToolButton *start_, *prev_key_, *prev_frame_, *play_back_;
    QToolButton *stop_, *play_fwd_, *next_frame_, *next_key_, *end_;

    QToolButton *autokey_;
    QPushButton *flipbook_;
    
    bool stop_playing_;
};

} // namespace
} // namespace

#endif
