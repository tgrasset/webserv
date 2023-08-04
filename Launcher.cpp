/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:41 by mbocquel          #+#    #+#             */
/*   Updated: 2023/08/04 15:57:11 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Launcher.hpp"

bool	Launcher::_verbose = false;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
Launcher::Launcher(std::string path)
{
	if (Launcher::_verbose)
		std::cout << "Launcher path constructor called" << std::endl;
	this->_path_conf = path;
	return ;
}

Launcher::Launcher(Launcher const & copy)
{
	*this = copy;
	if (Launcher::_verbose)
		std::cout << "Launcher copy constructor called" << std::endl;
}

Launcher::~Launcher(void)
{
	// Closing all socket
	close(_efd);
	for (std::list<Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); ++it)
		close(it->getCom_socket());
	for (std::vector<Server>::iterator it = this->_servers.begin();
		it != this->_servers.end(); ++it)
		close(it->getListenSocket());
	if (Launcher::_verbose)
		std::cout << "Launcher destructor called" << std::endl;
}

/* ************************************************************************** */
/*                     Surcharge d'operateur                                  */
/* ************************************************************************** */
Launcher & Launcher::operator=(Launcher const & launcher)
{
	if (this != &launcher)
	{
		this->_servers = launcher._servers;
		this->_clients = launcher._clients;
	}
	return (*this);
}

/* ************************************************************************** */
/*                     Methodes                                               */
/* ************************************************************************** */

void	Launcher::parse(void)
{
	ConfParser conf_parser(this->_path_conf);
	conf_parser.parse();
	this->_servers = conf_parser.getServers();
}

void	Launcher::launch_servers(void)
{
	_ep_event.reserve(this->_servers.size() * MAX_EVENTS);
	int nfds = 0;

	_efd = epoll_create1 (0);
	if (_efd == -1)
		throw LauncherException("Probleme epoll_create !");
	this->initiate_servers_listening();
	/* Global loop for the Webserv */
	while (1)
	{
		this->print_situation();
		this->check_timeout_clients();
		if ((nfds = epoll_wait(_efd, static_cast< struct epoll_event * >(_ep_event.data()), this->_servers.size() * MAX_EVENTS, -1)) == -1)
			throw LauncherException("Problem with epoll_wait !");
        for (int i = 0; i < nfds; i++)
		{
			if ((_ep_event[i].events & EPOLLIN) == EPOLLIN) //Read.
			{
				if (check_if_listen_socket(_ep_event[i].data.fd)) //New connexion
					this->process_new_client(i);
				else
					this->process_reading_existing_client(i);
			}
			else if ((_ep_event[i].events & EPOLLOUT) == EPOLLOUT) //Write.
				this->process_writing_to_client(i);
			else if ((_ep_event[i].events & EPOLLRDHUP) == EPOLLRDHUP) //Peer closed connection.
			{
				std::list<Client>::iterator client_it = this->find_client(_ep_event[i].data.fd);
				std::cout << "Client " << client_it->getId() << " has close connexion" << std::endl;
				this->remove_client(client_it);
			}
		}
	}
}

