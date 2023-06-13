/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 20:36:38 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/13 09:46:44 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "ConfParser.hpp"
#include "Server.hpp"
#include "Launcher.hpp"

void    signalHandler(int signum)
{
   std::cout << "Quitting webserv..." << std::endl;
   // liberer ce qui doit l'etre, et fermer toutes les connexions (ce qui necessitera surement de faire du Launcher une variable globale)
   // OU ALORS on envoie une exception, avec dans le catch un if qui return (0) si c'est cette exception la
   exit (0);
}

int main(int ac, char **av) {

    if (ac > 2)
    {
        std::cerr << "Error: Too many arguments" << std::endl;
        return (1);
    }
    signal(SIGINT, signalHandler);
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
