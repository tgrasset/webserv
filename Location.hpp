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
		bool						getCgiBool(void) const;
		unsigned int				getRedirectionCode(void) const;
		std::string					getRedirectionPath(void) const;
		std::map<std::string, std::string>	getCgiExtensionAndPath(void) const;
		std::string					getUploadDir(void) const;

		void	setPath(std::string path);
		void	setRoot(std::string root, std::string serverRoot);
		void	setIndex(std::string index);
		void	setAutoIndex(bool autoindex);
		void	setMethods(std::vector<std::string> methods);
		void	setCgiBool(bool cgi);
		void	setRedirectionCode(int code);
		void	setRedirectionPath(std::string path);
		void	setCgiExtensionAndPath(std::vector<std::string> cgiInfo);
		void	setUploadDir(std::string dir);

		void	checkConfig(std::string serverRoot);

		class LocationException : public std::exception {
            public :
                LocationException(std::string errMessage) throw() {
					_errMessage = "Location Error: " + errMessage;
				}
				virtual const char* what() const throw() {
					return (_errMessage.c_str());
				}
				~LocationException() throw() {}
            
            private:
                std::string _errMessage;
        };

	private:

		std::string					_path;
		std::string					_root;
		std::string					_index;
		bool						_autoindex;
		std::vector<std::string>	_methods;
		bool						_cgiLocation;
		std::map<std::string, std::string> 	_cgiExtensionAndPath;
		int							_redirectionCode;
		std::string					_redirection;
		std::string					_uploadDir;
		static bool					_verbose;
};

#endif