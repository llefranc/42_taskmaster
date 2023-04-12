/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TaskMaster.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:21:14 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/12 15:47:47 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TaskMaster.hpp"

#include <algorithm>
#include <poll.h>
#include <cstring>
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>

#define RELOAD_ON true
#define RELOAD_OFF false

#define RESTART_ON true
#define RESTART_OFF false

extern volatile int g_sigFlag;

void printPbList(const std::list<ProgramBlock> &pbList); // TODO degager

/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

TaskMaster::TaskMaster(char **env) :
		log_(NULL), configParser_(env), spawner_() {}

TaskMaster::~TaskMaster() {}


/* ----------------------------------------------- */
/* ------------------- GETTERS ------------------- */

Logger *TaskMaster::getLogger() const
{
	return log_;
}


/* ----------------------------------------------- */
/* ------------------- SETTERS ------------------- */

void TaskMaster::setLogger(Logger* log)
{
	log_ = log;
	spawner_.setLogger(log);
}


/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */

/**
 * Parse the configuration file for the first time.
*/
void TaskMaster::initConfigParser(const std::string &path)
{
	log_->iAll("Parsing configuration file (path: " + path + ")\n");
	pbList_ = configParser_.load(path);
	log_->iAll("Configuration file successfully loaded\n");

	// for (std::list<ProgramBlock>::iterator it = pbList_.begin();
	//     it != pbList_.end(); ++it) {
	// 	it->print();
	// } // TODO REMOVE
}

/**
 * - Wait for a command from user and execute it.
 * - Execute the appropriate action when a child process dies.
*/
void TaskMaster::shellRoutine()
{
	int pollRet;
	std::string buf;
	std::vector<std::string> tokens;
	struct pollfd pfd = {.fd = 0, .events = POLLIN, .revents = 0};

	spawner_.autostart(pbList_);
	log_->iUser("Launching shell\n");
	execStatus(std::vector<std::string>(1, std::string("status")));
	log_->iUser("taskmaster> ");

	while (true)
	{
		pollRet = poll(&pfd, 1, 0);
		if (g_sigFlag ) {
			signalOccured(RELOAD_ON, RESTART_ON);
		}
		else if (pollRet & POLLIN) {
			getline(std::cin, buf);
			tokens = splitEntry(buf.c_str());
			execCmd(tokens);
			log_->iUser("taskmaster> ");
			buf.clear();
		}
		else if (pollRet & (POLLERR | POLLNVAL)) {
			throw std::runtime_error(std::string("Poll failed: ")
					+ strerror(errno));
		}
	}
}


/* ----------------------------------------------- */
/* --------------- PRIVATE METHODS --------------- */

/**
 * Split user entry into tokens using space as separator.
*/
std::vector<std::string> TaskMaster::splitEntry(const std::string line)
{
	int i = 0;
	int start;
	std::vector<std::string> v;

	while (line[i]) {
		while (line[i] && isspace(line[i]))
			++i;
		start = i;
		while (line[i] && !isspace(line[i]))
			++i;
		if (start != i)
			v.push_back(line.substr(start, i - start));
		if (v.size() > 2)
			break;
	}
	return v;
}

void TaskMaster::execCmd(const std::vector<std::string> &tokens)
{
	methodPtr m;

	if (tokens.size() > 0) {
		for (size_t i = 0; i < sizeof(cmdMeths_) / sizeof(*cmdMeths_);
		    ++i) {
			if (tokens[0] == cmdMeths_[i].first) {
				m = cmdMeths_[i].second;
				return (this->*m)(tokens);
			}
		}
		log_->eUser(tokens[0] + ": unknow command\n");
	}
}

void TaskMaster::execStatus(const std::vector<std::string> &tokens)
{
	std::time_t startTime;

	if (tokens.size() > 1) {
		log_->eUser("too many arguments\n");
		log_->iUser("Usage: status\n");
		return;
	}
	// printPbList(pbList_);
	for (std::list<ProgramBlock>::iterator it = pbList_.begin();
	    it != pbList_.end(); ++it) {
		for (size_t i = 0; i < it->getProcInfos().size(); ++i) {
			startTime = it->getStartTime();
			it->getProcInfos()[i].updateStartingState(startTime);
			log_->iUser(it->getProcInfos()[i].toStrStatus() + "\n");
		}
	}
}

