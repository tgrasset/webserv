#ifndef SERVER_HPP

# define SERVER_HPP

# include "webserv.hpp"

class Server {

    public:

    private:

        uint16_t						_port;
		in_addr_t						_host;
		std::string						_server_name;
		std::string						_root;
		unsigned long					_client_max_body_size;
		std::string						_index;
		bool							_autoindex;
		std::map<int, std::string>      _error_pages;
		std::vector<Location> 			_locations;
        struct sockaddr_in 				_server_address;
        int     						_listen_fd;
};

#endif