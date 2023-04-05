/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TaskMaster.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:21:14 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/05 11:04:20 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TaskMaster.hpp"

#include <poll.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <ctime>

extern int g_nbZombiesCleaned;
extern volatile int g_nbProcessZombies;

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

	if (infoExec.second == NULL){
		log_->eAll("Process name not found\n");
		return SHELL_CONTINUE;
	}

	spawner_.startProcess(*infoExec.second, *infoExec.first);
	return SHELL_CONTINUE;

err:
	log_->iUser("Usage: start [program_name]\n");
	return SHELL_CONTINUE;
}

int TaskMaster::execStop(const std::vector<std::string> &tokens)
{
	std::string pblockCopyName = tokens[1].substr(0, tokens[1].find("_"));

	if (tokens.size() == 1) {
		log_->eUser("Missing program name\n");
		goto err;
	} else if (tokens.size() > 2) {
		log_->eUser("Too many arguments\n");
		goto err;
	}
	for (std::list<ProgramBlock>::iterator itList = pbList_.begin();
	     itList != pbList_.end(); itList++) {

		if (itList->getName() == pblockCopyName) {
			std::vector<ProcInfo> proc = itList->getProcInfos();
			for (size_t i =0; i < proc.size(); i++) {

				if (proc[i].getName() == tokens[1]) {
					std::cout << "stopping process\n";
				}
				else if (i == proc.size() - 1)
					log_->eAll("Process name not found\n");
			}
			return SHELL_CONTINUE;
		}
	}

	std::cout << "exec stop" << std::endl;
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

	std::cout << "exec restart" << std::endl;
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

void TaskMaster::updatePbList(std::list<ProgramBlock> *newPbList)
{
	std::pair<ProgramBlock*, ProcInfo*> infoExec;
	(void)newPbList;
	// parcourir toute la liste de l'ancien et checker si present dans la
	// new.
	// SI PAS PRESENT : state a remove et on l'insere au debut de la new
	// SI PRESENT ET UNCHANGED : mettre le state de celui dans la new a unchanged
	// SI PRESENT ET CHANGED : mettre le state de celui dans la new a changed +
	//                         state dans l'ancienne a changed remove et l'inserer
	//                         dans la liste
	// d'abord mettre les states a remove et les inserer au fur et a
	// mesure au debut de la liste.
}


int TaskMaster::execReload(const std::vector<std::string> &tokens)
{
	std::list<ProgramBlock> newPbList;

	if (tokens.size() > 1) {
		log_->eUser("Too many arguments\n");
		goto err;
	}

	std::cout << "exec reload" << std::endl;
	try {
		newPbList = configParser_.reload();
		std::cout << "---------------- New list ----------------\n";
		printPbList(newPbList);
		std::cout << "------------------------------------------\n";
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
	if (tokens.size() > 1) {
		log_->eUser("Too many arguments\n");
		goto err;
	}
	return SHELL_EXIT;

err:
	log_->iUser("Usage: exit\n");
	return SHELL_CONTINUE;
}

void TaskMaster::getProgExecutionInfoByName(const std::string& name,
		std::pair<ProgramBlock*, ProcInfo*>& info, bool bProcInfo)
{
	std::string pblockCopyName = name.substr(0, name.find("_"));

	info.first = NULL;
	info.second = NULL;

	for (std::list<ProgramBlock>::iterator itList = pbList_.begin();
	     itList != pbList_.end(); itList++) {

		if (itList->getName() == pblockCopyName) {
			if (!bProcInfo)
				return;
			ProcInfo *proc = itList->getProcInfoByName(name);
			if (proc != NULL){
				info.first = &(*itList);
				info.second = proc;
				return;
			}
		}
	}
}