/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 15:24:25 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/08 16:03:50 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <vector>

#include "ProgramBlock.hpp"
#include "ProcInfo.hpp"
#include "Spawner.hpp"
#include "Logger.hpp"
#include "TaskMaster.hpp"

int nbProcessZombies = 0;
int nbZombiesCleaned = 0;

int main(int ac, char** av, char** env)
{
	(void)ac;
	(void)av;
	(void)env;
	try 
	{
		

		// spawner.startProcess(pInfo, prgB);
		TaskMaster tm;
		tm.initLogger("./log.txt");

		tm.initConfigParser("");
		tm.shellRoutine();

	}
	catch (std::runtime_error & e)
	{
		std::cerr << e.what()  << " : "  << strerror(errno) << std::endl;
	}

	return 0;
}