void TaskMaster::execStart(const std::vector<std::string> &tokens)
{
	std::pair<ProgramBlock*, ProcInfo*> infoExec;

	if (tokens.size() == 1) {
		log_->eUser("missing process name\n");
		goto err;
	} else if (tokens.size() > 2) {
		log_->eUser("too many arguments\n");
		goto err;
	}
	getProgExecutionInfoByName(tokens[1], infoExec);

	if (infoExec.second == NULL) {
		log_->eUser(tokens[1] + ": no such process\n");
	} else if (infoExec.second->isRunning()) {
		log_->eUser(tokens[1] + ": process is already running\n");
	} else {
		spawner_.startProcess(*infoExec.second, *infoExec.first);

		if (waitProcStart(infoExec.second->getSpawnTime(),
		    infoExec.first->getStartTime(), *infoExec.second) == -1) {
			log_->eUser(tokens[1] + ": start failed\n");
		} else {
			log_->iUser("Process " + tokens[1] + " started\n");
		}
	}
	return;

err:
	log_->iUser("Usage: start [process_name]\n");
}

void TaskMaster::execStop(const std::vector<std::string> &tokens)
{
	std::pair<ProgramBlock*, ProcInfo*> infoExec;

	if (tokens.size() == 1) {
		log_->eUser("missing process name\n");
		goto err;
	} else if (tokens.size() > 2) {
		log_->eUser("too many arguments\n");
		goto err;
	}
	getProgExecutionInfoByName(tokens[1], infoExec);

	if (infoExec.second == NULL) {
		log_->eUser(tokens[1] + ": no such process\n");
	} else if (!infoExec.second->isRunning()) {
		log_->eUser(tokens[1] + ": process is not running\n");
	} else {
		spawner_.stopProcess(infoExec.second, *infoExec.first);
		waitProcStop(infoExec.second->getUnSpawnTime(),
				infoExec.first->getStopTime(),
				*infoExec.second);
		log_->iUser("Process " + tokens[1] + " stopped\n");
	}
	return;

err:
	log_->iUser("Usage: stop [process_name]\n");
}

void TaskMaster::execRestart(const std::vector<std::string> &tokens)
{
	std::pair<ProgramBlock*, ProcInfo*> infoExec;

	if (tokens.size() == 1) {
		log_->eUser("missing process name\n");
		goto err;
	} else if (tokens.size() > 2) {
		log_->eUser("too many arguments\n");
		goto err;
	}
	getProgExecutionInfoByName(tokens[1], infoExec);

	if (infoExec.second == NULL) {
		log_->eUser(tokens[1] + ": no such process\n");
	} else {
		execStop(tokens);
		execStart(tokens);
	}
	return;

err:
	log_->iUser("Usage: restart [process_name]\n");
}

// TODO: delete
void printPbList(const std::list<ProgramBlock> &pbList)
{
	for (std::list<ProgramBlock>::const_iterator it = pbList.begin();
	     it != pbList.end(); ++it) {
		it->print();
	}
}

/**
 * Search for a ProgramBlock matching name and return an iterator to it, or an
 * iterator to end position if no ProgramBlock matched.
*/
std::list<ProgramBlock>::iterator TaskMaster::findPbByName(
		std::list<ProgramBlock> *pbList, const std::string &name)
{
	std::list<ProgramBlock>::iterator it = pbList->begin();

	for (;it != pbList->end(); ++it) {
		if (it->getName() == name)
			break;
	}
	return it;
}

