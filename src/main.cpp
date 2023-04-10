/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 15:24:25 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/10 16:56:18 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <signal.h>

#include "TaskMaster.hpp"

volatile int g_nbProcessZombies = 0;
volatile int g_isSigHupReceived = 0;
int g_nbZombiesCleaned = 0;

void recvSigHup(int signal)
{
	if (signal == SIGHUP)
		g_isSigHupReceived = 1;
}

void recvSigChld(int signal)
{
	(void)signal;
	++g_nbProcessZombies;
}

int main(int ac, char **av, char **env)
{
	Logger log;
	TaskMaster taskMaster(env);

	if (ac != 3) {
		std::cerr << "[ERROR] Wrong number of arguments\n";
		std::cout << "Usage: taskmaster config-file log-file\n";
		return 1;
	}
	signal(SIGHUP, &recvSigHup);
	signal(SIGCHLD, &recvSigChld);

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
		return 1;
	}
	log.iAll("Quitting taskmaster\n");
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
