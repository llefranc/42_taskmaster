#include "ProcInfo.hpp"

ProcInfo::ProcInfo(std::string const& name, int state)
	: name_(name)
	, pid_(0)
	, state_(state)
	, nbRestart_(0)
	, startTime_(0)
	, endTime_(0)
{

}

ProcInfo::~ProcInfo()
{

}

ProcInfo::ProcInfo(ProcInfo& proc)
{
	(void)proc;
}

ProcInfo& ProcInfo::operator=(ProcInfo& proc)
{
	this->name_ = proc.getName();
	this->pid_ = proc.getPid();
	this->state_ = proc.getState();
	this->nbRestart_ = proc.getNbRestart();
	this->startTime_ = proc.getStartTime();
	this->endTime_ = proc.getEndTime();

	return *this;
}

std::string& ProcInfo::getName()
{
	return name_;
}

int ProcInfo::getPid()
{
	return pid_;
}

int ProcInfo::getNbRestart()
{
	return nbRestart_;
}

int ProcInfo::getState()
{
	return state_;
}

long ProcInfo::getStartTime()
{
	return startTime_;
}

long ProcInfo::getEndTime()
{
	return endTime_;
}


void ProcInfo::setName(std::string& name)
{
	name_ = name;
}

void ProcInfo::setPid(int pid)
{
	pid_ = pid;
}

void ProcInfo::setNbRestart(int nb)
{
	nbRestart_ = nb;
}

void ProcInfo::setState(int end)
{
	endTime_ = end;
}

void ProcInfo::setStartTime(long start)
{
	startTime_ = start;
}

void ProcInfo::setEndTime(long end)
{
	endTime_ = end;
}


void ProcInfo::toString()
{

}
