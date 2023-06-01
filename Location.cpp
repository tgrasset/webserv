#include "Location.hpp"

Location::Location(void) {

	_path = "";
	_root = "";
	_index = "";
	_autoindex = false;
	_CGIpath = "";
}

Location::Location(Location const &src) {

	if (this != &src)
        *this = src;
}

Location::~Location() {

}

Location &Location::operator=(Location const &rhs) {

	_path = rhs.getPath();
	_root = rhs.getRoot();
	_index = rhs.getIndex();
	_autoindex = rhs.getAutoIndex();
	_methods = rhs.getMethods();
	_CGIpath = rhs.getCGIpath();
    return (*this);
}

std::string		Location::getPath(void) const {

	return (_path);
}

std::string		Location::getRoot(void) const {

	return (_root);
}

std::string	Location::getIndex(void) const {

	return (_index);
}

bool	Location::getAutoIndex(void) const {

	return (_autoindex);
}

std::vector<std::string>	Location::getMethods(void) const {

	return (_methods);
}

std::string	Location::getCGIpath(void) const {

	return (_CGIpath);
}
