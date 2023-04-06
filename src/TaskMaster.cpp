/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TaskMaster.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:21:14 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/06 14:46:53 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TaskMaster.hpp"

#include <poll.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <ctime>
#include <csignal>

extern int g_nbZombiesCleaned;
extern volatile int g_nbProcessZombies;

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

	for (std::list<ProgramBlock>::iterator it = pbList_.begin();
	    it != pbList_.end(); ++it) {
		it->print();
	}
}

/**
 * - Wait for a command from user and execute it.
 * - Execute the appropriate action when a child process dies.
*/
void TaskMaster::shellRoutine()
{
	int pollRet;
	char buf[256] = {};
	std::vector<std::string> tokens;
	struct pollfd pfd = {.fd = 0, .events = POLLIN, .revents = 0};

	spawner_.autostart(pbList_);
	log_->iUser("Launching shell\n");
	log_->iUser("------------------------\n");
	log_->iUser("taskmaster> ");

	while (true)
	{
		pollRet = poll(&pfd, 1, 0);
		if (g_nbProcessZombies > g_nbZombiesCleaned){
			spawner_.unSpawnProcess(pbList_);
			g_nbZombiesCleaned++;
		}
		else if (pollRet & POLLIN) {
			if (read(0, buf, sizeof(buf)) == -1) {
				throw std::runtime_error("Read failed: "
						+ std::string(strerror(errno))
						+  "\n");
			}
			tokens = splitEntry(buf);
			if (execCmd(tokens) == SHELL_EXIT)
				break;

			log_->iUser("taskmaster> ");
			bzero(buf, sizeof(buf));
		}
		else if (pollRet & (POLLERR | POLLNVAL)) {
			throw std::runtime_error(std::string("Poll failed: ")
					+ strerror(errno));
		}
	}
}


/* ----------------------------------------------- */
/* --------------- PRIVATE METHODS --------------- */

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

int TaskMaster::execCmd(const std::vector<std::string> &tokens)
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
	return SHELL_CONTINUE;
}

int TaskMaster::execStatus(const std::vector<std::string> &tokens)
{
	if (tokens.size() > 1) {
		log_->eUser("Too many arguments\n");
		goto err;
	}
	printPbList(pbList_);
	for (std::list<ProgramBlock>::iterator it = pbList_.begin();
	    it != pbList_.end(); ++it) {
		for (size_t i = 0; i < it->getProcInfos().size(); ++i) {
			it->getProcInfos()[i].updateState(it->getStartTime());
			log_->iUser(it->getProcInfos()[i].toString() + "\n");
		}
	}
	return SHELL_CONTINUE;

err:
	log_->iUser("Usage: status\n");
	return SHELL_CONTINUE;
}

int TaskMaster::execStart(const std::vector<std::string> &tokens)
{
	std::pair<ProgramBlock*, ProcInfo*> infoExec;

	if (tokens.size() == 1) {
		log_->eUser("Missing program name\n");
		goto err;
	} else if (tokens.size() > 2) {
		log_->eUser("Too many arguments\n");
		goto err;
	}

	getProgExecutionInfoByName(tokens[1], infoExec);

	if (infoExec.second == NULL)
		log_->eAll("Process name not found\n");
	else if (infoExec.second->getState() == ProcInfo::E_STATE_RUNNING)
		log_->iUser("Process " + tokens[1] + " is alreading running\n");
	else{
		spawner_.startProcess(*infoExec.second, *infoExec.first);

		int started = processStarting(infoExec.second->getSpawnTime(), infoExec.first->getStartTime(), *infoExec.second);

		if (!started)
			log_->iUser("Start " + tokens[1] + " failed\n");
		else
			log_->iAll("Process " + tokens[1] + " started\n");
	}
	return SHELL_CONTINUE;

err:
	log_->iUser("Usage: start [program_name]\n");
	return SHELL_CONTINUE;
}

int TaskMaster::execStop(const std::vector<std::string> &tokens)
{
	std::pair<ProgramBlock*, ProcInfo*> infoExec;

	if (tokens.size() == 1) {
		log_->eUser("Missing program name\n");
		goto err;
	} else if (tokens.size() > 2) {
		log_->eUser("Too many arguments\n");
		goto err;
	}

	getProgExecutionInfoByName(tokens[1], infoExec);

	if (infoExec.second == NULL)
		log_->eUser("Process name not found\n");
	else if (infoExec.second->getState() == ProcInfo::E_STATE_STOPPED)
		log_->iUser("Process " + tokens[1] + " is already stopped\n");
	else{
		spawner_.stopProcess(*infoExec.second, *infoExec.first);

		processStopping(infoExec.second->getUnSpawnTime(), infoExec.first->getStopTime(), *infoExec.second);
		log_->iAll("Process " + tokens[1] + " stopped\n");
	}
	return SHELL_CONTINUE;

err:
	log_->iUser("Usage: stop [program_name]\n");
	return SHELL_CONTINUE;
}

