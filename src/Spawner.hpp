#ifndef SPAWNER_HPP
# define SPAWNER_HPP

#include "ProgramBlock.hpp"
#include "ProcInfo.hpp"
#include "Logger.hpp"
#include <list>

class Spawner
{
		/* ----------------------------------------------- */
		/* ---------------- COPLIEN FORM ----------------- */
	public:

		Spawner();
		~Spawner();
		Spawner(const Spawner&) = delete;


		/* ----------------------------------------------- */
		/* ------------- SETTERS / GETTERS --------------- */

		Logger *getLogger() const;
		void setLogger(Logger *log);


		/* ----------------------------------------------- */
		/* ------------------- METHODS ------------------- */

		void autostart(std::list<ProgramBlock>&);
		void startProcess(ProcInfo&, const ProgramBlock&);
		void stopProcess(ProcInfo *proc, const ProgramBlock &pb);
		int cleanProcess(std::list<ProgramBlock>& pbList,
				   bool isRestartOn);


		/* ----------------------------------------------- */
		/* --------------- PRIVATE METHODS --------------- */
	private:

		void fileProcHandler(const ProcInfo& pInfo,
				     const ProgramBlock& prg);
		int restartExitedProcess(ProcInfo *proc, ProgramBlock *pb,
					 bool isRestartOn, int exitPid);
		std::vector<std::string> splitCmdArgs(const std::string& cmd);
		char **strVecToCArray(const std::vector<std::string> &);
		void freeExecveArg(char **av, char **env);


		/* ----------------------------------------------- */
		/* ----------------- ATTRIBUTES ------------------ */
	private:

		Logger* logger_;
};


#endif