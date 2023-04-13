/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 15:24:25 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/13 10:24:32 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <csignal>

#include "TaskMaster.hpp"

volatile int g_sigFlag = 0;

void signalHandler(int signal)
{
	switch (signal){
		case SIGCHLD:
			g_sigFlag |= SCHLD;
			break;
		case SIGHUP:
			g_sigFlag |= SHUP;
			break;
		case SIGINT:
		case SIGQUIT:
		case SIGTERM:
			g_sigFlag |= SEXIT;
			break;
	}
}

void enableSignals(void)
{
	signal(SIGHUP, &signalHandler);
	signal(SIGCHLD, &signalHandler);
	signal(SIGINT, &signalHandler);
	signal(SIGQUIT, &signalHandler);
	signal(SIGTERM, &signalHandler);
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
	enableSignals();

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
	return 0;
}