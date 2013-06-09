// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/dialogs/splash_screen.hpp>

#include<fstream>

#include<boost/filesystem/operations.hpp>

#include<QApplication>
#include<QSplashScreen>

//#include<SeExpression.h>

#include<ramen/app/application.hpp>
#include<ramen/filesystem/path.hpp>
#include<ramen/algorithm/clamp.hpp>

namespace ramen
{
namespace ui
{
namespace
{
/*
class splash_expression_t : public SeExpression, boost::noncopyable
{
public:

	struct var_t : public SeExprScalarVarRef
	{
		var_t() {}
		explicit var_t( double v) : val( v) {}
		
		void eval( const SeExprVarNode* node, SeVec3d& result) { result[0] = val;}
		double val;
	};
		
	splash_expression_t( const std::string& expr) : SeExpression( expr) {}
	
	SeExprVarRef *resolveVar( const std::string& name) const
	{
		std::map<std::string,var_t>::iterator i = vars.find( name);
		
		if( i != vars.end())
			return &i->second;
	
		return 0;		
	}
	
	mutable std::map<std::string,var_t> vars;
};
*/

} // unnamed

class splash_screen_widget_t : public QSplashScreen
{
public:
	
	splash_screen_widget_t( const QPixmap& pixmap, Qt::WindowFlags f = 0) : QSplashScreen( pixmap, f)
	{
	}
};

splash_screen_t::splash_screen_t() : splash_( 0)
{
	if( render_splash())
	{
		QPixmap pixmap = QPixmap::fromImage( image_);
	    splash_ = new splash_screen_widget_t( pixmap);
	}
	else
	{
	    QPixmap pixmap( ":/splash.jpg");
	    splash_ = new splash_screen_widget_t( pixmap);
	}
}

splash_screen_t::~splash_screen_t()
{
    if( splash_)
        splash_->deleteLater();
}

void splash_screen_t::show()
{
    splash_->show();
    qApp->processEvents();
}

void splash_screen_t::show_message( const std::string& msg)
{
    splash_->showMessage( QString::fromStdString( msg));
    qApp->processEvents();
}

void splash_screen_t::finish( QWidget *w)
{ 
	image_ = QImage();
	splash_->finish( w);
}

bool splash_screen_t::render_splash()
{
    /*
	boost::filesystem::path splash_sexpr_path = app().system().app_user_path() / "ui/splash.se";
	if( do_render_splash( splash_sexpr_path))
		return true;
	
	splash_sexpr_path = app().system().app_bundle_path() / "ui/splash.se";
	if( do_render_splash( splash_sexpr_path))
		return true;
    */
	return false;
}

bool splash_screen_t::do_render_splash( const boost::filesystem::path& p)
{
    /*
	try
	{
		if( !boost::filesystem::exists( p))
			return false;
		
		std::ifstream istream( filesystem::file_cstring( p));
		if( !istream)
			return false;
		
		std::string expr_str( ( std::istreambuf_iterator<char>( istream)),
							  std::istreambuf_iterator<char>());
	
		splash_expression_t expr( expr_str);
		expr.vars["xmin"] = splash_expression_t::var_t( 0);
		expr.vars["ymin"] = splash_expression_t::var_t( 0);
		expr.vars["xmax"] = splash_expression_t::var_t( image_width() - 1);
		expr.vars["ymax"] = splash_expression_t::var_t( image_height() - 1);
		expr.vars["w"] = splash_expression_t::var_t( image_width());
		expr.vars["h"] = splash_expression_t::var_t( image_height());
		expr.vars["x"] = splash_expression_t::var_t();
		expr.vars["y"] = splash_expression_t::var_t();
		expr.vars["u"] = splash_expression_t::var_t();
		expr.vars["v"] = splash_expression_t::var_t();
	
		if( !expr.isValid())
			return false;
		
		image_ = QImage( image_width(), image_height(), QImage::Format_RGB32);
		
		double& u=expr.vars["u"].val;
		double& v=expr.vars["v"].val;
		double& x=expr.vars["x"].val;
		double& y=expr.vars["y"].val;
		
		for( int j = 0; j < image_height(); ++j)
		{
			for( int i = 0; i < image_width(); ++i)
			{
				x = i;
				y = j;
				u = ( double) i / image_width();
				v = ( double) j / image_height();
				
				SeVec3d result = expr.evaluate();
                image_.setPixel( i, j, qRgb( clamp( result[0], 0.0, 1.0) * 256,
                                             clamp( result[1], 0.0, 1.0) * 256,
                                             clamp( result[2], 0.0, 1.0) * 256));
			}
		}
		
		return true;
	}
	catch( ...)
	{
	}
    */

    return false;
}

} // ui
} // ramen
