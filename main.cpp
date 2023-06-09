/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 20:36:38 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/09 20:42:47 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "ConfParser.hpp"
#include "Server.hpp"
#include "Launcher.hpp"

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
        
    Launcher launcher(path);
    try {
        launcher.parse();
        launcher.launch_servers();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    return (0);
}