int TaskMaster::execRestart(const std::vector<std::string> &tokens)
{
	if (tokens.size() == 1) {
		log_->eUser("Missing program name\n");
		goto err;
	} else if (tokens.size() > 2) {
		log_->eUser("Too many arguments\n");
		goto err;
	}

	execStop(tokens);
	execStart(tokens);

	return SHELL_CONTINUE;

err:
	log_->iUser("Usage: restart [program_name]\n");
	return SHELL_CONTINUE;

}

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
			oldPb->setState(ProgramBlock::E_PB_STATE_REMOVE);
			newPbList->insert(remPos, *oldPb);
		} else if (*match == *oldPb) {
			match->setState(ProgramBlock::E_PB_STATE_UNCHANGE);
		} else {
			oldPb->setState(ProgramBlock::E_PB_STATE_CHANGE_REMOVE);
			newPbList->insert(changRemPos, *oldPb);
			match->setState(ProgramBlock::E_PB_STATE_CHANGE);
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
int TaskMaster::execReload(const std::vector<std::string> &tokens)
{
	std::list<ProgramBlock> newPbList;

	if (tokens.size() > 1) {
		log_->eUser("Too many arguments\n");
		goto err;
	}
	try {
		newPbList = configParser_.reload();
		updatePbList(&newPbList);

		std::list<ProgramBlock>::iterator it = newPbList.begin();
		printPbList(newPbList);

		for (; it != newPbList.end(); it = newPbList.begin()) {
			if (it->getState() == ProgramBlock::E_PB_STATE_REMOVE) {

				spawner_.stopAllProcess(it->getProcInfos(), *it);
				log_->iUser(it->getName() + ": stopped\n");
				log_->iUser(it->getName() + ": removed process group\n");
				newPbList.remove(*it);
			}
			else if  (it->getState() == ProgramBlock::E_PB_STATE_CHANGE_REMOVE) {
				spawner_.stopAllProcess(it->getProcInfos(), *it);
				log_->iUser(it->getName() + ": stopped\n");
				log_->iUser(it->getName() + ": updated process group\n");
				newPbList.remove(*it);
			}
			else {
				while (it != newPbList.end()) {
					if (it->getState() == ProgramBlock::E_PB_STATE_NEW)
						log_->iUser(it->getName() + ": added process group\n");
					it++;
				}
				break;
			}

		}
		spawner_.autostart(newPbList);

		pbList_ = newPbList;

	} catch (const std::runtime_error &e) {
		log_->eUser(e.what());
	}
	return SHELL_CONTINUE;

err:
	log_->iUser("Usage: reload\n");
	return SHELL_CONTINUE;
}

int TaskMaster::execExit(const std::vector<std::string> &tokens)
{
	std::list<ProgramBlock>::iterator it = pbList_.begin();

	if (tokens.size() > 1) {
		log_->eUser("Too many arguments\n");
		goto err;
	}

	for (; it != pbList_.end(); it++) {
		spawner_.stopAllProcess(it->getProcInfos(), *it);
	}
	return SHELL_EXIT;

err:
	log_->iUser("Usage: exit\n");
	return SHELL_CONTINUE;
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

int TaskMaster::processStarting(long startTime, long elapseTime, ProcInfo& proc)
{
	long now = time(NULL);
	while (now - startTime < elapseTime) {
		if (g_nbProcessZombies > g_nbZombiesCleaned){
			spawner_.unSpawnProcess(pbList_);
			g_nbZombiesCleaned++;
		}
		if (proc.getPid() < 0){
			return 0;
		}
		now = time(NULL);
	}
	return 1;
}

void TaskMaster::processStopping(long stopTime, long elapseTime, const ProcInfo &proc)
{
	long now = time(NULL);

	while (proc.getPid() > 0) {
		if (g_nbProcessZombies > g_nbZombiesCleaned){
			spawner_.unSpawnProcess(pbList_);
			g_nbZombiesCleaned++;
		}
		now = time(NULL);

		if (now - stopTime > elapseTime) {
			kill(proc.getPid(), SIGKILL);
		}
	}
}

