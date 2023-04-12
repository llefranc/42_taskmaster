#include "Spawner.hpp"

#include <fcntl.h>
#include <cstring>
#include <ctime>
#include <sys/stat.h>
#include <sys/wait.h>
#include <cstdlib>

extern volatile int g_sigFlag;

#define SCHLD (1U << 0)


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
 * Start all processes from all new ProgramBlocks with token autostart set to
 * true.
*/
void Spawner::autostart(std::list<ProgramBlock>& pbList)
{
	std::list<ProgramBlock>::iterator it = pbList.begin();

	for (; it != pbList.end(); it++) {

		if (it->getAutoStart() == true &&
		    (it->getState() == ProgramBlock::E_STATE_NEW ||
		    it->getState() == ProgramBlock::E_STATE_CHANGE)) {
			std::vector<ProcInfo>& proc = it->getProcInfos();
			for (size_t i = 0; i < proc.size(); i++) {
				startProcess(proc[i], *it);
			}
		}
	}
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

		/* Strings and quoted portions next to each other are joined */
		while (buf[i] && !isspace(buf[i])) {

			/* -1 corresponds to quotes character not escaped */
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
 * Setup stdin/stdout outputs for child process.
*/
void Spawner::fileProcHandler(const ProcInfo& pInfo, int& fd, int& fderr,
		const ProgramBlock& prg)
{
	int pipefd[2];
	std::string outFile = prg.getLogOut();
	std::string errFile = prg.getLogErr();

	umask(prg.getUmask());
	if (outFile.empty())
		outFile = "/tmp/" + pInfo.getName() + "_" + pInfo.getHash() + "_stdout.txt";
	if (errFile.empty())
		errFile = "/tmp/" + pInfo.getName() + "_" + pInfo.getHash() + "_stderr.txt";

	if ((fd = open(outFile.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0777)) < 0) {
		std::cerr << "Son open stdout failed\n";
		exit(EXIT_SPAWN_FAILED);
	}
	if ((fderr = open(errFile.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0777)) < 0) {
		std::cerr << "Son open stderr failed\n";
		exit(EXIT_SPAWN_FAILED);
	}

	// redirection out and err into log files
	if (dup2(fd, STDOUT_FILENO) < 0) {
		std::cerr << "Son dup2 stdout failed\n";
		exit(EXIT_SPAWN_FAILED);
	}
	if (dup2(fderr, STDERR_FILENO) < 0) {
		std::cerr << "Son dup2 stderr failed\n";
		exit(EXIT_SPAWN_FAILED);
	}

	if (pipe(pipefd) == -1) {
		std::cerr << "Son pipe failed\n";
		exit(EXIT_SPAWN_FAILED);
	}
	// To avoid unexpected exit for program using stdin like /bin/cat
	if (dup2(pipefd[0], STDIN_FILENO) < 0) {
		std::cerr << "Son dup2 pipe failed\n";
		exit(EXIT_SPAWN_FAILED);
	}
}

/**
 * Transform a vector of strings into a double char array.
*/
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

/**
 * Free the args allocated for execve.
*/
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

/**
 * Start and update state of process.
*/
void Spawner::startProcess(ProcInfo& pInfo, const ProgramBlock& prg)
{
	int fd, fderr;
	std::vector<std::string> vecArgs;
	pid_t pid;

	pid = fork();
	if (pid == 0) {
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

		// open process log files for stdout and stderr + pipe stdin
		fileProcHandler(pInfo, fd, fderr, prg);

		// change working directory if specified
		if (prg.getWorkDir().empty() == false) {
			if (chdir(prg.getWorkDir().c_str()) == -1) {
				std::cerr << "Son workdir failed\n";
				exit(EXIT_SPAWN_FAILED);
			}
		}
		if (execve(av[0], av, env) < 0) {
			freeExecveArg(av, env);
			std::cerr << "Son execve failed\n";
			exit(EXIT_SPAWN_FAILED);
		}
	} else if (pid > 0) {
		pInfo.setSpawnTime(time(NULL));
		pInfo.setUnSpawnTime(0);
		pInfo.setState(ProcInfo::E_STATE_STARTING);
		pInfo.setPid(pid);
	} else {
		throw std::runtime_error(std::string("Fork failed:") +
				strerror(errno) + "\n");
	}
}

/**
 * Restart an exited process if needed (depending on configuration file
 * parameters).
*/
int Spawner::restartExitedProcess(ProcInfo *proc, ProgramBlock *pb,
				  bool isRestartOn)
{
	int exitCode;
	std::time_t now = time(NULL);

	if ((now - proc->getSpawnTime()) < pb->getStartTime()) {
		if (isRestartOn && proc->getNbRestart() <
			pb->getStartRetries()) {
			proc->setState(ProcInfo::E_STATE_BACKOFF);
			proc->setNbRestart(proc->getNbRestart() + 1);
			startProcess(*proc, *pb);
			return 1;
		}
		proc->setState(ProcInfo::E_STATE_FATAL);
	}
	else if (proc->getState() != ProcInfo::E_STATE_STOPPED) {
		proc->setState(ProcInfo::E_STATE_EXITED);

		exitCode = proc->getExitCode();
		if (isRestartOn && pb->shouldAutoRestart(exitCode)) {
			proc->setNbRestart(0);
			startProcess(*proc, *pb);
			return 1;
		}
	}
	return 0;
}

/**
 * Wait on any child process that has exited and find the right ProcInfo.
 * Clean and update status of process. Restart processes if needed (depending on
 * configuration file parameters).
 * @isRestartOn: Indicate if a restart is possible after the unspawn of the
 * 		 process.
 *
 * Return: Child process pid if there was one to unspawn, 0 if there was no
 * 	   unwaited child, -1 if the child process PID was not found in
 * 	   ProgramBlock list.
*/
int Spawner::cleanProcess(std::list<ProgramBlock>& pbList, bool isRestartOn)
{
	int status = -1;
	int pidChild;
	ProcInfo *proc = NULL;
	ProgramBlock *pb = NULL;

	if ((pidChild = waitpid(-1, &status, WNOHANG)) <= 0)
		return 0;
	for (std::list<ProgramBlock>::iterator it = pbList.begin();
	     it != pbList.end(); it++) {
		proc = it->getProcInfoByPid(pidChild);
		if (proc) {
			pb = &(*it);
			break;
		}
	}
	if (proc == NULL || pb == NULL) {
		logger_->eUser("process PID not found\n");
		return -1;
	}
	proc->setExitCode(WEXITSTATUS(status));

	if (proc->getState() != ProcInfo::E_STATE_STOPPED) {
		if (proc->getExitCode() == EXIT_SPAWN_FAILED) {
			proc->setState(ProcInfo::E_STATE_FATAL);
		}
		else if (restartExitedProcess(proc, pb, isRestartOn)) {
			return pidChild;
		}
	}
	proc->setNbRestart(0);
	proc->setPid(-1);
	proc->setUnSpawnTime(time(NULL));
	return pidChild;
}


/**
 * Stop a child process with the signal defined in configuration file.
*/
void Spawner::stopProcess(ProcInfo *proc, const ProgramBlock& pb)
{
	if (proc->getPid() > 0) {
		if (kill(proc->getPid(), pb.getStopSignal()) < 0)
			throw std::runtime_error(std::string("kill failed ")
					+ strerror(errno) + "\n");
	}
	proc->setState(ProcInfo::E_STATE_STOPPED);
	proc->setUnSpawnTime(time(NULL));
}