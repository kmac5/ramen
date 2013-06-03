// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_DOCUMENT_HPP
#define RAMEN_DOCUMENT_HPP

#include<ramen/app/document_fwd.hpp>

#include<memory>

#include<boost/noncopyable.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/undo/stack_fwd.hpp>

namespace ramen
{

/*!
\ingroup app
\brief Document class.
*/
class RAMEN_API document_t : boost::noncopyable
{
public:

    document_t();
    ~document_t();

    bool dirty() const	    { return dirty_;}
    void set_dirty( bool d) { dirty_ = d;}

    const undo::stack_t& undo_stack() const { return *undo_;}
    undo::stack_t& undo_stack()             { return *undo_;}

    bool has_file() const { return !file_.empty();}

    boost::filesystem::path file() const { return file_;}
    void set_file( const boost::filesystem::path& p);

    // composition
    composition_t& composition()                { return comp_;}
    const composition_t& composition() const	{ return comp_;}

private:

    composition_t comp_;
    mutable bool dirty_;
    std::auto_ptr<undo::stack_t> undo_;
    boost::filesystem::path file_;
};

} // ramen

#endif
