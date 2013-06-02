// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ABOUT_DIALOG_HPP
#define	RAMEN_ABOUT_DIALOG_HPP

#include<QDialog>

#include"ui_about.h"

namespace ramen
{
namespace ui
{

class about_dialog_t : public QDialog
{
    Q_OBJECT
    
public:

    static about_dialog_t& instance();

private:

    about_dialog_t();

	Ui::about ui_;
};

} // namespace
} // namespace

#endif