/**
 * Compare the actual ProgramBlock list and a new one resulting from reload, and
 * do several operations in the new one to reflect the new configuration.
*/
void TaskMaster::updatePbList(std::list<ProgramBlock> *newPbList)
{
	std::list<ProgramBlock>::iterator match;
	std::list<ProgramBlock>::iterator remPos = newPbList->begin();
	std::list<ProgramBlock>::iterator changRemPos = newPbList->begin();

	for (std::list<ProgramBlock>::iterator oldPb = pbList_.begin();
	     oldPb != pbList_.end(); ++oldPb) {
		match = findPbByName(newPbList, oldPb->getName());

		if (match == newPbList->end()) {
			oldPb->setState(ProgramBlock::E_STATE_REMOVE);
			newPbList->insert(remPos, *oldPb);
		} else if (*match == *oldPb) {
			match->setState(ProgramBlock::E_STATE_UNCHANGE);
			match->setProcInfos(oldPb->getProcInfos());
		} else {
			oldPb->setState(ProgramBlock::E_STATE_CHANGE_REMOVE);
			newPbList->insert(changRemPos, *oldPb);
			match->setState(ProgramBlock::E_STATE_CHANGE);
			/*
			 * True only for first change_remove insert, allows to
			 * insert PB_STATE_REMOVE ProgramBlocks before
			 * PB_STATE_CHANGE_REMOVE ProgramBlocks.
			 */
			if (remPos == changRemPos)
				--remPos;
		}
	}
}

/**
 * Parse again the configuration file, and:
 * - Stop the removed process groups.
 * - Stop and start with new configuration the updated process groups.
 * - Start the added process groups which contain the token "autostart=true".
*/
void TaskMaster::execReload(const std::vector<std::string> &tokens)
{
	std::list<ProgramBlock>::iterator it;
	std::list<ProgramBlock> newPbList;

	if (tokens.size() > 1) {
		log_->eUser("too many arguments\n");
		log_->iUser("Usage: reload\n");
		return;
	}
	try {
		newPbList = configParser_.reload();
	} catch (const std::runtime_error &e) {
		log_->eUser(e.what());
		return;
	}

	log_->iFile("Reloading configuration file\n");
	updatePbList(&newPbList);
	// printPbList(newPbList); // TODO delete

	pbList_ = newPbList;
	it = pbList_.begin();
	for (; it != pbList_.end(); it = pbList_.begin()) {
		if (it->getState() == ProgramBlock::E_STATE_REMOVE) {
			stopAllPbProcsNoRestart(it->getProcInfos(), *it);
			log_->iAll(it->getName() + ": stopped\n");
			log_->iAll(it->getName() + ": removed process group\n");
			pbList_.remove(*it);
		}
		else if (it->getState() == ProgramBlock::E_STATE_CHANGE_REMOVE) {
			stopAllPbProcsNoRestart(it->getProcInfos(), *it);
			log_->iAll(it->getName() + ": stopped\n");
			log_->iAll(it->getName() + ": updated process group\n");
			pbList_.remove(*it);
		}
		else {
			while (it != pbList_.end()) {
				if (it->getState() == ProgramBlock::E_STATE_NEW)
					log_->iAll(it->getName() + ": added "
						    "process group\n");
				it++;
			}
			break;
		}
	}
	spawner_.autostart(pbList_);
	execStatus(std::vector<std::string>(1, std::string("status")));
}

void TaskMaster::execExit(const std::vector<std::string> &tokens)
{
	std::list<ProgramBlock>::iterator it = pbList_.begin();

	if (tokens.size() > 1) {
		log_->eUser("too many arguments\n");
		log_->iUser("Usage: exit\n");
		return;
	}

	log_->iAll("Stopping all processes...\n");
	for (; it != pbList_.end(); it++) {
		stopAllPbProcsNoRestart(it->getProcInfos(), *it);
		log_->iAll(it->getName() + ": stopped\n");
	}
	log_->iAll("Quitting taskmaster\n");
	log_->flushFile();
	exit(EXIT_SUCCESS);
}

void TaskMaster::getProgExecutionInfoByName(const std::string& name,
		std::pair<ProgramBlock*, ProcInfo*>& info)
{
	std::string pblockCopyName = name.substr(0, name.find("_"));

	info.first = NULL;
	info.second = NULL;

	for (std::list<ProgramBlock>::iterator itList = pbList_.begin();
	     itList != pbList_.end(); itList++) {

		if (itList->getName() == pblockCopyName) {
			ProcInfo *proc = itList->getProcInfoByName(name);
			if (proc != NULL){
				info.first = &(*itList);
				info.second = proc;
				return;
			}
		}
	}
}

