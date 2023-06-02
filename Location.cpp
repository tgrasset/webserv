#include "Location.hpp"

Location::Location(void) {

	_path = "";
	_root = "";
	_index = "";
	_autoindex = false;
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

void	Location::setPath(std::string path) {

	_path = path;
}

void	Location::setRoot(std::string root) {

	if (isValidConfValue(root) == false)
		throw LocationException("';' symbol needed after 'root' line in 'location' context");
	struct stat test;
	if (stat(root.c_str(), &test) == 0 && test.st_mode & S_IFDIR)
	{
		_root = root;
		return ;
	}
	char buffer[1024];
	getcwd(buffer, 1024);
	std::string root2 = buffer + root;
	if (stat(root2.c_str(), &test) == 0 && test.st_mode & S_IFDIR)
		_root = root2;
	else
		throw LocationException("Invalid root path in 'location' context");
}

void	Location::setIndex(std::string index) {

		if (isValidConfValue(index) == true)
			_index = index;
		else
			throw LocationException("';' symbol needed after 'index' line in 'location' context");
}

void	Location::setAutoIndex(bool autoindex) {

	_autoindex = autoindex;
}

void	Location::setMethods(std::vector<std::string> methods) {

	size_t size = methods.size();
	size_t j;

	for (size_t i = 0; i < size; i++)
	{
		if (methods[i] != "GET" && methods[i] != "POST" && methods[i] != "DELETE")
			throw LocationException("Unknown method : " + methods[i]);
		else
		{
			for (j = 0; j < _methods.size(); j++)
			{
				if (_methods[j] == methods[i])
					throw LocationException("Method repetition in 'location' context : " + methods[i]);
			}
			_methods.push_back(methods[i]);
		}
	}
}

void	Location::checkConfig(std::string serverRoot) {

	if (_path[0] != '/')
		throw LocationException("Invalid path format after 'location' directive");
	if (_root == "")
		_root = serverRoot;
	if (_autoindex == false && _index != "" && checkFile(_index, _root + _path) == false)
		throw LocationException("Index file in 'location' context doesn't exist or couldn't be read");
}