#include "Spawner.hpp"
#include <fcntl.h>
#include <string.h>
#include <ctime>
#include <sys/stat.h>
#include <sys/wait.h>


extern int g_nbZombiesCleaned;
extern volatile int g_nbProcessZombies;

/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

Spawner::Spawner()
	: logger_(NULL)
{}

Spawner::~Spawner()
{
}

/* ----------------------------------------------- */
/* ------------- GETTERS / SETTERS --------------- */

Logger *Spawner::getLogger() const
{
	return logger_;
}

void Spawner::setLogger(Logger *logger)
{
	logger_ = logger;
}

/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */

/**
 * Start all processes from all new programBlocks
 * it has to be configure as : autostart = true
*/

void Spawner::autostart(std::list<ProgramBlock>& pbList)
{
	try
	{
		std::list<ProgramBlock>::iterator it = pbList.begin();

		for (; it != pbList.end(); it++) {

			if (it->getAutoStart() == true
			  && (it->getState() == ProgramBlock::E_PB_STATE_NEW ||
			  it->getState() == ProgramBlock::E_PB_STATE_CHANGE)) {

				std::vector<ProcInfo>& proc = it->getProcInfos();
				for (size_t i = 0; i < proc.size(); i++) {
					startProcess(proc[i], *it);
				}
			}
		}
	}
	catch(std::runtime_error & e)
	{
		throw e;
	}
}


/**
 * wait pid and find the right ProcInfo
 * clean and update status of process
*/
void Spawner::unSpawnProcess(std::list<ProgramBlock>& pbList)
{
	int status = -1;
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
	proc->setExitCode(status);

	if (proc->getState() != ProcInfo::E_STATE_STOPPED) {
		long now = time(NULL);
		if (now - proc->getSpawnTime() < pb->getStartTime()) {
			if (pb->getAutoRestart() == ProgramBlock::E_AUTO_FALSE)
				proc->setState(ProcInfo::E_STATE_FATAL);
			else
				proc->setState(ProcInfo::E_STATE_BACKOFF);
			
		}
		else if (pb->getAutoRestart() == ProgramBlock::E_AUTO_FALSE ||
		       (pb->getAutoRestart() == ProgramBlock::E_AUTO_UNEXP &&
		       pb->getExitCodes().find(status) != pb->getExitCodes().end())) {
			proc->setState(ProcInfo::E_STATE_EXITED);
		}
		if (pb->getAutoRestart() == ProgramBlock::E_AUTO_TRUE ) {
			if (proc->getNbRestart() < pb->getStartRetries())
				return this->startProcess(*proc, *pb);
			proc->setState(ProcInfo::E_STATE_FATAL);
		}
	}
	else {
		proc->setNbRestart(0);
	}
	proc->setPid(-1);
	proc->setUnSpawnTime(time(NULL));
}

/**
 * Fill a buffer with a copy of the command, removing escape characters and
 * replacing enclosing quotes with -1 to identify them.
*/
static inline int trimQuotes(const std::string &cmd, std::string *buf)
{
	bool escape = false;
	bool inSimpleQuotes = false;
	bool inDoubleQuotes = false;
	size_t j = 0;

	for (size_t i = 0; i < cmd.length(); ++i) {
		if (cmd[i] == '\\' && !escape) {
			escape = true;
			continue;
		} else if (cmd[i] == '"' && !escape && !inSimpleQuotes) {
			inDoubleQuotes = !inDoubleQuotes;
			(*buf)[j++] = -1;
		} else if (cmd[i] == '\'' && !escape && !inDoubleQuotes) {
			inSimpleQuotes = !inSimpleQuotes;
			(*buf)[j++] = -1;
		} else {
			(*buf)[j++] = cmd[i];
		}
		escape = false;
	}
	if (inSimpleQuotes || inDoubleQuotes)
		return -1;
	return 0;
}

/**
 * Split the command from configuration file into tokens, handling quotes like
 * in a shell. Return an empty vector if quotes are not corretly closed.
 * Ex: - 3 tokens: wc /tmp/file.txt /tmp/"second_file.txt"
 *     - 2 tokens: print "Hello world"
*/
std::vector<std::string> Spawner::splitCmdArgs(const std::string& cmd)
{
	size_t i = 0;
	size_t start = 0;
	std::string tmp;
	std::string buf(cmd.length(), '\0');
	std::vector<std::string> vec;

	if (trimQuotes(cmd, &buf) == -1)
		return vec;

	while (buf[i]) {
		tmp = "";
		while (buf[i] && isspace(buf[i]))
			++i;
		if (!buf[i])
			break;

		/* Strings and quotes portions next to each other are joined */
		while (buf[i] && !isspace(buf[i])) {

			/* -1 == quotes character not escaped */
			if (buf[i] == -1) {
				start = ++i;
				while (buf[i] && buf[i] != -1)
					++i;
				tmp += buf.substr(start, i - start);
				++i;
			} else {
				start = i;
				while (buf[i] && buf[i] != -1 && buf[i] != ' ')
					++i;
				tmp += buf.substr(start, i - start);
			}
		}
		vec.push_back(tmp);
	}
	return vec;
}

