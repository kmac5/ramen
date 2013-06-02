// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_UI_PALETTE_HPP
#define	RAMEN_UI_PALETTE_HPP

#include<map>
#include<memory>

#include<boost/noncopyable.hpp>

#include<OpenEXR/ImathColor.h>

#include<QColor>

namespace ramen
{
namespace ui
{

class palette_t
{
public:

    static palette_t& instance();

	void rebuild();

    const Imath::Color3c& color( const std::string& k) const;
	QColor qcolor( const std::string& k) const;

	bool find_color( const std::string& k) const;

	void set_color( const std::string& k, const Imath::Color3c& c);
	void set_color( const std::string& k, const QColor& c);

private:

    palette_t();
	~palette_t();

	struct impl;
	std::auto_ptr<impl> pimpl_;
};

} // namespace
} // namespace

#endif
