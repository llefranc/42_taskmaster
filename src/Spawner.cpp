#include "Spawner.hpp"
#include <unistd.h>
#include <stdexcept>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <iostream>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>

extern int g_nbZombiesCleaned;
extern volatile int g_nbProcessZombies;  // TODO inutile a supprimer

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
		std::vector<ProcInfo> &pInfo = prg.getProcInfos();
		for (it = pInfo.begin(); it != pInfo.end(); it++)
		{
			int pState = it->getState();
			if (pState != ProcInfo::E_STATE_STARTING || pState != ProcInfo::E_STATE_RUNNING)
				startProcess(*it, prg);
		}

	}
	catch(std::runtime_error & e)
	{
		throw e;
	}

}

void Spawner::unSpawnProcess(std::list<ProgramBlock>& pbList)
{
	int status= -1;
	int pidChild = wait(&status);

	ProcInfo *proc = NULL;
	ProgramBlock *pb = NULL;
	for (std::list<ProgramBlock>::iterator it = pbList.begin();
	      it != pbList.end(); it++) {
		proc = it->getProcInfoByPid(pidChild);
		if (proc){
			pb = &(*it);
			break;
		}
	}

	if (proc == NULL || pb == NULL) {
		logger_->eUser("Process PID not found\n");
		return ;
	}

	if (proc->getState() != ProcInfo::E_STATE_STOPPED) {
		long now = time(NULL);
		if (now - proc->getStartTime() < pb->getStartTime())
			proc->setState(ProcInfo::E_STATE_BACKOFF);
		else if (pb->getAutoRestart() == ProgramBlock::E_AUTO_FALSE || 
		       (pb->getAutoRestart() == ProgramBlock::E_AUTO_UNEXP &&
		       pb->getExitCodes().find(status) != pb->getExitCodes().end())) { 
			proc->setState(ProcInfo::E_STATE_EXITED);
		}
		if (pb->getAutoRestart() == ProgramBlock::E_AUTO_TRUE) {
			if (proc->getNbRestart() < pb->getStartRetries())
				return this->startProcess(*proc, *pb);
			proc->setState(ProcInfo::E_STATE_FATAL);
		}
	}
	else {
		proc->setNbRestart(0);
	}
	proc->setPid(-1);
	proc->setEndTime(time(NULL));

}

void Spawner::startProcess(ProcInfo& pInfo, const ProgramBlock& prg)
{
	try
	{
		pid_t pid;

		pid = fork();
		if (pid == 0)
		{
			
			int oldOut = dup(STDOUT_FILENO);
			int oldErr = dup(STDERR_FILENO);
			int fd, fderr;
			// open process log files for stdout and stderr + pipe stdin
			fileProcHandler(pInfo, fd, fderr, prg);

			// change working directory if specified
			if (prg.getWorkDir().empty() == false)
				chdir(prg.getWorkDir().c_str());

			// set up for execve - argument and environment variables
			char **env = setExecveEnv(prg.getEnv());
			char **arg = (char**)malloc(sizeof(char*) * 2);
			arg[0] = (char*)(prg.getCmd().c_str());
			arg[1] = 0;

			if (execve(prg.getCmd().c_str(), arg, env) < 0)
			{
				// free arg and env + redirect stdout and stderr back
				freeExecveArg(arg, env, prg.getEnv().size());
				dup2(oldOut, STDOUT_FILENO);
				dup2(oldErr, STDERR_FILENO);
				close(fd);
				close(fderr);
				// TODO: il y a toujours 2 process. Je pense il faut exit avec un
				// code  d'erreur !!!!!
				throw std::runtime_error(std::string("Execve failed: ") + strerror(errno));
			}
		}
		else if (pid > 0)
		{
			pInfo.setStartTime(time(NULL));
			pInfo.setEndTime(0);
			pInfo.setState(ProcInfo::E_STATE_STARTING);
			pInfo.setPid(pid);
			int nbRes = pInfo.getNbRestart();
			pInfo.setNbRestart(++nbRes);
		}
		else
		{
			throw std::runtime_error(std::string("Fork failed:") + strerror(errno) + "\n");
		}
	}
	catch (std::runtime_error & e)
	{
		throw e;
	}
}

void Spawner::stopProcess(ProcInfo& proc, const ProgramBlock& pb)
{
	kill(proc.getPid(), pb.getStopSignal());
	proc.setState(ProcInfo::E_STATE_STOPPED);
	proc.setEndTime(time(NULL));
}


void Spawner::fileProcHandler(const ProcInfo& pInfo, int& fd, int& fderr, const ProgramBlock& prg)
{
	mode_t mode = umask(prg.getUmask());   // sauvegarder l'ancienne valeur mode_t
	std::string outFile = pInfo.getName() + "_" + pInfo.getHash() + "_stdout.txt";
	std::string errFile = pInfo.getName() + "_" + pInfo.getHash() + "_stderr.txt";
	std::string outPath(prg.getLogOut() + "/" + outFile);
	std::string errPath(prg.getLogErr() + "/" + errFile);

	if ((fd = open(outPath.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0777)) < 0)
		throw std::runtime_error("Open " + outPath + " : " +  strerror(errno) + "\n");
	if ((fderr = open(errPath.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0777)) < 0)
		throw std::runtime_error("error open " + errPath + "\n");

	// redirection out and err into log files
	if (dup2(fd, STDOUT_FILENO) < 0)
		throw std::runtime_error(std::string("dup2 stdout failed : ") + strerror(errno) + "\n");
	if (dup2(fderr, STDERR_FILENO) < 0)
		throw std::runtime_error(std::string("dup2 stderr failed : ") + strerror(errno) + "\n");
	
	int pipefd[2];
	if (pipe(pipefd) == -1)
		throw std::runtime_error("pipe\n");
	// for command /bin/cat
	if (dup2(pipefd[0], STDIN_FILENO) < 0)
		throw std::runtime_error(std::string("dup2 stdin failed : ") + strerror(errno) + "\n");
	
	umask(mode);	// umask at its initial state
}

char** Spawner::setExecveEnv(const std::vector<std::string> &vecEnv)
{
	static char **env;
	int nbEnv = vecEnv.size();
	env = (char**)malloc(sizeof(char*) * (nbEnv + 1));
	if (env == NULL)
		throw std::runtime_error("Malloc failed in setExecveEnv\n");

	bzero(env, sizeof(*env));
	for (int i = 0; i < nbEnv; i++)
	{
		env[i] = strdup(vecEnv[i].c_str());
	}
	env[nbEnv] = 0;
	return env;
}

// char** Spawner::setExecveArg(std::string const &cmd)
// {
// 	int word = 0;
// 	bool lastIsSpace=false;
// 	if (isspace(cmd[0]))
// 		lastIsSpace=true;
// 	else
// 		word++;
// 	for (size_t i=1; i < cmd.size(); i++)
// 	{
// 		if (isspace(cmd[i]))
// 			lastIsSpace=true;
// 		else if (lastIsSpace && !isspace(cmd[i]))
// 		{
// 			word++;
// 			lastIsSpace=false;
// 		}
// 	}
// }

void Spawner::freeExecveArg(char** arg, char** env, size_t size)
{
	free(arg);

	for (size_t i = 0; i < size; i++)
		free(env[i]);
	free(env);
}