void	Launcher::initiate_servers_listening(void)
{
	struct epoll_event	ev;

	for (std::vector<Server>::iterator it = this->_servers.begin();
	it != this->_servers.end(); ++it)
	{
		if (this->getServerWithSameHostPort(it) != this->_servers.end())
			it->setListenSocket(this->getServerWithSameHostPort(it)->getListenSocket());
		else
		{
			int listensocket = socket(AF_INET, SOCK_STREAM, 0);
			if (listensocket < 0)
				throw LauncherException("Problem with socket !");
			const int enable = 1;
			if (setsockopt(listensocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
				throw LauncherException("Problem with setsockopt !");
			it->setListenSocket(listensocket);
			it->setServaddr();
			it->bind_server();
			if (listen(it->getListenSocket(), MAX_WAIT) < 0)
				throw LauncherException("Impossible de listen !");
		}
		ev.events = EPOLLIN;
		ev.data.fd = it->getListenSocket();
		if (epoll_ctl(_efd, EPOLL_CTL_ADD, it->getListenSocket(), &ev) == -1)
			throw LauncherException("\e[31mProblem with epoll_ctl ! 1\e[0m");
	}
}

/*
- Get fd_new from accept
- Add fd_new to epoll
- Add a new client in the client vector
*/
void	Launcher::process_new_client(int i)
{
	socklen_t 			addrlen;
	struct epoll_event	ev;
	struct sockaddr_in  client_addr;
	char				str_ip_client[INET_ADDRSTRLEN];

	addrlen = sizeof (struct sockaddr_in);
	int fd_new;
	fd_new = accept(_ep_event[i].data.fd, (struct sockaddr *) &client_addr, &addrlen);
	if (fd_new == -1)
		throw LauncherException("Problem with accept !");
	ev.events = EPOLLIN;
	ev.data.fd = fd_new;
	if (epoll_ctl(_efd, EPOLL_CTL_ADD, fd_new, &ev) == -1)
		throw LauncherException("Problem with epoll_ctl !");
	Client	new_client(client_addr, fd_new);
	this->add_server_of_client(_ep_event[i].data.fd, &new_client);
	_clients.push_back(new_client);
	inet_ntop (AF_INET, &(client_addr.sin_addr), str_ip_client, sizeof (str_ip_client));
	std::cout << getTimestamp() << "\e[33m	I just accepted a new connexion from " << str_ip_client << "\e[0m" << std::endl;
}

/*
- Launch the function that recieve the client request
- If the client is done sending request, move if to the EPOLLOUT list
*/
void	Launcher::process_reading_existing_client(int i)
{
	std::list<Client>::iterator client = this->find_client(_ep_event[i].data.fd);
	if (client->receive_request())
		this->remove_client(client);
	if (client->getStatus() == WAITING_FOR_RES)
	{
		client->reset_last_activity();
		struct epoll_event	ev;
		int fd = _ep_event[i].data.fd;
		ev.events = EPOLLOUT;
		ev.data.fd = fd;
		if (epoll_ctl(_efd, EPOLL_CTL_DEL, _ep_event[i].data.fd, NULL) == -1)
			throw LauncherException("Error: epoll_ctl DEL!");
		if (epoll_ctl(_efd, EPOLL_CTL_ADD, fd, &ev) == -1)
			throw LauncherException("Problem with epoll_ctl !");
	}
}

void	Launcher::process_writing_to_client(int i)
{
	std::list<Client>::iterator client = this->find_client(_ep_event[i].data.fd);
	client->send_response();
	if (client->getStatus() == RES_SENT && client->getKeepAlive())
	{
		std::cout << "Finish writing to " << client->getId() << ", I add him to listen socket" << std::endl << std::endl;
		client->reset_client();
		struct epoll_event	ev;
		int fd = _ep_event[i].data.fd;
		ev.events = EPOLLIN;
		ev.data.fd = fd;
		if (epoll_ctl(_efd, EPOLL_CTL_DEL, _ep_event[i].data.fd, NULL) == -1)
			throw LauncherException("Error: epoll_ctl DEL!");
		if (epoll_ctl(_efd, EPOLL_CTL_ADD, fd, &ev) == -1)
			throw LauncherException("Problem with epoll_ctl !");
	}
	else if (client->getStatus() == RES_SENT)
	{
		std::cout << "Process finished for client " << client->getId() << ", it will now be removed" << std::endl << std::endl;
		this->remove_client(client);
	}
	else if (client->getStatus() == ERROR_WHILE_SENDING)
	{
		std::cout << "An error happend while sending the response  to client " << client->getId() << ". The client will now be removed" << std::endl << std::endl;
		this->remove_client(client);
	}
}

std::vector<Server>::iterator 	Launcher::getServerWithSameHostPort(std::vector<Server>::iterator it_find)
{
	for (std::vector<Server>::iterator it = this->_servers.begin(); it != this->_servers.end();
	++it)
	{
		if (it == it_find)
			return (this->_servers.end());
		if (it->getHost() == it_find->getHost() && it->getPort() == it_find->getPort())
			return (it);
	}
	return (this->_servers.end());
}

std::list<Client>::iterator	Launcher::find_client(int socket)
{
	std::list<Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if (it->getCom_socket() == socket)
			break ;
		++it;
	}
	if (it == this->_clients.end())
		throw LauncherException("Error: Client not found !");
	return (it);
}

bool	Launcher::check_if_listen_socket(int socket)
{
	for (std::vector<Server>::iterator it = this->_servers.begin();
	it != this->_servers.end(); ++it)
	{
		if (socket == it->getListenSocket())
			return (true);
	}
	return (false);
}

void 	Launcher::add_server_of_client(int listen_sock, Client *client)
{
	for (std::vector<Server>::iterator it = this->_servers.begin();
	it != this->_servers.end(); ++it)
	{
		if (listen_sock == it->getListenSocket())
			client->AddServerPtr(&(*it));
	}
}

void	Launcher::check_timeout_clients(void)
{
	unsigned long time = 0;
	for(std::list<Client>::iterator it = this->_clients.begin(); it != this->_clients.end();++it)
	{
		time = it->time_since_last_activity_us() / 1000000;
		if (time > MAX_TIME_CLIENT_S)
		{
			std::cout << "\e[31mTimeout for client " << it->getId()
			<< ", time = " << time << " it will now be removed\e[0m" << std::endl << std::endl;
			std::list<Client>::iterator it_tmp = it;
			--it;
			this->remove_client(it_tmp);
		}
	}
}

void	Launcher::remove_client(std::list<Client>::iterator client)
{
	if (epoll_ctl(_efd, EPOLL_CTL_DEL, client->getCom_socket(), NULL) == -1)
		throw LauncherException("Error: epoll_ctl DEL!");
	close(client->getCom_socket());
	this->_clients.erase(client);
}

void	Launcher::print_situation(void)
{
	char				str_ip_client[INET_ADDRSTRLEN];
	struct sockaddr_in  client_addr;

	//std::cout << std::endl << "--------------------- Servers -----------------" << std::endl;
	/*for (std::vector<Server>::iterator it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		std::cout << "Listen Socket : " << it->getListenSocket() << " | Name : " << it->getServerName()
		<< " | Host : " << it->getHost() << " | Port : " << ntohs(it->getPort()) << std::endl << std::endl;
	}*/
	return ;
	for (std::list<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (it == this->_clients.begin())
			std::cout << "--------------------- Clients -----------------" << std::endl;
		client_addr = it->getClient_addr();
		inet_ntop (AF_INET, &(client_addr.sin_addr), str_ip_client, sizeof (str_ip_client));
		std::cout << "ID : " << it->getId() << "	|";
		std::cout << " Com Socket : " << it->getCom_socket() << "	|";
		std::cout << " Addresse : " << str_ip_client << "	|";
		std::cout << " Server :";
		it->print_ClientServer();
		std::cout << "	|";
		std::cout << " Status : ";
		switch (it->getStatus())
		{
		case 0:
			std::cout << "WANT_TO_RECIVE_REQ";
			break;
		case 1:
			std::cout << "RECIVING_REQ_HEADER";
			break;
		case 2:
			std::cout << "RECIVING_REQ_BODY";
			break;
		case 3:
			std::cout << "REQ_RECIVED";
			break;
		case 4:
			std::cout << "WAITING_FOR_RES";
			break;
		case 5:
			std::cout << "SENDING_RES_HEADER";
			break;
		case 6:
			std::cout << "SENDING_RES_BODY";
			break;
		case 7:
			std::cout << "RES_SENT";
			break;
		default:
			std::cout << "ERROR_WHILE_SENDING";
			break;
		}
		std::cout << std::endl << std::endl;
	}
}
void Launcher::test_folder_tmp(void) const
{
	std::string body_tmp_folder(BODY_TMP_FOLDER);
	struct stat stats_dossier;

	if (stat(body_tmp_folder.c_str(), &stats_dossier) != 0
	|| access(body_tmp_folder.c_str(), W_OK | R_OK) != 0
	|| !S_ISDIR(stats_dossier.st_mode))
		throw LauncherException("Temp body folder does not exist");

}
