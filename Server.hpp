#ifndef SERVER_HPP

# define SERVER_HPP

# include "webserv.hpp"
# include "Location.hpp"

class Server {

    public:

		Server(void);
        Server(Server const &src);

        ~Server();

        Server &operator=(Server const &rhs);

		in_port_t					getPort(void) const;
		in_addr_t					getHost(void) const;
		std::string 				getServerName(void) const;
		std::string					getRoot(void) const;
		unsigned long				getClientMaxBodySize(void) const;
		std::string					getIndex(void) const;
		std::map<int, std::string>	getErrorPages(void) const;
		std::vector<Location>		getLocations(void) const;
		int							getListenSocket(void) const;

		void						setPort(std::string port);
		void						setHost(std::string host);
		void 						setServerName(std::string name);
		void						setRoot(std::string root);
		void						setClientMaxBodySize(std::string clientMaxBodySize);
		void						setIndex(std::string index);
		void						setErrorPages(std::vector<std::string> errorPages);
		void						setLocation(std::string path, std::vector<std::string> content);

		void						checkDoubleLocations(void);

		class ServerException : public std::exception {
            public :
                ServerException(std::string errMessage) throw() {
					_errMessage = "Server Error: " + errMessage;
				}
				virtual const char* what() const throw() {
					return (_errMessage.c_str());
				}
				~ServerException() throw() {}
            
            private:
                std::string _errMessage;
        };
		
    private:

        in_port_t						_port;
		in_addr_t						_host;
		std::string						_server_name;
		std::string						_root;
		unsigned int					_client_max_body_size;
		std::string						_index;
		std::map<int, std::string>      _error_pages;
		std::vector<Location> 			_locations;
		int								_listenSocket;
		static bool						_verbose;
};

#endif