/**
 * Wait until the process is successfully started.
*/
int TaskMaster::waitProcStart(long spawnTime, long startTime,
			      const ProcInfo& proc)
{
	long now = time(NULL);
	int pidSaved = proc.getPid();
	std::vector<int> pidsUnspawned;

	while (now - spawnTime < startTime) {
		if (g_sigFlag)
			pidsUnspawned = signalOccured(RELOAD_OFF, RESTART_ON);

		if (std::find(pidsUnspawned.begin(), pidsUnspawned.end(),
		    pidSaved) != pidsUnspawned.end())
			return -1;

		now = time(NULL);
	}
	return 0;
}

/**
 * Wait until the process is successfully stopped.
*/
int TaskMaster::waitProcStop(long unSpawnTime, long endTime,
			     const ProcInfo &proc)
{
	long now = time(NULL);
	int pidSaved = proc.getPid();
	std::vector<int> pidsUnspawned;

	while (now - unSpawnTime < endTime) {
		if (g_sigFlag)
			pidsUnspawned = signalOccured(RELOAD_OFF, RESTART_OFF);

		if (std::find(pidsUnspawned.begin(), pidsUnspawned.end(),
		    pidSaved) != pidsUnspawned.end()) {
			return 0;
		}
		now = time(NULL);
	}

	std::cout << "sending kill signal\n"; // TODO remove
	if (kill(proc.getPid(), SIGKILL) == -1)
		throw std::runtime_error(std::string("kill failed ")
				+ strerror(errno) + "\n");
	while (1) {
		while (!(g_sigFlag & SCHLD));
		pidsUnspawned = signalOccured(RELOAD_OFF, RESTART_ON);

		if (std::find(pidsUnspawned.begin(), pidsUnspawned.end(),
		    pidSaved) != pidsUnspawned.end()) {
			std::cout << "killed the good pid\n"; // TODO remove
			break;
		}
	}
	return -1;
}

/**
 * Handle the different signals, which can lead to 3 actions: unspawn one or
 * several child processes, trigger a configuration reload or exit taskmaster.
 * @isReloadOn: Indicate when a SIGHUP signal is received if the reload
 * 		can be executed in the actual program flow or should be
 * 		executed later.
 * @isRestartOn: Indicate when unspawning children processes after SIGCHLD
 * 		 signal if the autorestart/startretries parameters from config
 * 		 file can trigger or not.
*/
std::vector<pid_t> TaskMaster::signalOccured(bool isReloadOn, bool isRestartOn)
{
	static bool isExiting = false;
	pid_t newPid;
	std::vector<pid_t> vec;

	if (g_sigFlag & SCHLD) {
		g_sigFlag &= ~SCHLD;
		while ((newPid = spawner_.cleanProcess(pbList_, isRestartOn))
		       > 0) {
			vec.push_back(newPid);
		}
	}
	else if (isReloadOn && (g_sigFlag & SHUP)) {
		log_->iUser("\nSIGHUP signal received\n");
		execReload(std::vector<std::string>(1, std::string("reload")));
		g_sigFlag &= ~SHUP;
		log_->iUser("taskmaster> ");
	}
	else if (!isExiting && (g_sigFlag & SEXIT)) {
		isExiting = true;
		execExit(std::vector<std::string>(1, std::string("exit")));
	}
	return vec;
}

/**
 * Stop all the processes in a ProgramBlock with no restart possibility.
*/
void TaskMaster::stopAllPbProcsNoRestart(std::vector<ProcInfo>& vec,
					 const ProgramBlock& pb)
{
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[i].getPid() <= 0)
			continue;

		spawner_.stopProcess(&(vec[i]), pb);
		waitProcStop(vec[i].getUnSpawnTime(), pb.getStopTime(), vec[i]);
	}
}