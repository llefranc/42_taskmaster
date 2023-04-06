/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcInfo.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/17 17:05:25 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/06 14:36:18 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ProcInfo.hpp"

#include <unordered_map>
#include <sstream>

#define STATUS_PADDING_LEN 40

#define DAYS_DIVIDER 86400
#define HOURS_DIVIDER 3600
#define MINUTES_DIVIDER 60

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
 * Udapte ProcInfo state from STARTING to RUNNING if the process is running
 * since long enough. If the process is in another state, does nothing.
 * @starttime: The starttime token of a program block from the config file.
*/
void ProcInfo::updateState(std::time_t pbstarttime)
{
	time_t now = std::time(NULL);

	if (state_ == E_STATE_STARTING && now - spawnTime_ > pbstarttime)
		state_ = E_STATE_RUNNING;
}

// http://supervisord.org/subprocess.html >> Pour les differents process states

/** ------- set pas le spawner -------- **/
// script                     STARTING
/*
Quand un process est start, il est en STARTING tant que
actual time - spwantime < starttime sachant que on pourra voir ca logiquement
que pour les autostart
*/

/** ------- set par status command -------- **/
// script                     RUNNING    pid 1721, uptime 0:00:08
/*
Quand son actual time - > start time
*/

/** ------- set par stop command -------- **/
// script                     STOPPING
/*
// A process transitions into the STOPPING state via an administrative stop
// request >>> Ne peut jamais arriver car stopping freeze le client et on wait
// stoptime secs avant d'envoyer sigkill donc nous on peut pas le voir dans notre
// (car on peut voire qu'en faisant status et la tant qu'on wait sur stoptime
// on ne peut pas run status.) On le voit avec le vrai supervisor uniquement
qunad on se connecte avec plusieurs clients.
*/


/** ------- set par unspawner -------- **/
// script                     BACKOFF    Exited too quickly
/*
Quand un process exit et qu'il etait en STARTING, il est alors mis en
backoff et l'os essaye de le restart si unexpected ? a verifier
*/

/** ------- set par unspawner -------- **/
// script                     FATAL      Exited too quickly
/*
Survient parce que start 4 fois d'affilee suite a 4 BACKOFF, et
que startretries etait de 3
if (nbstart > startretries)
        FATAL
*/

/** ------- set par unspawner -------- **/
// script                     EXITED     Feb 23 05:26 PM
/*
Quand un process exit et qu'il etait en RUNNING, il est alors mis en EXITED
que son exit code soit correct ou unexpected . On affiche alors le endtime.
*/

/** ------- set par config parser -------- **/
// script                     STOPPED    Not started
/*
Quand un process n'a jamais run il est en STOPPED + Not started. Son spwantime=0
*/

/** ------- set par stop command -------- **/
// script                     STOPPED    Feb 23 05:26 PM
/*
Quand un process a run une fois il est en stopped + endtime. Son
unspawntime=heure du stop.
*/

/**
 * Return the ProcInfo state as a string (with additional info depending on the
 * state). Possible values:
 * program_name             STARTING
 * program_name             RUNNING    pid 1721, uptime 0:00:08
 * program_name             STOPPING
 * program_name             BACKOFF    Exited too quickly
 * program_name             FATAL      Exited too quickly
 * program_name             EXITED     Feb 23 05:26 PM
 * program_name             STOPPED    Not started
 * program_name             STOPPED    Feb 23 05:26 PM
*/
std::string ProcInfo::toString() const
{
	static const std::string stateStr[7] = {
		"STOPPED",
		"STARTING",
		"RUNNING",
		"BACKOFF",
		"STOPPING",
		"EXITED",
		"FATAL",
	};
	std::string str(30, ' ');
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