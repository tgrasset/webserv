/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:41 by mbocquel          #+#    #+#             */
/*   Updated: 2023/09/12 10:22:19 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Launcher.hpp"

/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
Launcher::Launcher(std::string path)
{
	this->_pathConf = path;
	_breakPollLoop = false;
	_pollEvent.clear();
	return ;
}

Launcher::Launcher(Launcher const & copy)
{
	*this = copy;
}

Launcher::~Launcher(void)
{
	for (std::list<Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); ++it)
		close(it->getComSocket());
	for (std::vector<Server>::iterator it = this->_servers.begin();
		it != this->_servers.end(); ++it)
		close(it->getListenSocket());
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
				if (it->revents & POLLIN && !_breakPollLoop) //Read.
				{
					if (checkIfListenSocket(it->fd)) //New connexion
						this->processNewClient(it->fd);
					else
						this->processReadingFd(it->fd);
				}
				else if (it->revents & POLLOUT && !_breakPollLoop) //Write
				{
					this->processWritingFd(it->fd);
				}
				else if (((it->revents & POLLERR) || (it->revents & POLLHUP)) && !_breakPollLoop)
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
		char	str_ip_server[INET_ADDRSTRLEN];
		struct sockaddr_in  server_addr = it->getServerAddr();
		inet_ntop (AF_INET, &(server_addr.sin_addr), str_ip_server, sizeof (str_ip_server));
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
	std::cout << TXT_MAG << TXT_B << getTimestamp() << "Client " << new_client.getId() << ":	New client connexion from " << str_ip_client << TXT_END << std::endl;
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
		if (client->receiveRequest())
			this->removeClient(client);
		break;
	case CGI_PIPE:
		if (client->addCgiToBuff())
		{
			std::cout << TXT_RED << getTimestamp() << "Client " << client->getId() << ":	Error while adding CGI to Res buffer... Removing the client !" << TXT_END << std::endl;
			this->removeClient(client);
		}
		break;
	case RES_FILE_FD:
		if (client->addBodyFileToBuff())
		{
			std::cout << TXT_RED << getTimestamp() << "Client " << client->getId() << ":	Error while adding Body File to Res buffer... Removing the client !" << TXT_END << std::endl;
			this->removeClient(client);
		}
		break;
	case UPLOAD_TMP_IN:
		client->resetLastActivity();
		client->transferUploadFileInSide();
		break;
	default:
		std::cout << TXT_RED << TXT_B << "Problem reading fd is not a correct category. Removing client number :" << client->getId()  << "    FD number: " << fd << " categorie = " << client->getSocketType(fd) << TXT_END << std::endl;
		client->printClientFd();
		this->removeClient(client);
	}
}

void	Launcher::processWritingFd(int fd)
{
	std::list<Client>::iterator client = this->findClient(fd);
	switch (client->getSocketType(fd))
	{
	case COM_SOCKET:
		client->sendResponse();
		if (client->getStatus() == RES_SENT && client->getKeepAlive() && client->getHttpRes() &&  client->getHttpRes()->getStatusCode() < 300)
		{
			std::cout << TXT_GREEN << getTimestamp() << "Client " << client->getId() << ":	Writing finised. Adding him to reading monitored sockets" << TXT_END << std::endl;
			client->resetClient();
			removeFdFromPoll(fd);
			addFdToPollIn(fd);
		}
		else if (client->getStatus() == RES_SENT || client->getStatus() == ERROR_WHILE_SENDING)
		{
			if (client->getStatus() == RES_SENT)
				std::cout << TXT_GREEN << getTimestamp() << "Client " << client->getId() << ":	Process finished. Client removed" << TXT_END << std::endl;
			else if (client->getStatus() == ERROR_WHILE_SENDING)
				std::cout << TXT_RED << getTimestamp() << "Client " << client->getId() << ":	An error happend while sending the response. Client removed" << TXT_END << std::endl;
			this->removeClient(client);
		}
		break;
	case REQ_FILE_FD:
		if (client->writeReqBodyFile())
		{
			std::cout << TXT_RED << getTimestamp() << "Client " << client->getId() << ":	An error happend while writing on the Req Tmp Body File. Client removed" << TXT_END << std::endl;
			this->removeClient(client);
		}
		break;
	case UPLOAD_OUT:
		client->resetLastActivity();
		client->transferUploadFileOutSide();
		break;
	default:
		std::cout << TXT_RED << TXT_B << "Problem writing fd is not a correct category. Removing client number :" << client->getId()  << "    FD number: " << fd << " categorie = " << client->getSocketType(fd) << TXT_END << std::endl;
		client->printClientFd();
		this->removeClient(client);
	}
}

void	Launcher::processCloseConnexionOrError(int fd)
{
	std::list<Client>::iterator client = this->findClient(fd);
	switch (client->getSocketType(fd))
	{
	case COM_SOCKET:
		std::cout << TXT_RED << getTimestamp() << "Client " << client->getId() << ":	Connexion has been closed" << TXT_END <<std::endl;
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
		if (time > static_cast<unsigned long>(it->getTimeOut()))
		{
			std::cout << TXT_MAG << getTimestamp() << "Client " << it->getId() << ":	Timeout, time = " << time << ". Client removed" << TXT_END << std::endl;
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
	if (it != _pollEvent.end())
	{
		_pollEvent.erase(it);
		_breakPollLoop = true;
	}
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

std::vector<Server>	&Launcher::getServers(void) {

	return (this->_servers);
}

std::list<Client>	&Launcher::getClients(void) {

	return (this->_clients);
}