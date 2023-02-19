/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 15:24:25 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/19 17:52:17 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Taskmaster.hpp"

int main(int ac, char** av)
{
	TaskMaster taskMaster;

	if (ac != 3) {
		std::cerr << "[FATAL] Wrong number of arguments\n";
		std::cout << "Usage: taskmaster config-file log-file\n";
		return 1;
	}

	try {
		taskMaster.initConfigParser(av[1]);
		taskMaster.initLogger(av[2]);
		taskMaster.shellRoutine();
	} catch (const std::runtime_error &e) {
		std::cerr << e.what();
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

// 	vecpci.clear();
// 	a.setProcInfos(vecpci);
// 	b.print();
// 	c.print();
// 	a.print();


// }