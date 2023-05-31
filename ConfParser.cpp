#include "ConfParser.hpp"

ConfParser::ConfParser(std::string path) : _path(path) {

    _serverNb = 0;
}

ConfParser::ConfParser(ConfParser const &src) {

    if (this != &src)
        *this = src;
}

ConfParser::~ConfParser() {

}

ConfParser &ConfParser::operator=(ConfParser const &rhs) {

    _path = rhs.getPath();
    _servers = rhs.getServers();
    _serverConf = rhs.getServerConf();
    _serverNb = rhs.getServerNb();
    return (*this);
}

std::string ConfParser::getPath() const {

    return (_path);
}

std::vector<Server> ConfParser::getServers() const {

    return (_servers);
}

std::vector<std::string>    ConfParser::getServerConf() const {

    return (_serverConf);
}

int ConfParser::getServerNb() const {

    return (_serverNb);
}

std::string ConfParser::extractContent(std::string const &path) {

    struct stat buf;
    int ret = stat(path.c_str(), &buf);
    std::ifstream fileStream;

    if (ret < 0 || !S_ISREG(buf.st_mode))
        throw ConfParserException("Invalid path");
    if (access(path.c_str(), R_OK) != 0)
        throw ConfParserException("No reading rights on specified file");
    fileStream.open(path.c_str());
    if (fileStream.is_open() == false)
        throw ConfParserException("Configuration file could not be opened");

    std::stringstream result;
    result << fileStream.rdbuf();
    fileStream.close();
    return (result.str());
}

void    ConfParser::cleanComments(std::string &content) const {

    size_t sharp = content.find('#');
    while (sharp != std::string::npos)
	{
		size_t nl;
		nl = content.find('\n', sharp);
		content.erase(sharp, nl - sharp);
		sharp = content.find('#');
	}
}

size_t  ConfParser::getBlockStart(size_t blockStart, std::string &content) const {

    size_t i;

    for (i = blockStart; content[i] != '\0'; i++)
    {
        if (std::isspace(content[i]))
            continue ;
        else if (content[i] == 's')
            break;
        else
            throw ConfParserException("Unknown directive or unexpected symbol in main context");
    }
    if (content[i] == '\0')
        return (blockStart);
    if (content.compare(i, 6, "server") != 0)
        throw ConfParserException("Unknown directive or unexpected symbol in main context");
    i += 6;
    while (content[i] != '\0' && std::isspace(content[i]))
        i++;
    if (content[i] == '{')
        return (i);
    throw ConfParserException("Expected '{' symbol after server directive");
}

size_t  ConfParser::getBlockEnd(size_t blockStart, std::string &content) const {

    size_t i;
    int subContext = 0;

    for (i = blockStart + 1; content[i] != '\0'; i++)
    {
        if (content[i] == '{')
            subContext++;
        else if (content[i] == '}' && subContext > 0)
            subContext--;
        else if (content[i] == '}' && subContext == 0)
            return (i);
    }
    return (blockStart);
}

void    ConfParser::splitServerBlocks(std::string &content) {

    size_t blockStart = 0;
    size_t blockEnd = 1;
    size_t i = 0;

    if (content.find("server") == std::string::npos)
        throw ConfParserException("No server block found in configuration file");
    while (blockStart != blockEnd && blockStart < content.length())
    {
        blockStart = getBlockStart(blockStart, content);
        blockEnd = getBlockEnd(blockStart, content);
        if (blockStart == blockEnd)
        {
            for (i = blockStart; content[i] != '\0'; i++)
            {
                if (std::isspace(content[i]) == 0)      //en cas de whitespaces en fin de fichier
                    throw ConfParserException("Something seems wrong with curved brackets scopes");
            }
            break ;
        }
        _serverConf.push_back(content.substr(blockStart, blockEnd - blockStart + 1));
        _serverNb++;
        blockStart = blockEnd + 1;
    }
}

