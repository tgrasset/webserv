#include "webserv.hpp"
#include "ConfParser.hpp"

int main(int ac, char **av) {

    if (ac > 2)
    {
        std::cerr << "Error: Too many arguments" << std::endl;
        return (1);
    }
    std::string path;
    if (ac == 1)
        path = "./configs/default.conf";
    else
        path = av[1];
    ConfParser confParser(path);
    try {
        confParser.parse();
        // TO BE CONTINUED...
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    return (0);
}