// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_PANEL_HPP
#define	RAMEN_UI_PANEL_HPP

#include<ramen/ui/inspector/panel_fwd.hpp>

#include<boost/signals2/connection.hpp>

#include<QObject>

#include<ramen/params/parameterised_fwd.hpp>

class QWidget;

namespace ramen
{
namespace ui
{

class panel_t : public QObject
{
    Q_OBJECT

public:

    panel_t( parameterised_t *p);
    ~panel_t();

	void set_connection( const boost::signals2::connection c);

    QWidget *widget() { return panel_;}

    void update();

private:

    parameterised_t *p_;
    QWidget *panel_;
	boost::signals2::connection connection_;
};

} // namespace
} // namespace

#endif
