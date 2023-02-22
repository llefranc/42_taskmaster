#include "Spawner.hpp"
#include <unistd.h>
#include <stdexcept>
#include <fcntl.h>
#include <string.h>
#include "time.h"

#include <iostream>



Spawner::Spawner()
	: logger_(NULL)
{}

Spawner::~Spawner()
{
}

Logger *Spawner::getLogger() const
{
	return logger_;
}

void Spawner::setLogger(Logger *logger)
{
	logger_ = logger;
}

void Spawner::startProgramBlock(ProgramBlock& prg)
{
	try
	{
		std::vector<ProcInfo>::iterator it;
		std::vector<ProcInfo> pInfo = prg.getProcInfos();
		for (it = pInfo.begin(); it != pInfo.end(); it++)
		{
			int pState = it->getState();
			if (pState != ProcInfo::PC_STATE_STARTING || pState != ProcInfo::PC_STATE_RUNNING)
				startProcess(*it, prg);
		}

	}
	catch(std::runtime_error & e)
	{
		throw e;
	}

}

void Spawner::startProcess(ProcInfo& pInfo, const ProgramBlock& prg)
{
	try
	{
		pid_t pid;

		pid = fork();
		int oldOut = dup(STDOUT_FILENO);
		int oldErr = dup(STDERR_FILENO);
		if (pid == 0)
		{
			// open log files for stdout and stderr
			int fd, fderr;
			std::string outFile = pInfo.getName() + "_" + pInfo.getHash() + "_stdout.txt";
			std::string errFile = pInfo.getName() + "_" + pInfo.getHash() + "_stderr.txt";
			std::string outPath(prg.getLogOut() + "/" + outFile);
			std::string errPath(prg.getLogErr() + "/" + errFile);
			if ((fd = open(outPath.c_str(), O_CREAT | O_APPEND | O_WRONLY)) < 0)
				throw std::runtime_error("error open " + outFile);
			if ((fderr = open(errPath.c_str(), O_CREAT | O_APPEND | O_WRONLY)) < 0)
				throw std::runtime_error("error open " + errFile);

			// redirection out and err into log files
			if (dup2(fd, STDOUT_FILENO) < 0)
				throw std::runtime_error("premier dup");
			if (dup2(fderr, STDERR_FILENO) < 0)
				throw std::runtime_error("deuxieme dup");

			// change working directory if specified
			if (prg.getWorkDir().empty() == false)
				chdir(prg.getWorkDir().c_str());

			// set up for execve - argument and environment variables
			char **env = setExecveEnv(prg.getEnv());
			char **arg = (char**)malloc(sizeof(char*));
			bzero(arg, sizeof(char*));
			if (execve("script.sh", arg, env) < 0)
			{
				// free arg and env + redirect stdout and stderr back
				freeExecveArg(arg, env, prg.getEnv().size());
				dup2(oldOut, STDOUT_FILENO);
				dup2(oldErr, STDERR_FILENO);
				close(fd);
				close(fderr);
				throw std::runtime_error(std::string("Execve failed: ") + strerror(errno));
			}
		}
		else if (pid > 0)
		{
			pInfo.setStartTime(time(NULL));
			pInfo.setState(ProcInfo::PC_STATE_STARTING);
			pInfo.setPid(pid);
			int nbRes = pInfo.getNbRestart();
			pInfo.setNbRestart(nbRes++);
			logger_->iAll("Process " + pInfo.getName() + " started\n");
		}
		else
		{
			throw std::runtime_error(std::string("Fork failed:") + strerror(errno));
		}
	}
	catch (std::runtime_error & e)
	{
		throw e;
	}
}

char** Spawner::setExecveEnv(const std::vector<std::string> &vecEnv)
{
	char **env;
	int nbEnv = vecEnv.size();
	env = (char**)malloc(sizeof(char*) * (nbEnv + 1));
	if (env == NULL)
		throw std::runtime_error("Malloc failed in setExecveEnv");

	bzero(env, sizeof(*env));
	for (int i = 0; i < nbEnv; i++)
	{
		env[1] = strdup(vecEnv[i].c_str());
	}

	return env;
}

void Spawner::freeExecveArg(char** arg, char** env, size_t size)
{
	free(arg);

	for (size_t i = 0; i < size; i++)
		free(env[i]);
	free(env);
}