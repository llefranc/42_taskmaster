#ifndef SPAWNER_HPP
# define SPAWNER_HPP

#include "ProgramBlock.hpp"
#include "ProcInfo.hpp"
#include "Logger.hpp"
#include <list>

class Spawner
{
	public:
		/* ----------------------------------------------- */
		/* ---------------- COPLIEN FORM ----------------- */

		Spawner();
		~Spawner();
		Spawner(const Spawner&) = delete;

		/* ----------------------------------------------- */
		/* ------------- SETTERS / GETTERS --------------- */

		Logger *getLogger() const;
		void setLogger(Logger *log);

		/* ----------------------------------------------- */
		/* ------------------- METHODS ------------------- */

		void startProgramBlock(ProgramBlock&);
		void autostart(std::list<ProgramBlock>&);
		void startProcess(ProcInfo&, const ProgramBlock&);
		void stopProcess(ProcInfo *proc, const ProgramBlock &pb);
		int unSpawnProcess(std::list<ProgramBlock>& pbList,
				   bool isRestartOn);

	private:

		void fileProcHandler(const ProcInfo& pInfo, int& fd, int& fderr,
				const ProgramBlock& prg);
		char **strVecToCArray(const std::vector<std::string> &);
		std::vector<std::string> splitCmdArgs(const std::string& cmd);
		void freeExecveArg(char **av, char **env);


	private:

		Logger* logger_;
};


#endif