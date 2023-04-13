/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcInfo.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 17:05:25 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/13 12:14:34 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ProcInfo.hpp"

#include "ConfigParser.hpp"

#include <unordered_map>
#include <sstream>
#include <cstdlib>
#include <ctime>

#define DAYS_DIVIDER 86400
#define HOURS_DIVIDER 3600
#define MINUTES_DIVIDER 60

static const std::string stateStr[7] = {
	"STOPPED",
	"STARTING",
	"RUNNING",
	"BACKOFF",
	"STOPPING",
	"EXITED",
	"FATAL",
};

/* ex: programname_21, 21 corresponds to numprocs */
#define PROGRAM_NAME_MAX_LEN (CP_MAX_NAME_LEN + sizeof(":_xxx") + 10)

#define STATE_STR_MAX_LEN (sizeof("STARTING"))
#define STATUS_PADDING_LEN (PROGRAM_NAME_MAX_LEN + STATE_STR_MAX_LEN)

static inline std::string timeToStr(int time);
static inline std::string runningStateToStr(pid_t pid, std::time_t startTime);


/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

ProcInfo::ProcInfo() :
	state_(E_STATE_STOPPED),
	name_(),
	hash_(),
	pid_(-1),
	nbRestart_(0),
	spawnTime_(0),
	unSpawnTime_(0),
	exitCode_(0)
{}

/**
 * Create a ProcInfo with a name and generate its hash based on its name.
*/
ProcInfo::ProcInfo(const std::string &name) :
	state_(E_STATE_STOPPED),
	name_(name),
	pid_(-1),
	nbRestart_(0),
	spawnTime_(0),
	unSpawnTime_(0),
	exitCode_(0)
{
	std::stringstream stream;
	std::hash<int> hasher;
	size_t hashNb;

	srand(time(NULL));
	hashNb = hasher(rand());
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
	spawnTime_(c.spawnTime_),
	unSpawnTime_(c.unSpawnTime_),
	exitCode_(c.exitCode_)
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


long ProcInfo::getSpawnTime() const
{
	return spawnTime_;
}

long ProcInfo::getUnSpawnTime() const
{
	return unSpawnTime_;
}

uint8_t ProcInfo::getExitCode() const
{
	return exitCode_;
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


void ProcInfo::setSpawnTime(long spawnTime)
{
	spawnTime_ = spawnTime;
}

void ProcInfo::setUnSpawnTime(long unSpawnTime)
{
	unSpawnTime_ = unSpawnTime;
}

void ProcInfo::setExitCode(uint8_t exitCode)
{
	exitCode_ = exitCode;
}


/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */

/**
 * Return true if the process is eitheir in E_STATE_STARTING or E_STATE_RUNNING.
*/
bool ProcInfo::isRunning() const
{
	return state_ == E_STATE_STARTING || state_ == E_STATE_RUNNING;
}

/**
 * Udapte ProcInfo state from STARTING to RUNNING if the process is running
 * since long enough. If the process is in another state, does nothing.
 * @starttime: The starttime token of a program block from the config file.
*/
void ProcInfo::updateStartingState(std::time_t pbstarttime)
{
	time_t now = std::time(NULL);

	if (state_ == E_STATE_STARTING && now - spawnTime_ > pbstarttime)
		state_ = E_STATE_RUNNING;
}

/**
 * Return the ProcInfo state as a string for status command (with additional
 * info depending on the state). Possible values:
 *
 * program_name             STARTING
 * program_name             RUNNING    pid 1721, uptime 0:00:08
 * program_name             STOPPING
 * program_name             BACKOFF    Exited too quickly
 * program_name             FATAL      Exited too quickly
 * program_name             EXITED     Feb 23 05:26 PM
 * program_name             STOPPED    Not started
 * program_name             STOPPED    Feb 23 05:26 PM
*/
std::string ProcInfo::toStrStatus() const
{
	std::string str(PROGRAM_NAME_MAX_LEN, ' ');
	char buf[sizeof("mon dd hh:mm AM")] = {};

	for (size_t i = 0; i < name_.length() && i < str.length() - 1; ++i)
		str[i] = name_[i];

	str += stateStr[state_];
	if (state_ != E_STATE_STARTING || state_ != E_STATE_STOPPING) {
		str += std::string(STATUS_PADDING_LEN - str.length(), ' ');
	}
	if (state_ == E_STATE_FATAL && exitCode_ == EXIT_SPAWN_FAILED) {
		str += "Spawn failed";
	} else if (state_ == E_STATE_BACKOFF || state_ == E_STATE_FATAL) {
		str += "Exited too quickly";
	} else if (state_ == E_STATE_STOPPED && !spawnTime_) {
		str += "Not started";
	} else if (state_ == E_STATE_STOPPED || state_ == E_STATE_EXITED) {
		strftime(buf, sizeof(buf), "%b %d %I:%M %p",
			 std::localtime(&unSpawnTime_));
		str += buf;
	} else if (state_ == E_STATE_RUNNING) {
		str += runningStateToStr(pid_, spawnTime_);
	}
	return str;
}

/**
 * Return the ProcInfo state as a string for log file (with addional info
 * depending on the state). Possible values:
 * 
 * process started (pid: 166996)
 * process ended (state EXITED, pid: 166996)
 * process ended (state STOPPED, pid: 167119)
 * process ended (state BACKOFF, exited too quickly, pid: 171435)
 * process ended (state FATAL, exited too quickly, pid: 171435)
 * process ended (state FATAL, spawn failed, pid: 171435)
*/
std::string ProcInfo::toStrLog(int pidSaved) const
{
	std::string str = name_;

	if (state_ == E_STATE_STARTING) {
		str += ": process started (";
	} else if (state_ == E_STATE_BACKOFF || state_ == E_STATE_EXITED ||
	           state_ == E_STATE_FATAL || state_ == E_STATE_STOPPED) {
		str += ": process ended (state " + stateStr[state_] + ", ";

		if (state_ == E_STATE_FATAL && exitCode_ == EXIT_SPAWN_FAILED)
			str += "spawn failed, ";
		else if (state_ == E_STATE_BACKOFF || state_ == E_STATE_FATAL)
			str += "exited too quickly, ";
	}
	str += "pid: " + std::to_string(pidSaved) + ")\n";
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
	std::swap(a.spawnTime_, b.spawnTime_);
	std::swap(a.unSpawnTime_, b.unSpawnTime_);
	std::swap(a.exitCode_, b.exitCode_);
}

/**
 * Return hours, minutes or seconds as a string of 2 characters.
*/
static inline std::string timeToStr(int time)
{
	std::string str = std::to_string(time);

	if (str.length() == 1)
		str.insert(0, "0");
	return str;
}

/**
 * Calculate and return as a string the elapsed time since when a process runs.
*/
static inline std::string runningStateToStr(pid_t pid, std::time_t startTime)
{
	std::string str;
	std::time_t now = std::time(NULL);
	long diff = (long)difftime(now, startTime);
	int nbDays = diff / DAYS_DIVIDER;

	str += "pid " + std::to_string(pid) + ", uptime ";
	if (nbDays == 1)
		str += "1 day, ";
	else if (nbDays > 1)
		str += std::to_string(nbDays) + " days, ";

	diff %= DAYS_DIVIDER;
	str += timeToStr(diff / HOURS_DIVIDER) + ":";
	diff %= HOURS_DIVIDER;
	str += timeToStr(diff / MINUTES_DIVIDER) + ":" +
	       timeToStr(diff % MINUTES_DIVIDER);
	return str;
}