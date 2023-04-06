#ifndef PROC_INFO_HPP
# define PROC_INFO_HPP

#include <string>
#include <ctime>

#define EXIT_SPAWN_FAILED 126

class ProcInfo
{
	public:

		static enum {
			E_STATE_STOPPED,
			E_STATE_STARTING,
			E_STATE_RUNNING,
			E_STATE_BACKOFF,
			E_STATE_STOPPING,
			E_STATE_EXITED,
			E_STATE_FATAL,
		} E_procState;


		/* ----------------------------------------------- */
		/* ---------------- COPLIEN FORM ----------------- */

		ProcInfo();
		ProcInfo(const std::string &name);
		~ProcInfo();
		ProcInfo(const ProcInfo &c);
		ProcInfo& operator=(ProcInfo a);


		/* ----------------------------------------------- */
		/* ------------------- GETTERS ------------------- */

		int getState() const;
		const std::string& getName() const;
		const std::string& getHash() const;
		int getPid() const;
		int getNbRestart() const;
		long getSpawnTime() const;
		long getUnSpawnTime() const;
		uint8_t getExitCode() const;


		/* ----------------------------------------------- */
		/* ------------------- SETTERS ------------------- */

		void setState(int state);
		void setName(const std::string &name);
		void setHash(const std::string &hash);
		void setPid(int pid);
		void setNbRestart(int nbRestart);
		void setSpawnTime(long spawnTime);
		void setUnSpawnTime(long unSpawnTime);
		void setExitCode(uint8_t exitCode);


		/* ----------------------------------------------- */
		/* ------------------- METHODS ------------------- */

		void updateState(std::time_t starttime);
		std::string toString() const;


		/* ----------------------------------------------- */
		/* ---------------- HELPER FUNCS------------------ */

		friend void swap(ProcInfo &a, ProcInfo &b);


		/* ----------------------------------------------- */
		/*------------------ ATTRIBUTES ------------------ */

	private:

		int state_;
		std::string name_;
		std::string hash_;
		pid_t pid_;
		int nbRestart_;
		time_t spawnTime_; /* renommer ces variables en spawn unspawn ? */
		time_t unSpawnTime_;
		uint8_t exitCode_;
};

#endif // PROC_INFO_HPP