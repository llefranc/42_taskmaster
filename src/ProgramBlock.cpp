/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramBlock.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 15:17:16 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/19 18:03:44 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>

#include "ProgramBlock.hpp"

ProgramBlock::ProgramBlock() :
	state_(PB_STATE_NEW),
	procInfos_(),
	name_(),
	cmd_(),
	numProcs_(1),
	umask_("022"),
	workDir_(),
	autoStart_(false),
	autoRestart_(false),
	startRetries_(0),
	startTime_(0),
	exitCodes_(1, 0),
	stopSignal_(SIGTERM),
	logOut_("/tmp"),
	logErr_("/tmp"),
	env_()
{

}

ProgramBlock::ProgramBlock(const ProgramBlock &c) :
	state_(c.state_),
	procInfos_(c.procInfos_),
	name_(c.name_),
	cmd_(c.cmd_),
	numProcs_(c.numProcs_),
	umask_(c.umask_),
	workDir_(c.workDir_),
	autoStart_(c.autoStart_),
	autoRestart_(c.autoRestart_),
	startRetries_(c.startRetries_),
	startTime_(c.startTime_),
	exitCodes_(c.exitCodes_),
	stopSignal_(c.stopSignal_),
	logOut_(c.logOut_),
	logErr_(c.logErr_),
	env_(c.env_)
{}

ProgramBlock::~ProgramBlock() {}

ProgramBlock& ProgramBlock::operator=(ProgramBlock a)
{
	swap(*this, a);
	return *this;
}


/* ----------------------------------------------- */
/* ------------------- GETTERS ------------------- */

const int& ProgramBlock::getState() const
{
	return state_;
}

const std::vector<ProcInfo>& ProgramBlock::getProcInfos() const
{
	return procInfos_;
}

const std::string& ProgramBlock::getName() const
{
	return name_;
}


const std::string& ProgramBlock::getCmd() const
{
	return cmd_;
}


const int& ProgramBlock::getNumprocs() const
{
	return numProcs_;
}


const std::string& ProgramBlock::getUmask() const
{
	return umask_;
}


const std::string& ProgramBlock::getWorkDir() const
{
	return workDir_;
}


const bool& ProgramBlock::getAutoStart() const
{
	return autoStart_;
}


const int& ProgramBlock::getAutoRestart() const
{
	return autoRestart_;
}


const int& ProgramBlock::getStartRetries() const
{
	return startRetries_;
}


const int& ProgramBlock::getStartTime() const
{
	return startTime_;
}


const std::vector<int>& ProgramBlock::getExitCodes() const
{
	return exitCodes_;
}


const int& ProgramBlock::getStopSignal() const
{
	return stopSignal_;
}


const std::string& ProgramBlock::getLogOut() const
{
	return logOut_;
}


const std::string& ProgramBlock::getLogErr() const
{
	return logErr_;
}


const std::vector<std::string>& ProgramBlock::getEnv() const
{
	return env_;
}


/* ----------------------------------------------- */
/* ------------------- SETTERS ------------------- */

void ProgramBlock::setState(const int state)
{
	state_ = state;
}

void ProgramBlock::setProcInfos(const std::vector<ProcInfo> &procInfos)
{
	procInfos_ = procInfos;
}


void ProgramBlock::setName(const std::string& name)
{
	name_ = name;
}

void ProgramBlock::setCmd(const std::string& cmd)
{
	cmd_ = cmd;
}

void ProgramBlock::setNumProcs(const int numProcs)
{
	numProcs_ = numProcs;
}

void ProgramBlock::setUmask(const std::string& uMask)
{
	umask_ = uMask;
}

void ProgramBlock::setWorkDir(const std::string& workDir)
{
	workDir_ = workDir;
}

void ProgramBlock::setAutoStart(const bool autoStart)
{
	autoStart_ = autoStart;
}

void ProgramBlock::setAutoRestart(const int autoRestart)
{
	autoRestart_ = autoRestart;
}

void ProgramBlock::setStartRetries(const int startRetries)
{
	startRetries_ = startRetries;
}

