// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.



#include<ramen/ui/dialogs/multiline_alert.hpp>

#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QButtonGroup>
#include<QDialogButtonBox>
#include<QTextEdit>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>

namespace ramen
{
namespace ui
{

multiline_alert_t& multiline_alert_t::instance()
{
    static multiline_alert_t dialog;
    return dialog;
}

multiline_alert_t::multiline_alert_t() : QDialog( app().ui()->main_window())
{
    setWindowTitle( "Alert");

    QVBoxLayout *vlayout = new QVBoxLayout( this);

	text_ = new QTextEdit();
	text_->setReadOnly( true);
	vlayout->addWidget( text_);

    QDialogButtonBox *buttonBox = new QDialogButtonBox();
    buttonBox->setOrientation( Qt::Horizontal);
    buttonBox->setStandardButtons( QDialogButtonBox::Ok);
    connect( buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    vlayout->addWidget( buttonBox);
}

void multiline_alert_t::show_alert( const std::string& title, const std::string& text)
{
	setWindowTitle( title.c_str());
	text_->setText( text.c_str());
	int result = exec();
}

} // namespace
} // namespace
