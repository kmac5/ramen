/*
	bayesian matting tool
	2006 Est.
*/

#include<exception>
#include<algorithm>
#include<functional>
#include<fstream>
#include<iostream>

#include<boost/cstdlib.hpp>

#include<boost/program_options.hpp>
namespace po = boost::program_options;

#include<adobe/dictionary.hpp>

#include<vigra/basicimageview.hxx>
#include<vigra/transformimage.hxx>
#include<vigra/colorconversions.hxx>

#include"imageio.hpp"
#include"bmatte.hpp"

#include<OpenGL/gl.h>
#include<GLUT/glut.h>

blitz::Array<Imath::Color3f,2> image;
blitz::Array<Imath::Color3f,2> fg, bg;

blitz::Array<Imath::Color3f,2> comp;

blitz::Array<uint8_t,2> trimap;
blitz::Array<float,2> alpha;

enum { display_a, display_fg, display_bg, display_comp, display_img, display_tri};
int disp = display_a;

void gl_init( void)
{
	glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );
	glClearColor ( 0.0F, 0.0F, 0.0F, 0.0F );

	glViewport ( 0, 0, image.cols(), image.rows());

   // set coordinate frame for graphics in window
	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity();

	gluOrtho2D ( 0, image.cols(), image.rows(), 0);
	glMatrixMode ( GL_MODELVIEW);
	glLoadIdentity();
		
	glPixelZoom( 1, -1);
}

void quit( void)
{
	//annClose();
	exit( boost::exit_success);
}

void reshape(int w, int h) 
{
	glViewport ( 0, 0, w, h);

	glMatrixMode ( GL_PROJECTION );
	glLoadIdentity();

	gluOrtho2D ( 0, w, h, 0);

	glMatrixMode ( GL_MODELVIEW );
	glLoadIdentity();
}

void key( unsigned char key, int x, int y)
{
	if( key == 'a')
	{
		disp = display_a;
		glutPostRedisplay();
	}

	if( key == 'f')
	{
		disp = display_fg;
		glutPostRedisplay();
	}

	if( key == 'b')
	{
		disp = display_bg;
		glutPostRedisplay();
	}

	if( key == 'c')
	{
		disp = display_comp;
		glutPostRedisplay();
	}

	if( key == 'i')
	{
		disp = display_img;
		glutPostRedisplay();
	}

	if( key == 't')
	{
		disp = display_tri;
		glutPostRedisplay();
	}

	if( (key == 27) || (key == 'q') || (key == 'Q'))
		quit();
}

void mouse( int button, int state, int x, int y)
{
	//#define CLICK_SAVE_COLS
	#ifdef CLICK_SAVE_COLS
	
		int kw = 16;
		int xmin, xmax, ymin, ymax;
		
		xmin = std::max( 1, x - kw);
		ymin = std::max( 1, y - kw);
		xmax = std::min( x + kw, image.cols() - 1);
		ymax = std::min( y + kw, image.rows() - 1);
	
		std::ofstream of( "cluster_dat.hpp");
	
		if( !of)
			return;

		of << "const int num_pts = " << (xmax - xmin) * (ymax - ymin) * 4 << ";\n";
		of << "float dat[num_pts] = {\n";
	
		for( int j=ymin;j<ymax;++j)
		{
			for( int i=xmin;i<xmax;++i)
			{
				Imath::Color3f col = image( j, i);
				of << col.x << ", " << col.y << ", " << col.z << ", 1.0, \n";
			}
		}

		of << "};\n";
	#endif
}

void display( void)
{
	glClear( GL_COLOR_BUFFER_BIT);

	glRasterPos2i( 0, 0);
	
	switch( disp)
	{
		case display_a:
			glDrawPixels( image.cols(), image.rows(), GL_LUMINANCE, GL_FLOAT, alpha.dataFirst());
		break;
		
		case display_bg:
			glDrawPixels( image.cols(), image.rows(), GL_RGB, GL_FLOAT, bg.dataFirst());
		break;

		case display_fg:
			glDrawPixels( image.cols(), image.rows(), GL_RGB, GL_FLOAT, fg.dataFirst());
		break;

		case display_comp:
			glDrawPixels( image.cols(), image.rows(), GL_RGB, GL_FLOAT, comp.dataFirst());
		break;
		
		case display_img:
			glDrawPixels( image.cols(), image.rows(), GL_RGB, GL_FLOAT, image.dataFirst());
		break;
		
		case display_tri:
			glDrawPixels( image.cols(), image.rows(), GL_LUMINANCE, GL_UNSIGNED_BYTE, trimap.dataFirst());
		break;
	}
	
	glutSwapBuffers();
}

void usage()
{
	std::cout << "Usage: bmatte [args] input trimap -o output\n";
	exit( boost::exit_failure);
}

const float lab_scale = 100.0f;

void convert2lab( blitz::Array<Imath::Color3f,2>& img)
{
vigra::BasicImageView<vigra::TinyVector<float,3> > img_view( (vigra::TinyVector<float,3> *) img.dataFirst(), img.cols(), img.rows());
	
	vigra::transformImage( vigra::srcImageRange( img_view), vigra::destImage( img_view), vigra::RGB2LabFunctor<float>( 1.0f));

	for( int j=0;j<img.rows();++j)
		for( int i=0;i<img.cols();++i)
			img( j, i) /= lab_scale;
}

