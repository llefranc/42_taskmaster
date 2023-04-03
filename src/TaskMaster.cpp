/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TaskMaster.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:21:14 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/03 17:06:36 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TaskMaster.hpp"
#include "poll.h"
#include "unistd.h"
#include <string>
#include <string.h>


/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

TaskMaster::TaskMaster(char **env) :
		log_(NULL), configParser_(env), spawner_() {}

TaskMaster::~TaskMaster() {}


/* ----------------------------------------------- */
/* ------------------- GETTERS ------------------- */

Logger *TaskMaster::getLogger() const
{
	return log_;
}


/* ----------------------------------------------- */
/* ------------------- SETTERS ------------------- */

void TaskMaster::setLogger(Logger* log)
{
	log_ = log;
	spawner_.setLogger(log);
}


/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */

/**
 * Parse the configuration file for the first time.
*/
void TaskMaster::initConfigParser(const std::string &path)
{
	log_->iAll("Parsing configuration file (path: " + path + ")\n");
	pbList_ = configParser_.load(path);
	log_->iAll("Configuration file successfully loaded\n");

	for (std::list<ProgramBlock>::iterator it = pbList_.begin();
	    it != pbList_.end(); ++it) {
		it->print();
	}
}

/**
 * - Wait for a command from user and execute it.
 * - Execute the appropriate action when a child process dies.
*/
void TaskMaster::shellRoutine()
{
	int pollRet;
	char buf[256];
	struct pollfd pfd;

	pfd.fd = 0;
	pfd.events = POLLIN;

	log_->iUser("Launching shell\n");
	write(1, ">>>> ", sizeof(">>>> "));

	try {
		while (true)
		{
			pollRet = poll(&pfd, 1, 0);
			bzero(buf, sizeof(buf));
			if (pollRet & POLLIN)
			{
				int nb = read(0, buf, sizeof(buf));
				write(1, ">>>> ", sizeof(">>>> "));
				(void)nb;
				std::string shellLine(buf);
				if (!shellLine.find("start"))
					spawner_.startProgramBlock(*(pbList_.begin()));
			}
			else if (pollRet & (POLLERR | POLLNVAL))
			{
				throw std::runtime_error(std::string("Poll failed: ") + strerror(errno));
			}
			else
			{
			}
		}
	} catch (const std::exception &e) {

		throw;
	}

}
