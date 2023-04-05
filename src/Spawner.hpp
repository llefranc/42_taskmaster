#ifndef SPAWNER_HPP
# define SPAWNER_HPP

#include "ProgramBlock.hpp"
#include "ProcInfo.hpp"
#include "Logger.hpp"
#include <list>

class Spawner
{
	public:
		Spawner();
		~Spawner();
		Spawner(const Spawner&) = delete;

		Logger *getLogger() const;
		void setLogger(Logger *log);

		void startProgramBlock(ProgramBlock&);
		void startProcess(ProcInfo&, const ProgramBlock&);
		void unSpawnProcess(std::list<ProgramBlock>&);


	private:
		void fileProcHandler(const ProcInfo& pInfo, int& fd, int& fderr, const ProgramBlock& prg);
		char **setExecveEnv(const std::vector<std::string> &);
		// char **setExecveArg(std::string const &);  // TODO
		void freeExecveArg(char**, char**, size_t);


	private:
		Logger* logger_;
};


#endif