void ProgramBlock::setStartTime(const int startTime)
{
	startTime_ = startTime;
}

void ProgramBlock::setExitCodes(const std::vector<int>& exitCodes)
{
	exitCodes_ = exitCodes;
}

void ProgramBlock::setStopSignal(const int stopSignal)
{
	stopSignal_ = stopSignal;
}

void ProgramBlock::setLogOut(const std::string& logOut)
{
	logOut_ = logOut;
}

void ProgramBlock::setLogErr(const std::string& logErr)
{
	logErr_ = logErr;
}

void ProgramBlock::setEnv(const std::vector<std::string>& env)
{
	env_ = env;
}


/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */

bool ProgramBlock::isCorrect() const
{
	return name_.length() && cmd_.length();
}

void ProgramBlock::clear()
{
	state_ = PB_STATE_NEW;
	procInfos_.clear();
	name_ = "";
	cmd_ = "";
	numProcs_ = 1;
	umask_ = "022";
	workDir_ = "";
	autoStart_ = false;
	autoRestart_ = false;
	startRetries_ = 0;
	startTime_ = 0;
	exitCodes_.clear();
	exitCodes_.push_back(0);
	stopSignal_ = SIGTERM;
	logOut_ = "/tmp";
	logErr_ = "/tmp";
	env_.clear();
}

#include <iostream>
void ProgramBlock::print() const
{
	std::cout << "state_=" << state_ << std::endl;
	int i = 0;
	for (std::vector<ProcInfo>::const_iterator it = procInfos_.begin();
			it != procInfos_.end(); ++it, ++i) {
		std::cout << "procInfo[" << i++ << "]=" << it->toString() << "\n";
	}
	std::cout << "name_=" << name_ << std::endl;
	std::cout << "cmd_=" << cmd_ << std::endl;
	std::cout << "numProcs_=" << numProcs_ << std::endl;
	std::cout << "umask_=" << umask_ << std::endl;
	std::cout << "workDir_=" << workDir_ << std::endl;
	std::cout << "autoStart_=" << autoStart_ << std::endl;
	std::cout << "autoRestart_=" << autoRestart_ << std::endl;
	std::cout << "startRetries_=" << startRetries_ << std::endl;
	std::cout << "startTime_=" << startTime_ << std::endl;

	i = 0;
	for (std::vector<int>::const_iterator it = exitCodes_.begin();
			it != exitCodes_.end(); ++it) {
		std::cout << "exitCodes_[" << i << "]=" << *it << "\n";
	}

	std::cout << "stopSignal_=" << stopSignal_ << std::endl;
	std::cout << "logOut_=" << logOut_ << std::endl;
	std::cout << "logErr_=" << logErr_ << std::endl;

	i = 0;
	for (std::vector<std::string>::const_iterator it = env_.begin();
			it != env_.end(); ++it) {
		std::cout << "env_[" << i << "]=" << *it << "\n";
	}
	std::cout << "--------------------------" << std::endl;
}


/* ----------------------------------------------- */
/* ---------------- HELPER FUNCS------------------ */

void swap(ProgramBlock &a, ProgramBlock &b)
{
	std::swap(a.state_, b.state_);
	std::swap(a.procInfos_, b.procInfos_);
	std::swap(a.name_, b.name_);
	std::swap(a.cmd_, b.cmd_);
	std::swap(a.numProcs_, b.numProcs_);
	std::swap(a.umask_, b.umask_);
	std::swap(a.workDir_, b.workDir_);
	std::swap(a.autoStart_, b.autoStart_);
	std::swap(a.autoRestart_, b.autoRestart_);
	std::swap(a.startRetries_, b.startRetries_);
	std::swap(a.startTime_, b.startTime_);
	std::swap(a.exitCodes_, b.exitCodes_);
	std::swap(a.stopSignal_, b.stopSignal_);
	std::swap(a.logOut_, b.logOut_);
	std::swap(a.logErr_, b.logErr_);
	std::swap(a.env_, b.env_);
}