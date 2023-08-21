/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:41 by mbocquel          #+#    #+#             */
/*   Updated: 2023/08/21 18:16:50 by mbocquel         ###   ########.fr       */
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
	this->_pathConf = path;
	_breakPollLoop = false;
	_pollEvent.clear();
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
	for (std::list<Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); ++it)
		close(it->getComSocket());
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
		_servers = launcher._servers;
		_clients = launcher._clients;
		_pathConf = launcher._pathConf;
		_pollEvent = launcher._pollEvent;
		_breakPollLoop = launcher._breakPollLoop;
	}
	return (*this);
}

/* ************************************************************************** */
/*                     Methodes                                               */
/* ************************************************************************** */

void	Launcher::parse(void)
{
	ConfParser conf_parser(this->_pathConf);
	conf_parser.parse();
	this->_servers = conf_parser.getServers();
}

void	Launcher::launchServers(void)
{
	_pollEvent.reserve(this->_servers.size() * MAX_EVENTS);
	int nfds = 0;
	this->initiateServersListening();
	/* Global loop for the Webserv */
	while (1)
	{
		_breakPollLoop = false;
		this->checkTimeoutClients();
		if ((nfds = poll(_pollEvent.data(), _pollEvent.size() , -1)) == -1)
			throw LauncherException("Problem with poll !");
		if (nfds > 0)
		{
			for (std::vector< struct pollfd >::iterator it = _pollEvent.begin(); it != _pollEvent.end(); ++it)
			{	
				if (it->revents & POLLIN) //Read.
				{
					if (checkIfListenSocket(it->fd)) //New connexion
						this->processNewClient(it->fd);
					else
						this->processReadingFd(it->fd);
				}
				else if (it->revents & POLLOUT) //Write
				{
					this->processWritingFd(it->fd);
				}
				else if ((it->revents & POLLERR) || (it->revents & POLLHUP))
				{
					this->processCloseConnexionOrError(it->fd);
				}
				if (_breakPollLoop)
					break;
			}
		}
	}
}

void	Launcher::initiateServersListening(void)
{
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
				throw LauncherException("Impossible to listen !");
		}
		addFdToPollIn(it->getListenSocket());
	}
}


void	Launcher::processNewClient(int fd)
{
	int 				fd_new;
	socklen_t 			addrlen;
	struct sockaddr_in  client_addr;
	char				str_ip_client[INET_ADDRSTRLEN];

	addrlen = sizeof (struct sockaddr_in);
	fd_new = accept(fd, (struct sockaddr *) &client_addr, &addrlen);
	if (fd_new == -1)
		throw LauncherException("Problem with accept !");
	addFdToPollIn(fd_new);
	Client	new_client(this, client_addr, fd_new);
	addServerOfClient(fd, &new_client);
	_clients.push_back(new_client);
	inet_ntop (AF_INET, &(client_addr.sin_addr), str_ip_client, sizeof (str_ip_client));
	std::cout << getTimestamp() << "\e[33m	I just accepted a new connexion from " << str_ip_client << "\e[0m" << std::endl;
}

/*
- Launch the function that recieve the client request
- If the client is done sending request, move if to the EPOLLOUT list
*/

void	Launcher::processReadingFd(int fd)
{
	std::list<Client>::iterator client = this->findClient(fd);
	switch (client->getSocketType(fd))
	{
	case COM_SOCKET:
		std::cout << std::endl << TXT_B << TXT_CY << "I have a reading COM_SOCKET event from client " << client->getId() << TXT_END << std::endl;
		if (client->receiveRequest())
			this->removeClient(client);
		else if (client->getStatus() == WAITING_FOR_RES)
		{
			client->resetLastActivity();
			removeFdFromPoll(fd);
			addFdToPollOut(fd);
		}
		break;
	case CGI_PIPE:
		std::cout << std::endl << TXT_B << TXT_CY << "I have a reading CGI_PIPE event from client " << client->getId() << TXT_END << std::endl;
		client->addCgiToBuff();
		break;
	case RES_FILE_FD:
		std::cout<< std::endl << TXT_B << TXT_CY << "I have a reading RES_FILE_FD event from client " << client->getId() << TXT_END << std::endl;
		client->addBodyFileToBuff();
		break;
	/*case REQ_FILE_FD:
		std::cout << std::endl << "I have a reading REQ_FILE_FD event from client " << client->getId() << std::endl;
		break;*/
	default:
		throw LauncherException("The Reading FD is not in possible category");
	}
}

void	Launcher::processWritingFd(int fd)
{
	std::list<Client>::iterator client = this->findClient(fd);
	switch (client->getSocketType(fd))
	{
	case COM_SOCKET:
		std::cout << std::endl << TXT_B << TXT_MAG << "I have a writing COM_SOCKET event from client " << client->getId() << TXT_END << std::endl;
		client->sendResponse();
		if (client->getStatus() == RES_SENT && client->getKeepAlive())
		{
			std::cout << "Finish writing to " << client->getId() << ", I add him to listen socket" << std::endl;
			client->resetClient();
			removeFdFromPoll(fd);
			addFdToPollIn(fd);
		}
		else if (client->getStatus() == RES_SENT)
		{
			std::cout << "Process finished for client " << client->getId() << ", it will now be removed" << std::endl;
			this->removeClient(client);
		}
		else if (client->getStatus() == ERROR_WHILE_SENDING)
		{
			std::cout << "An error happend while sending the response  to client " << client->getId() << ". The client will now be removed" << std::endl << std::endl;
			this->removeClient(client);
		}
		break;
	case REQ_FILE_FD:
		std::cout << std::endl << TXT_B << TXT_MAG << "I have a writing REQ_FILE_FD event from client " << client->getId() << TXT_END << std::endl;
		client->writeReqBodyFile();
		break;
	default:
		throw LauncherException("Problem writing fd is not a correct category");
	}
}

