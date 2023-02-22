/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 15:24:25 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/22 12:30:16 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "TaskMaster.hpp"

// int nbProcessZombies = 0; // Maintenant dans taskmaster
// int nbZombiesCleaned = 0; // Peut-etre la mettre dans taskmaster

int main(int ac, char **av, char **env)
{
	Logger log;
	TaskMaster taskMaster(env);

	if (ac != 3) {
		std::cerr << "[ERROR] Wrong number of arguments\n";
		std::cout << "Usage: taskmaster config-file log-file\n";
		return 1;
	}

	try {
		log.init(av[2]);
	} catch (const std::runtime_error &e) {
		std::cerr << e.what();
		std::cout << "[INFO ] Taskmaster exited unexpectedly\n";
		return 1;
	}
	log.iAll("Starting taskmaster\n");
	log.iUser("Logger initialized (path: " + log.getPath() + ")\n");
	taskMaster.setLogger(&log);

	try {
		taskMaster.initConfigParser(av[1]);
		taskMaster.shellRoutine();
	} catch (const std::runtime_error &e) {
		log.eAll(e.what());
		log.iAll("Taskmaster exited unexpectedly\n");

		// (Lucas to Helene: il vaudrait mieux integrer sterror(errno) au moment
		// du throw de l'exception, car errno ne sera pas forcement set a chaque
		// throw)
		// std::cerr << e.what()  << " : "  << strerror(errno) << std::endl;
		return 1;
	}

	return 0;
}

/**
 * TESTS PROGRAM_BLOCK
*/
// #include "ProgramBlock.hpp"
// #include "ProcInfo.hpp"

// int main(int ac, char **av)
// {
// 	(void)ac;
// 	(void)av;

// 	std::vector<int> vec;
// 	vec.push_back(2);
// 	vec.push_back(4);
// 	vec.push_back(0);
// 	vec.push_back(100);

// 	std::vector<std::string> vecstr;
// 	vecstr.push_back("SALUT");
// 	vecstr.push_back("YOOOOOO");
// 	vecstr.push_back("HELLOOOOO");

// 	// tester programblock
// 	ProgramBlock a;

// 	a.print();
// 	ProcInfo pci("procinfo_name", 3, 22, 43);
// 	ProcInfo pci2("procinfo_name 2", 3, 22, 43);
// 	std::vector<ProcInfo> vecpci;
// 	vecpci.push_back(pci);
// 	vecpci.push_back(pci2);

// 	a.setProcInfos(vecpci);
// 	a.setExitCodes(vec);
// 	a.setEnv(vecstr);
// 	a.print();

// 	ProgramBlock b(a);
// 	ProgramBlock c;
// 	c = a;

// 	b.print();
// 	c.print();
// 	a.print();
// 	a.clear();
// 	a.print();
// 	return 0;
// }
