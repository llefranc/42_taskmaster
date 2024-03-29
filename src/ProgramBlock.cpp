/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramBlock.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 15:17:16 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/13 10:41:40 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ProgramBlock.hpp"

#include <iostream>
#include <csignal>


/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

/**
 * Create a new ProgramBlock with default values for each field. See attributes
 * in ProgramBlock.hpp for more info.
*/
ProgramBlock::ProgramBlock() :
	state_(E_STATE_NEW),
	procInfos_(),
	name_(),
	cmd_(),
	numProcs_(1),
	umask_(0022), /* Octal value */
	workDir_(),
	autoStart_(false),
	autoRestart_(false),
	startRetries_(0),
	startTime_(0),
	exitCodes_(),
	stopSignal_(SIGTERM),
	stopTime_(1),
	logOut_(),
	logErr_(),
	env_()
{
	exitCodes_.insert(0);
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
	stopTime_(c.stopTime_),
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

std::vector<ProcInfo>& ProgramBlock::getProcInfos()
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


const int& ProgramBlock::getUmask() const
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


const std::set<int>& ProgramBlock::getExitCodes() const
{
	return exitCodes_;
}


const int& ProgramBlock::getStopSignal() const
{
	return stopSignal_;
}

const int& ProgramBlock::getStopTime() const
{
	return stopTime_;
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

void ProgramBlock::setUmask(const int uMask)
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

void ProgramBlock::setExitCodes(const std::set<int>& exitCodes)
{
	exitCodes_ = exitCodes;
}

void ProgramBlock::setStopSignal(const int stopSignal)
{
	stopSignal_ = stopSignal;
}

void ProgramBlock::setStopTime(const int stopTime)
{
	stopTime_ = stopTime;
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

/**
 * Return true if the ProgramBlock was corretly filled during config file
 * parsing, i.e. name and command where specified.
*/
bool ProgramBlock::isCorrect() const
{
	return name_.length() && cmd_.length();
}

/**
 * Indicate if a status code from an exited process of the program block should
 * lead to a restart or not.
*/
bool ProgramBlock::shouldAutoRestart(int status) const
{
	if (getAutoRestart() == ProgramBlock::E_AUTO_TRUE ||
	    (getAutoRestart() == ProgramBlock::E_AUTO_UNEXP &&
	    getExitCodes().find(status) == getExitCodes().end()))
		return true;
	return false;
}

/**
 * Reset the ProgramBlock to its default value.
*/
void ProgramBlock::clear()
{
	state_ = E_STATE_NEW;
	procInfos_.clear();
	name_ = "";
	cmd_ = "";
	numProcs_ = 1;
	umask_ = 0022; /* Octal value */
	workDir_ = "";
	autoStart_ = false;
	autoRestart_ = false;
	startRetries_ = 0;
	startTime_ = 0;
	exitCodes_.clear();
	exitCodes_.insert(0);
	stopSignal_ = SIGTERM;
	stopTime_ = 1;
	logOut_ = "";
	logErr_ = "";
	env_.clear();
}

/**
 * Print ProgramBlock content on stdout (usefull for debug).
*/
void ProgramBlock::print() const
{
	static const std::string stateStr[5] = {
		"E_STATE_NEW",
		"E_STATE_UNCHANGE",
		"E_STATE_CHANGE",
		"E_STATE_CHANGE_REMOVE",
		"E_STATE_REMOVE",
	};

	std::cout << "state_=" << stateStr[state_] << std::endl;
	int i = 0;
	for (std::vector<ProcInfo>::const_iterator it = procInfos_.begin();
			it != procInfos_.end(); ++it, ++i) {
		std::cout << "pInfo[" << i << "]=" << it->toStrStatus() << "\n";
	}
	std::cout << "name_=" << name_ << std::endl;
	std::cout << "cmd_=" << cmd_ << std::endl;
	std::cout << "numProcs_=" << numProcs_ << std::endl;
	std::cout << "umask_=" << std::oct << umask_ << std::dec << std::endl;
	std::cout << "workDir_=" << workDir_ << std::endl;
	std::cout << "autoStart_=" << autoStart_ << std::endl;
	std::cout << "autoRestart_=" << autoRestart_ << std::endl;
	std::cout << "startRetries_=" << startRetries_ << std::endl;
	std::cout << "startTime_=" << startTime_ << std::endl;

	i = 0;
	for (std::set<int>::const_iterator it = exitCodes_.begin();
			it != exitCodes_.end(); ++it, ++i) {
		std::cout << "exitCodes_[" << i << "]=" << *it << "\n";
	}

	std::cout << "stopSignal_=" << stopSignal_ << std::endl;
	std::cout << "stopTime_=" << stopTime_ << std::endl;
	std::cout << "logOut_=" << logOut_ << std::endl;
	std::cout << "logErr_=" << logErr_ << std::endl;

	i = 0;
	for (std::vector<std::string>::const_iterator it = env_.begin();
			it != env_.end(); ++it, ++i) {
		std::cout << "env_[" << i << "]=" << *it << "\n";
	}
	std::cout << "--------------------------" << std::endl;
}

/**
 * Return the ProcInfo matching a given PID. NULL if no match.
*/
ProcInfo *ProgramBlock::getProcInfoByPid(int pid)
{
	for (size_t i =0; i < procInfos_.size(); i++) {
		if (procInfos_[i].getPid() == pid)
			return &procInfos_[i];
	}
	return NULL;
}

/**
 * Return the ProcInfo matching a given process name. NULL if no match.
*/
ProcInfo *ProgramBlock::getProcInfoByName(const std::string &name)
{
	for (size_t i =0; i < procInfos_.size(); i++) {
		if (procInfos_[i].getName() == name)
			return &procInfos_[i];
	}
	return NULL;
}


/* ----------------------------------------------- */
/* ------- NON-MEMBER FUNCTION OVERLOADS --------- */

bool operator==(const ProgramBlock& lhs, const ProgramBlock& rhs)
{
	return lhs.name_ == rhs.name_ &&
		lhs.cmd_ == rhs.cmd_ &&
		lhs.numProcs_ == rhs.numProcs_ &&
		lhs.umask_ == rhs.umask_ &&
		lhs.workDir_ == rhs.workDir_ &&
		lhs.autoStart_ == rhs.autoStart_ &&
		lhs.autoRestart_ == rhs.autoRestart_ &&
		lhs.startRetries_ == rhs.startRetries_ &&
		lhs.startTime_ == rhs.startTime_ &&
		lhs.exitCodes_ == rhs.exitCodes_ &&
		lhs.stopSignal_ == rhs.stopSignal_ &&
		lhs.stopTime_ == rhs.stopTime_ &&
		lhs.logOut_ == rhs.logOut_ &&
		lhs.logErr_ == rhs.logErr_ &&
		lhs.env_ == rhs.env_;
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
	std::swap(a.stopTime_, b.stopTime_);
	std::swap(a.logOut_, b.logOut_);
	std::swap(a.logErr_, b.logErr_);
	std::swap(a.env_, b.env_);
}