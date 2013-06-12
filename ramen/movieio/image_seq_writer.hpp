// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MOVIEIO_IMAGE_SEQ_WRITER_HPP
#define	RAMEN_MOVIEIO_IMAGE_SEQ_WRITER_HPP

#include<ramen/movieio/writer.hpp>

namespace ramen
{
namespace movieio
{

class RAMEN_API image_seq_writer_t : public writer_t
{
public:

    image_seq_writer_t();
	
private:

	std::string tag_;
};

} // movieio
} // ramen

#endif
