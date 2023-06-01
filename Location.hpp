#ifndef LOCATION_HPP

# define LOCATION_HPP

#include "webserv.hpp"

class Location {

	public:

		Location();
		Location(Location const &src);

		~Location();

		Location &operator=(Location const &rhs);

		std::string					getPath(void) const;
		std::string					getRoot(void) const;
		std::string					getIndex(void) const;
		bool						getAutoIndex(void) const;
		std::vector<std::string>	getMethods(void) const;
		std::string					getCGIpath(void) const;

		void	setPath(std::string path);
		void	setRoot(std::string root);
		void	setIndex(std::string index);
		void	setAutoIndex(bool autoindex);
		void	setMethods(std::vector<std::string> methods);
		void	setCGIpath(std::string path);

	private:

		std::string					_path;
		std::string					_root;
		std::string					_index;
		bool						_autoindex;
		std::vector<std::string>	_methods;
		std::string					_CGIpath;

};

#endif