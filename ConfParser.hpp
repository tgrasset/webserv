#ifndef CONFPARSER_HPP

# define CONFPARSER_HPP
# include "webserv.hpp"
# include "Server.hpp"


class ConfParser {

    public:

        ConfParser(std::string path);
        ConfParser(ConfParser const &src);
        ~ConfParser();

        ConfParser                  &operator=(ConfParser const &rhs);

        std::string                 getPath() const;
        std::vector<Server>         getServers() const;
        std::vector<std::string>    getServerConf() const;
        int                         getServerNb() const;

        void                        parse();
        std::string                 extractContent(std::string const &path);
        void                        cleanComments(std::string &content) const;
        void                        splitServerBlocks(std::string &content);
        size_t                      getBlockStart(size_t blockStart, std::string &content) const;
        size_t                      getBlockEnd(size_t blockStart, std::string &content) const;
        void                        configurateServer(Server &server, std::string &config) const;
        void                        checkServerConfig(Server &server) const;

        class ConfParserException : public std::exception {
            public :
                ConfParserException(std::string errMessage) throw() {
					_errMessage = "Parse Error: " + errMessage;
				}
				virtual const char* what() const throw() {
					return (_errMessage.c_str());
				}
				~ConfParserException() throw() {}
            
            private:
                std::string _errMessage;
        };

    private:
        ConfParser();

        std::string                 _path;
        std::vector<Server>         _servers;
        std::vector<std::string>	_serverConf;
        int 						_serverNb;
		static bool					_verbose;
};

#endif