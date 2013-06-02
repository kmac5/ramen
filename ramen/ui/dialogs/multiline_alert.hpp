// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MULTILINE_ALERT_HPP
#define	RAMEN_MULTILINE_ALERT_HPP

#include<string>

#include<QDialog>

class QTextEdit;

namespace ramen
{
namespace ui
{

class multiline_alert_t : public QDialog
{
    Q_OBJECT

public:

    static multiline_alert_t& instance();

    void show_alert( const std::string& title, const std::string& text);

private:

    multiline_alert_t();

	QTextEdit *text_;
};

} // namespace
} // namespace

#endif
