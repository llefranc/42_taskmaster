/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TaskMaster.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:21:14 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/19 19:21:45 by llefranc         ###   ########.fr       */
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
	pbList_ = configParser_.load(path);
}

void TaskMaster::shellRoutine() {
	log_->iUser("Launching shell\n");
}