void	Launcher::processCloseConnexionOrError(int fd)
{
	/*Si c'est un CGI, le POLLHUP veut dire que c'est fini et il faut terminer. */
	std::list<Client>::iterator client = this->findClient(fd);
	switch (client->getSocketType(fd))
	{
	case COM_SOCKET:
		std::cout << "Client " << client->getId() << " has close connexion" << std::endl;
		this->removeClient(client);
		break;
	case CGI_PIPE:
		client->cgiPipeFinishedWriting();
		break;
	default:
		break;
	}
}

std::vector<Server>::iterator 	Launcher::getServerWithSameHostPort(std::vector<Server>::iterator it_find)
{
	for (std::vector<Server>::iterator it = this->_servers.begin(); it != this->_servers.end();	++it)
	{
		if (it == it_find)
			return (this->_servers.end());
		if (it->getHost() == it_find->getHost() && it->getPort() == it_find->getPort())
			return (it);
	}
	return (this->_servers.end());
}

std::list<Client>::iterator	Launcher::findClient(int socket)
{
	std::list<Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if (it->getSocketType(socket) != NOT_MINE)
			break;
		++it;
	}
	if (it == this->_clients.end())
	{
		throw LauncherException("Client not found !");
	}
	return (it);
}
	
bool	Launcher::checkIfListenSocket(int socket)
{
	for (std::vector<Server>::iterator it = this->_servers.begin();
	it != this->_servers.end(); ++it)
	{
		if (socket == it->getListenSocket())
			return (true);
	}
	return (false);
}

void 	Launcher::addServerOfClient(int listen_sock, Client *client)
{
	for (std::vector<Server>::iterator it = this->_servers.begin();
	it != this->_servers.end(); ++it)
	{
		if (listen_sock == it->getListenSocket())
			client->AddServerPtr(&(*it));
	}
}

void	Launcher::checkTimeoutClients(void)
{
	unsigned long time = 0;
	for(std::list<Client>::iterator it = this->_clients.begin(); it != this->_clients.end();++it)
	{
		time = it->timeSinceLastActivityUs() / 1000000;
		if (time > MAX_TIME_CLIENT_S)
		{
			std::cout << "\e[31mTimeout for client " << it->getId()
			<< ", time = " << time << " it will now be removed\e[0m" << std::endl << std::endl;
			std::list<Client>::iterator it_tmp = it;
			--it;
			this->removeClient(it_tmp);
		}
	}
}

void	Launcher::removeClient(std::list<Client>::iterator client)
{
	removeFdFromPoll(client->getComSocket());
	close(client->getComSocket());
	this->_clients.erase(client);
}

void	Launcher::printSituation(void)
{
	char				str_ip_client[INET_ADDRSTRLEN];
	struct sockaddr_in  client_addr;

	for (std::list<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		if (it == this->_clients.begin())
			std::cout << "--------------------- Clients -----------------" << std::endl;
		client_addr = it->getClient_addr();
		inet_ntop (AF_INET, &(client_addr.sin_addr), str_ip_client, sizeof (str_ip_client));
		std::cout << "ID : " << it->getId() << "	|";
		std::cout << " Com Socket : " << it->getComSocket() << "	|";
		std::cout << " Addresse : " << str_ip_client << "	|";
		std::cout << " Server :";
		it->printClientServer();
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
void Launcher::testFolderTmp(void) const
{
	std::string body_tmp_folder(BODY_TMP_FOLDER);
	struct stat stats_dossier;

	if (stat(body_tmp_folder.c_str(), &stats_dossier) != 0
	|| access(body_tmp_folder.c_str(), W_OK | R_OK) != 0
	|| !S_ISDIR(stats_dossier.st_mode))
		throw LauncherException("Temp body folder does not exist");

}

void	Launcher::removeFdFromPoll(int fd)
{
	std::vector <struct pollfd >::iterator it = _pollEvent.begin();
	while (it != _pollEvent.end())
	{
		if (it->fd == fd)
			break;
		++it;
	}
	_pollEvent.erase(it);
	_breakPollLoop = true;
}

void	Launcher::printPollEvent(void) const
{
	for(std::vector < struct pollfd >::const_iterator it = this->_pollEvent.begin(); it != _pollEvent.end(); ++it)
	{
		std::cout << "	FD: " << it->fd << " | event: " << it->events << " | revent: " << it->revents << std::endl;
	}
}

void	Launcher::addFdToPollIn(int fd)
{
	struct pollfd poll_struct;
	poll_struct.fd = fd;
	poll_struct.events = POLLIN | POLLHUP | POLLERR;
	_pollEvent.push_back(poll_struct);
	_breakPollLoop = true;
}

void	Launcher::addFdToPollOut(int fd)
{
	struct pollfd poll_struct;
	poll_struct.fd = fd;
	poll_struct.events = POLLOUT | POLLERR;
	_pollEvent.push_back(poll_struct);
	_breakPollLoop = true;
}
