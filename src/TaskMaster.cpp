/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TaskMaster.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:21:14 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/22 09:50:50 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Taskmaster.hpp"


int g_nbSigChldReceived = 0;

/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

TaskMaster::TaskMaster() : log_(NULL), configParser_() {}

TaskMaster::~TaskMaster() {}


/* ----------------------------------------------- */
/* ------------------- GETTERS ------------------- */

Logger* TaskMaster::getLogger() const
{
	return log_;
}


/* ----------------------------------------------- */
/* ------------------- SETTERS ------------------- */

void TaskMaster::setLogger(Logger* log)
{
	log_ = log;
}


/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */

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

void TaskMaster::shellRoutine() {
	log_->iUser("Launching shell\n");
}