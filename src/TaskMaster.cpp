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

#include "Taskmaster.hpp"


int g_nbSigChldReceived = 0;

/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

TaskMaster::TaskMaster() : log_(), configParser_() {}

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
	int ret;

	ret = configParser_.load(&log_, path);

	if (ret == ConfigParser::ERR_COULD_NOT_OPEN_FILE) {
		throw std::runtime_error("[FATAL] Failed to open config"
				" file (path: " + path + ")\n");
	} else if (ret == ConfigParser::ERR_PARSING) {
		throw std::runtime_error("[FATAL] Error while parsing "
				"configuration file (path: " + path + ")\n");
	}
}

void TaskMaster::shellRoutine() {
	log_.iUser("Launching shell\n");
}