void    ConfParser::configurateServer(Server &server, std::string &config) const {

    std::vector<std::string> params = cpp_split(config, " \n\t");
    int size = params.size();
    if (size < 3)
        throw ConfParserException("Server context can't be left empty");
    bool    inLocations = false;
    
    for (int i = 0; i < size; i++)
    {
        if (params[i].compare("listen") == 0 && i + 1 < size && inLocations == false)
        {
            if (server.getPort() != 0)
                throw ConfParserException("Each server context can't have more than one 'listen' directive");
            server.setPort(params[++i]);
        }
        else if (params[i] == "server_name" && i + 1 < size && inLocations == false)
        {
            if (server.getServerName() != "")
                throw ConfParserException("Each server context can't have more than one 'server_name' directive");
            server.setServerName(params[++i]);
        }
        else if (params[i] == "host" && i + 1 < size && inLocations == false)
        {
            if (server.getHost() != 0)
                throw ConfParserException("Each server context can't have more than one 'host' directive");
            server.setHost(params[++i]);
        }
        else if (params[i] == "root" && i + 1 < size && inLocations == false)
        {
            if (server.getRoot() != "")
                throw ConfParserException("Each server context can't have more than one 'root' directive");
            server.setRoot(params[++i]);
        }
        else if (params[i] == "client_max_body_size" && i + 1 < size && inLocations == false)
        {
            if (server.getClientMaxBodySize() != 0)
                throw ConfParserException("Each server context can't have more than one 'client_max_body_size' directive");
            server.setClientMaxBodySize(params[++i]);
        }
        else if (params[i] == "index" && i + 1 < size && inLocations == false)
        {
            if (server.getIndex() != "")
                throw ConfParserException("Each server context can't have more than one 'index' directive");
            server.setIndex(params[++i]);
        }
        else if (params[i] == "error_page" && i + 1 < size && inLocations == false)
        {
            std::vector<std::string> errorPages;
            i++;
            while (i < size)
            {
                errorPages.push_back(params[i]);
                if (params[i].find(';') != std::string::npos)
                    break ;
                i++;
            }
            if (i == size)
                throw ConfParserException("Missing ';' symbol after error_page line");
            server.setErrorPages(errorPages);
        }
        else if (params[i] == "location" && i + 1 < size)
        {
            std::string path;
            std::vector<std::string> content;
            i++;
            if (params[i] == "{" || params[i] == "}")
                throw ConfParserException("Missing path after 'location' directive");
            path = params[i];
            i++;
            if (params[i] != "{")
                throw ConfParserException("Missing '{' symbol after 'location' directive");
            i++;
            for (; i < size && params[i] != "}"; i++)
                content.push_back(params[i]);
            if (i == size)
                throw ConfParserException("Missing '}' symbol at the end of 'location' block");
            server.setLocations(path, content);
            inLocations = true;
        }
        else if (params[i] != "{" && params[i] != "}")
        {
            if (inLocations == true)
                throw ConfParserException("'location' blocks must be at the end of 'server' context");
            else
                throw ConfParserException("Unknown directive : " + params[i]);
        }
    }
}

bool    ConfParser::checkFile(std::string const &index, std::string const &root) const {

    struct stat test;

	if (stat(index.c_str(), &test) == 0 && test.st_mode & S_IFREG && access(index.c_str(), R_OK) == 0)
        return (true);
    std::string full = root + index;
    if (stat(full.c_str(), &test) == 0 && test.st_mode & S_IFREG && access(full.c_str(), R_OK) == 0)
        return (true);
    return (false);
}

void    ConfParser::checkServerConfig(Server &server) const {

    if (server.getRoot() == "")
        server.setRoot("/;");
    if (server.getHost() == 0)
        server.setHost("localhost;");
    if (server.getIndex() == "")
        server.setIndex("index.html;");
    if (server.getPort() == 0)
        throw ConfParserException("Missing port, please add 'listen' directive");
    if (checkFile(server.getIndex(), server.getRoot()) == false)
        throw ConfParserException("The index specified doesn't exist or couldn't be read");
    //ajouter les locations et les error pages
}

void    ConfParser::parse() {

    std::string content = extractContent(_path);
    if (content.empty() == true)
        throw ConfParserException("Configuration file is empty");
    cleanComments(content);
    splitServerBlocks(content);
    for (int i = 0; i < _serverNb; i++)
    {
        Server newServer;
        configurateServer(newServer, _serverConf[i]);
        checkServerConfig(newServer);
        _servers.push_back(newServer);
    }
    //verifier si 2 serveurs ont pas meme port ET meme host ET meme server_name
}