void convert2rgb( blitz::Array<Imath::Color3f,2>& img)
{
vigra::BasicImageView<vigra::TinyVector<float,3> > img_view( (vigra::TinyVector<float,3> *) img.dataFirst(), img.cols(), img.rows());

	for( int j=0;j<img.rows();++j)
		for( int i=0;i<img.cols();++i)
			img( j, i) *= lab_scale;

	vigra::transformImage( vigra::srcImageRange( img_view), vigra::destImage( img_view), vigra::Lab2RGBFunctor<float>( 1.0f));
}

int main( int argc, char **argv)
{
std::string inpf, trif, outf, backf;
adobe::dictionary_t params;

bool use_lab = false;

	try
	{
	po::options_description desc("Allowed options");
	po::variables_map vm;
	po::positional_options_description p;

		p.add( "input" , 1);
		p.add( "trimap", 1);

		desc.add_options()	("help", "produce help message")
							// files
							("input", po::value<std::string>(), "input file")
							("trimap",po::value<std::string>(), "trimap file")
							("output,o", po::value<std::string>(), "output file")
							("background,bg", po::value<std::string>(), "clean background")
							("save_bg", "save estimated background")

							// colors
							("rgb", "use rgb colorspace")

							// sampling
							("kwin", po::value<int>()->default_value( 8), "known window radius")
							("uwin", po::value<int>()->default_value( 8), "unknown window radius")
							("mins", po::value<int>()->default_value( 16), "min samples")
							
							// clusters
							("clusters", po::value<int>()->default_value( 5), "max number of clusters")
							("ctheresh", po::value<double>()->default_value( 0.001), "cluster thereshold")
	
							// optimize
							("cvar", po::value<double>()->default_value( 0.04), "camera variance")

							// gui
							("window,w", "show results in a window")
							; // end

		po::store( po::command_line_parser( argc, argv).options( desc).positional( p).run(), vm);
		po::notify( vm);

		if( vm.count( "help") || !vm.count("input") || !vm.count("trimap"))
			usage();
		
		if( (!vm.count("output")) && (!vm.count("window")))
			usage();
			
		inpf = vm["input"].as<std::string>();
		trif = vm["trimap"].as<std::string>();
		
		if( vm.count("output"))
			outf = vm["output"].as<std::string>();

		use_lab = !vm.count("rgb");
		
		params[adobe::name_t("use_lab")] = adobe::any_regular_t( use_lab);
		
		// load images
		image.reference( read_image( inpf.c_str()));
		trimap.reference( read_trimap( trif.c_str()));

		if( (trimap.rows() != image.rows()) || (trimap.cols() != image.cols()))
		{
			std::cout << "Image & trimap dimensions don't match\n";
			exit( boost::exit_failure);
		}

		if( use_lab)
			convert2lab( image);
		
		if( vm.count( "background"))
		{
			backf = vm["background"].as<std::string>();
			params[adobe::name_t("use_back")] = adobe::any_regular_t( true);

			bg.reference( read_image( backf.c_str()));
			
			if( use_lab)
				convert2lab( bg);
		}
		else
		{
			bg.resize( image.rows(), image.cols());
			params[adobe::name_t("use_back")] = adobe::any_regular_t( false);
		}

		// sampling
		params[ adobe::name_t( "kwin_size")] = adobe::any_regular_t( vm["kwin"].as<int>());
		params[ adobe::name_t( "uwin_size")] = adobe::any_regular_t( vm["uwin"].as<int>());
		params[ adobe::name_t( "min_samples")] = adobe::any_regular_t( vm["mins"].as<int>());

		// cluster
		params[ adobe::name_t( "maxk")] = adobe::any_regular_t( vm["clusters"].as<int>());
		params[ adobe::name_t( "ctheresh")] = adobe::any_regular_t( vm["ctheresh"].as<double>());

		// optimize
		params[ adobe::name_t( "cvar")] = adobe::any_regular_t( vm["cvar"].as<double>());

		fg.resize( image.rows(), image.cols());
		alpha.resize( image.rows(), image.cols());
		
		bmatte bm( image, trimap, fg, bg, alpha, params);
		bm();

		if( use_lab)
		{
			convert2rgb( image);
			convert2rgb( fg);
			convert2rgb( bg);
		}

		if( vm.count( "output"))
		{
			save_image( outf.c_str(), fg, &alpha);
		
			if( vm.count( "save_bg"))
			{
			std::string bgf = "bg_" + outf;

				save_image( bgf.c_str(), bg);
			}
		}

		if( vm.count( "window"))
		{
			comp.resize( image.rows(), image.cols());
		
			for( int j=0;j<image.rows();++j)
			{
				for( int i=0;i<image.cols();++i)
				{
					float t = alpha( j, i);
					comp( j, i) = (fg( j, i) * t) + ( Imath::Color3f( .5f, .5f, .5f) * (1.0f - t));
				}
			}

			glutInit(&argc, argv);
			glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	
			glutInitWindowPosition( 50, 50);

			glutInitWindowSize( image.cols(), image.rows());
			glutCreateWindow( "BMatte");

			glutKeyboardFunc( key);
			glutMouseFunc( mouse);

			glutDisplayFunc( display);

			gl_init();
			glutMainLoop();
		}
	}
    catch( std::exception& e)
	{
        std::cerr << "error: " << e.what() << "\n";
        return boost::exit_failure;
    }

    catch( ...) { std::cerr << "Exception of unknown type!\n";}
	return boost::exit_success;
}
