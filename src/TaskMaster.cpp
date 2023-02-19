/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TaskMaster.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:21:14 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/17 14:41:12 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TaskMaster.hpp"
#include "poll.h"
#include "unistd.h"
#include <string>
#include <string.h>

int g_nbSigChldReceived = 0;

/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

TaskMaster::TaskMaster() : log_(), configParser_(), spawner_(log_)
{
}

TaskMaster::~TaskMaster() {}


/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */

void TaskMaster::initLogger(const std::string &logPath)
{
	log_.init(logPath);
	log_.iUser("Logger initialized (path: " + log_.getPath() + ")\n");
}

void TaskMaster::initConfigParser(const std::string &path)
{
	(void)path;
	ProgramBlock prgB;
	ProcInfo pInfo("script", 0, 0, 0);

	prgB.setState(ProgramBlock::PB_STATE_NEW);
	prgB.setName("script");
	prgB.setCmd("script.sh");
	prgB.setLogErr(".");
	prgB.setLogOut(".");
	prgB.setWorkDir(".");
	
	prgB.getProcInfos().push_back(pInfo);
	pb_.push_back(prgB);

	// int ret;

	// ret = configParser_.load(&log_, path);

	// if (ret == ConfigParser::ERR_COULD_NOT_OPEN_FILE) {
	// 	throw std::runtime_error("[FATAL] Failed to open config"
	// 			" file (path: " + path + ")\n");
	// } else if (ret == ConfigParser::ERR_PARSING) {
	// 	throw std::runtime_error("[FATAL] Error while parsing "
	// 			"configuration file (path: " + path + ")\n");
	// }
}

void TaskMaster::shellRoutine() 
{
	int pollRet;
	char buf[256];
	struct pollfd pfd;

	pfd.fd = 0;
	pfd.events = POLLIN;
	
	log_.iUser("Launching shell\n");
	write(1, ">>>> ", sizeof(">>>> "));

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
				spawner_.startProgramBlock(*(pb_.begin()));
		}
		else if (pollRet & (POLLERR | POLLNVAL))
		{
			throw std::runtime_error(std::string("Poll failed: ") + strerror(errno));
		}
		else
		{
		}
	}
}
