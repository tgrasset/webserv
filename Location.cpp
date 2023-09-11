/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/12 15:27:21 by mbocquel          #+#    #+#             */
/*   Updated: 2023/09/11 11:19:16 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

bool	Location::_verbose = false;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
Location::Location(void) 
{
	if (Location::_verbose)
		std::cout << "Location default constructor called" << std::endl;
	_path = "";
	_root = "";
	_index = "";
	_autoindex = false;
	_cgiLocation = false;
	_redirectionCode = 0;
	_redirection = "";
	_uploadDir = "";
}

Location::Location(Location const &src) 
{
	if (this != &src)
        *this = src;
	if (Location::_verbose)
		std::cout << "Location copy constructor called" << std::endl;
}

Location::~Location() 
{
	if (Location::_verbose)
		std::cout << "Location destructor called" << std::endl;
}

/* ************************************************************************** */
/*                     Surcharge d'operateur                                  */
/* ************************************************************************** */
Location &Location::operator=(Location const &rhs) 
{
	if (this != &rhs)
	{
		_path = rhs.getPath();
		_root = rhs.getRoot();
		_index = rhs.getIndex();
		_autoindex = rhs.getAutoIndex();
		_methods = rhs.getMethods();
		_cgiLocation = rhs.getCgiBool();
		_redirectionCode = rhs.getRedirectionCode();
		_redirection = rhs.getRedirectionPath();
		_cgiExtensionAndPath = rhs.getCgiExtensionAndPath();
		_uploadDir = rhs.getUploadDir();
	}
    return (*this);
}

/* ************************************************************************** */
/*                     Methodes                                               */
/* ************************************************************************** */
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

bool	Location::getCgiBool(void) const {

	return (_cgiLocation);
}

unsigned int	Location::getRedirectionCode(void) const {

	return (_redirectionCode);
}

std::string	Location::getRedirectionPath(void) const {

	return (_redirection);
}

std::map<std::string, std::string>	Location::getCgiExtensionAndPath(void) const {

	return (_cgiExtensionAndPath);
}

std::string	Location::getUploadDir(void) const {

	return (_uploadDir);
}

void	Location::setPath(std::string path) {

	_path = path;
}

void	Location::setRoot(std::string root, std::string serverRoot) {

	if (isValidConfValue(root) == false)
		throw LocationException("';' symbol needed after 'root' line in 'location' context");
	struct stat test;
	if (stat(root.c_str(), &test) == 0 && test.st_mode & S_IFDIR)
	{
		_root = root;
		return ;
	}
	std::string root2 = serverRoot + "/" + root;
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

void	Location::setCgiBool(bool cgi) {

	_cgiLocation = cgi;
}

void	Location::setCgiExtensionAndPath(std::vector<std::string> cgiInfo) {

	if (cgiInfo.empty() == true)
		throw LocationException("'cgi' directive with appropriate information needed in 'cgi-bin' location");
	size_t size = cgiInfo.size();
	std::string ext;
	std::string path;

	if (size % 2 != 0)
		throw LocationException("Listing after 'cgi' directive must follow pattern : <file_extension> <cgi_path>");
	if (size == 4 && cgiInfo[0] == cgiInfo[2])
		throw LocationException("Duplicate file extension after 'cgi' directive in 'cgi-bin' location");
	if (size > 4)
		throw LocationException("Only 'php and 'python' CGI can be handled by this server");
	for (size_t i = 0 ; i < size ; i++)
	{
		if (i % 2 == 0)
		{
			ext = cgiInfo[i];
			if (ext != ".py" && ext != ".php")
				throw LocationException("Unknown cgi extension : " + ext);
		}
		else
		{
			path = cgiInfo[i];
			if (i == cgiInfo.size() - 1)
			{
				if (isValidConfValue(path) == false)
					throw LocationException("';' symbol needed at the end of 'cgi' list");
			}
			if (checkFile(path, "/") == false)
				throw LocationException("Invalid cgi path : " + path);
			_cgiExtensionAndPath.insert(std::make_pair(ext, path));
		}
	}
}

void	Location::setRedirectionCode(int code) {

	_redirectionCode = code;
}

void	Location::setRedirectionPath(std::string path) {

	_redirection = path;
}

void	Location::setUploadDir(std::string dir) {

	_uploadDir = dir;
}

void	Location::checkConfig(std::string serverRoot) {

	if (_cgiLocation == true && _index == "")
		throw LocationException("Missing 'index' directive in 'cgi-bin' location");
	if (_path[0] != '/')
		throw LocationException("Invalid path format after 'location' directive");
	if (_root == "")
		_root = serverRoot;
	if (this->getMethods().empty() == true)
	{
		_methods.push_back("GET");
		_methods.push_back("POST");
		_methods.push_back("DELETE");
	}
	if (_root == serverRoot && _autoindex == false && _index != "" && checkFile(_index, _root + _path) == false)
			throw LocationException("Index file in 'location' context doesn't exist or couldn't be read");
	else if (_root != serverRoot && _autoindex == false && _index != "" && checkFile(_index, _root) == false)
		throw LocationException("Index file in 'location' context doesn't exist or couldn't be read");
	if (_uploadDir != "")
	{
		struct stat test;
		std::string dirPath = _root + "/" + _uploadDir;
		if (stat(dirPath.c_str(), &test) != 0 || !S_ISDIR(test.st_mode) || access(dirPath.c_str(), R_OK | W_OK) != 0)
			throw LocationException("Invalid upload directory path at the end of 'upload_dir' line");
		_uploadDir = dirPath;
	}
	if (_autoindex == true && _index != "")
		throw LocationException("A location can't have an default index if autoindex is on");
}