/**
 * start and update state of process
*/
void Spawner::startProcess(ProcInfo& pInfo, const ProgramBlock& prg)
{
	std::vector<std::string> vecArgs;
	pid_t pid;

	pid = fork();
	if (pid == 0) {
		int fd, fderr;
		// open process log files for stdout and stderr + pipe stdin
		fileProcHandler(pInfo, fd, fderr, prg);

		// change working directory if specified
		if (prg.getWorkDir().empty() == false) {
			if (chdir(prg.getWorkDir().c_str()) == -1) {
				std::cerr << "Son workdir failed\n";
				exit(EXIT_SPAWN_FAILED);
			}
		}

		// set up for execve - argument and environment variables
		char **env = strVecToCArray(prg.getEnv());
		vecArgs = splitCmdArgs(prg.getCmd());
		if (vecArgs.empty()) {
			std::cerr << "Son quotes not closed\n";
			exit(EXIT_SPAWN_FAILED);
		}
		char **av = strVecToCArray(vecArgs);
		if (av == NULL || env == NULL) {
			std::cerr << "Son malloc failed\n";
			exit(EXIT_SPAWN_FAILED);
		}

		if (execve(av[0], av, env) < 0) {
			freeExecveArg(av, env);
			close(fd);
			close(fderr);
			std::cerr << "Son execve failed\n";
			exit(EXIT_SPAWN_FAILED);
		}
	} else if (pid > 0) {
		pInfo.setSpawnTime(time(NULL));
		pInfo.setUnSpawnTime(0);
		pInfo.setState(ProcInfo::E_STATE_STARTING);
		pInfo.setPid(pid);
		int nbRes = pInfo.getNbRestart();
		pInfo.setNbRestart(++nbRes);
	} else {
		throw std::runtime_error(std::string("Fork failed:") +
				strerror(errno) + "\n");
	}
}


void Spawner::stopProcess(ProcInfo& proc, const ProgramBlock& pb)
{
	if (kill(proc.getPid(), pb.getStopSignal()) < 0)
		throw std::runtime_error(std::string("kill failed") + strerror(errno));
	proc.setState(ProcInfo::E_STATE_STOPPED);
	proc.setUnSpawnTime(time(NULL));
}

/**
 * kill a list of processes of ProgramBlock
*/
void Spawner::stopAllProcess(std::vector<ProcInfo>& vec, const ProgramBlock& pb)
{
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[i].getPid() > 0) {
			stopProcess(vec[i], pb);
			while (g_nbProcessZombies == g_nbZombiesCleaned) ;
			int status;
			wait(&status);
			g_nbZombiesCleaned++;
		}
	}
}

void Spawner::fileProcHandler(const ProcInfo& pInfo, int& fd, int& fderr, const ProgramBlock& prg)
{
	mode_t mode = umask(prg.getUmask());
	std::string outFile = pInfo.getName() + "_" + pInfo.getHash() + "_stdout.txt";
	std::string errFile = pInfo.getName() + "_" + pInfo.getHash() + "_stderr.txt";
	std::string outPath(prg.getLogOut() + "/" + outFile);
	std::string errPath(prg.getLogErr() + "/" + errFile);

	if ((fd = open(outPath.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0777)) < 0) {
		std::cerr << "Son open stdout failed\n";
		exit(EXIT_FAILURE);
		// throw std::runtime_error("Open " + outPath + " : " +  strerror(errno) + "\n");
	}
	if ((fderr = open(errPath.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0777)) < 0) {
		std::cerr << "Son open stderr failed\n";
		exit(EXIT_FAILURE);
		// throw std::runtime_error("error open " + errPath + "\n");
	}

	// redirection out and err into log files
	if (dup2(fd, STDOUT_FILENO) < 0) {
		std::cerr << "Son dup2 stdout failed\n";
		exit(EXIT_FAILURE);
		// throw std::runtime_error(std::string("dup2 stdout failed : ") + strerror(errno) + "\n");
	}
	if (dup2(fderr, STDERR_FILENO) < 0) {
		std::cerr << "Son dup2 stderr failed\n";
		exit(EXIT_FAILURE);
		// throw std::runtime_error(std::string("dup2 stderr failed : ") + strerror(errno) + "\n");
	}

	int pipefd[2];
	if (pipe(pipefd) == -1) {
		std::cerr << "Son pipe failed\n";
		exit(EXIT_FAILURE);
		// throw std::runtime_error("pipe\n");
	}
	// To avoid unexpected exit for program using stdin like /bin/cat
	if (dup2(pipefd[0], STDIN_FILENO) < 0) {
		std::cerr << "Son dup2 pipe failed\n";
		exit(EXIT_FAILURE);
	}

	umask(mode); // umask to its initial state
}

char** Spawner::strVecToCArray(const std::vector<std::string> &vec)
{
	static char **array;
	int size = vec.size();

	if ((array = (char**)malloc(sizeof(char*) * (size + 1))) == NULL)
		return NULL;

	bzero(array, sizeof(*array));
	for (int i = 0; i < size; i++)
	{
		if ((array[i] = strdup(vec[i].c_str())) == NULL)
			return NULL;
	}
	array[size] = NULL;
	return array;
}

void Spawner::freeExecveArg(char** av, char** env)
{
	int i = 0;

	while (av[i])
		free(av[i++]);
	free(av[i]); /* for NULL ptr */
	free(av);

	i = 0;
	while (env[i])
		free(env[i++]);
	free(env[i]); /* for NULL ptr */
	free(env);
}
