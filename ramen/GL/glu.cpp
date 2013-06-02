// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/GL/glu.hpp>

namespace ramen
{

void glu_ortho2d( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
	gluOrtho2D( left, right, bottom, top);
}

} // ramen
