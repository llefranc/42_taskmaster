/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramBlock.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 15:17:16 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/08 15:19:24 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ProgramBlock.hpp"

ProgramBlock::ProgramBlock()
{

}

ProgramBlock::ProgramBlock(const ProgramBlock& cp)
{
	this->state_ = cp.getState();
	this->procs_ = cp.getProcs();
	this->name_ = cp.getName();
	this->cmd_ = cp.getCommand();
	this->numprocs_ = cp.getNumprocs();
	this->umask_ = cp.getUnamsk();
	this->workDir_ = cp.getWorkdir();
	this->autoStart_ = cp.getAutoStart();
	this->autoRestart_ = cp.getAutoRestart();
	this->startRetries_ = cp.getStartRetries();
	this->startTime_ = cp.getStartTime();
	this->exitCodes_ = cp.getExitCode();
	this->stopSignal_ = cp.getStopSignal();
	this->logOut_ = cp.getLogOut();
	this->logErr_ = cp.getLogErr();
	this->env_ = cp.getEnv();
}

ProgramBlock::~ProgramBlock()
{

}

ProgramBlock& ProgramBlock::operator=(const ProgramBlock& pb)
{
	(void)pb;
	return *this;
}

void ProgramBlock::setState(int state)
{
	state_ = state;
}

const int& ProgramBlock::getState() const
{
	return state_;
}

void ProgramBlock::setProcs(const std::vector<std::pair<int, long> >& proc)
{
	procs_ = proc;
}

const std::vector<std::pair<int, long> >& ProgramBlock::getProcs() const
{
	return procs_;
}

void ProgramBlock::setName(const std::string& name)
{
	name_ = name;
}

const std::string& ProgramBlock::getName() const
{
	return name_;
}

void ProgramBlock::setCommand(const std::string& cmd)
{
	cmd_=cmd;
}

const std::string& ProgramBlock::getCommand() const
{
	return cmd_;
}

void ProgramBlock::setNumprocs(const int numprocs)
{
	numprocs_ = numprocs;
}

const int& ProgramBlock::getNumprocs() const
{
	return numprocs_;
}

void ProgramBlock::setUnamsk(const std::string& umask)
{
	umask_ = umask;
}

const std::string& ProgramBlock::getUnamsk() const
{
	return umask_;
}

void ProgramBlock::setWorkdir(const std::string& workdir)
{
	workDir_ = workdir;
}

const std::string& ProgramBlock::getWorkdir() const
{
	return workDir_;
}

void ProgramBlock::setAutoStart(const bool autoStart)
{
	autoStart_ = autoStart;
}

const bool& ProgramBlock::getAutoStart() const
{
	return autoStart_;
}

void ProgramBlock::setAutoRestart(const int autoRestart)
{
	autoRestart_ = autoRestart;
}

const int& ProgramBlock::getAutoRestart() const
{
	return autoRestart_;
}

void ProgramBlock::setStartRetries(const int retries)
{
	startRetries_ = retries;
}

const int& ProgramBlock::getStartRetries() const
{
	return startRetries_;
}


void ProgramBlock::setStartTime(const int startTime)
{
	startTime_ = startTime;
}

const int& ProgramBlock::getStartTime() const
{
	return startTime_;
}

void ProgramBlock::setExitCode(const std::vector<int>& exitCode)
{
	exitCodes_ = exitCode;
}

const std::vector<int>& ProgramBlock::getExitCode() const
{
	return exitCodes_;
}

void ProgramBlock::setStopSignal(const int stopSignal)
{
	stopSignal_ = stopSignal;
}

const int& ProgramBlock::getStopSignal() const
{
	return stopSignal_;
}

void ProgramBlock::setLogOut(const std::string& logOut)
{
	logOut_ = logOut;
}

const std::string& ProgramBlock::getLogOut() const
{
	return logOut_;
}

void ProgramBlock::setLogErr(const std::string& logErr)
{
	logErr_ = logErr;
}

const std::string& ProgramBlock::getLogErr() const
{
	return logErr_;
}

void ProgramBlock::setEnv(const std::vector<std::string>& env)
{
	env_ = env;
}

const std::vector<std::string>& ProgramBlock::getEnv() const
{
	return env_;
}
