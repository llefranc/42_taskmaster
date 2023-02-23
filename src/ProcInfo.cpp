/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcInfo.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 17:05:25 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/22 11:47:56 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ProcInfo.hpp"

#include <unordered_map>
#include <sstream>


/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

ProcInfo::ProcInfo() :
	state_(PC_STATE_STOP),
	name_(),
	hash_(),
	pid_(0),
	nbRestart_(0),
	startTime_(0),
	endTime_(0)
{}

ProcInfo::ProcInfo(const std::string &name) :
	state_(PC_STATE_STOP),
	name_(name),
	pid_(0),
	nbRestart_(0),
	startTime_(0),
	endTime_(0)
{
	std::stringstream stream;
	std::hash<std::string> hasher;
	size_t hashNb = hasher(name);

	stream << std::hex << hashNb;
	hash_ = stream.str();
}

ProcInfo::~ProcInfo() {}

ProcInfo::ProcInfo(const ProcInfo& c) :
	state_(c.state_),
	name_(c.name_),
	hash_(c.hash_),
	pid_(c.pid_),
	nbRestart_(c.nbRestart_),
	startTime_(c.startTime_),
	endTime_(c.endTime_)
{}

ProcInfo& ProcInfo::operator=(ProcInfo a)
{
	swap(*this, a);
	return *this;
}


/* ----------------------------------------------- */
/* ------------------- GETTERS ------------------- */

int ProcInfo::getState() const
{
	return state_;
}

const std::string& ProcInfo::getName() const
{
	return name_;
}

const std::string& ProcInfo::getHash() const
{
	return hash_;
}

int ProcInfo::getPid() const
{
	return pid_;
}

int ProcInfo::getNbRestart() const
{
	return nbRestart_;
}


long ProcInfo::getStartTime() const
{
	return startTime_;
}

long ProcInfo::getEndTime() const
{
	return endTime_;
}


/* ----------------------------------------------- */
/* ------------------- SETTERS ------------------- */

void ProcInfo::setState(int state)
{
	state_ = state;
}

void ProcInfo::setName(const std::string &name)
{
	name_ = name;
}

void ProcInfo::setHash(const std::string &hash)
{
	hash_ = hash;
}

void ProcInfo::setPid(int pid)
{
	pid_ = pid;
}

void ProcInfo::setNbRestart(int nb)
{
	nbRestart_ = nb;
}


void ProcInfo::setStartTime(long start)
{
	startTime_ = start;
}

void ProcInfo::setEndTime(long end)
{
	endTime_ = end;
}


/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */

// rendu attendu:
// process1                         RUNNING    pid 935, uptime 17386 days, 14:52:25
// process2                         RUNNING    pid 936, uptime 17386 days, 14:52:25
// process3                         RUNNING    pid 31907, uptime 0:00:09
std::string ProcInfo::toString() const
{
	std::string str(30, ' ');
	static const std::string stateStr[5] = {
		"STOP",
		"CRASHED",
		"STARTING",
		"RUNNING",
		"FATAL"
	};

	for (size_t i = 0; i < name_.length() && i < str.length() - 1; ++i)
		str[i] = name_[i];
	str += stateStr[state_];

	size_t len = 40 - str.length();
	std::string padding(len, ' ');
	str += padding;

	str += "pid |";
	return str;
}


/* ----------------------------------------------- */
/* ---------------- HERLPER FUNCS ---------------- */

void swap(ProcInfo &a, ProcInfo &b)
{
	std::swap(a.state_, b.state_);
	std::swap(a.name_, b.name_);
	std::swap(a.hash_, b.hash_);
	std::swap(a.pid_, b.pid_);
	std::swap(a.nbRestart_, b.nbRestart_);
	std::swap(a.startTime_, b.startTime_);
	std::swap(a.endTime_, b.startTime_);
}