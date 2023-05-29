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
    return (*this);
}

std::string ConfParser::getPath() const {

    return (_path);
}

std::vector<Server> ConfParser::getServers() const {

    return (_servers);
}

std::string ConfParser::extractContent(std::string const &path) {

    struct stat buf;
    int ret = stat(path.c_str(), &buf);
    std::ifstream fileStream;

    if (ret < 0 || !S_ISREG(buf.st_mode))
        throw ConfParserException("Invalid path");
    if (access(path.c_str(), R_OK) != 0)
        throw ConfParserException("No reading rights on specified file");
    fileStream.open(path);
    if (fileStream.is_open() == false)
        throw ConfParserException("Configuration file could not be opened");

    std::stringstream result;
    result << fileStream.rdbuf();
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
            throw ConfParserException("Unknown directive out of server scope");
    }
    if (content[i] == '\0')
        return (blockStart);
    if (content.compare(i, 6, "server") != 0)
        throw ConfParserException("Unknown directive out of server scope");
    i += 6;
    while (content[i] != '\0' && std::isspace(content[i]))
        i++;
    if (content[i] == '{')
        return (i);
    throw ConfParserException("Expected '{' symbol after server directive");
}

size_t  ConfParser::getBlockEnd(size_t blockStart, std::string &content) const {

    size_t i;
    int subBlock = 0;

    for (i = blockStart + 1; content[i] != '\0'; i++)
    {
        if (content[i] == '{')
            subBlock++;
        else if (content[i] == '}' && subBlock > 0)
            subBlock--;
        else if (content[i] == '}' && subBlock == 0)
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
                    throw ConfParserException("Something seems wrong with server scopes");
            }
            return ;
        }
        _serverConf.push_back(content.substr(blockStart, blockEnd - blockStart + 1));
        _serverNb++;
        blockStart = blockEnd + 1;
    }
    for (i = 0; i < _serverNb; i++)
    {
        Server newServer;
        initServerConf(newServer, _serverConf[i]);
        _servers.push_back(newServer);
    }
    //verifier si 2 serveurs ont pas meme port, meme server_name et meme host
}

void    ConfParser::parse() {

    std::string content = extractContent(_path);
    if (content.empty() == true)
        throw ConfParserException("Configuration file is empty");
    cleanComments(content);
    splitServerBlocks(content);
}