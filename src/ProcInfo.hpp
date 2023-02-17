#ifndef PROC_INFO_HPP
# define PROC_INFO_HPP

#include <string>

class ProcInfo
{
	public:

		static enum {
			PC_STATE_STOP,
			PC_STATE_CRASHED,
			PC_STATE_STARTING,
			PC_STATE_RUNNING,
			PC_STATE_FATAL,
		} E_procState;


		/* ----------------------------------------------- */
		/* ---------------- COPLIEN FORM ----------------- */

		ProcInfo();
		ProcInfo(const std::string &name, int nbRestart, long startTime,
				long endTime);
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
		long getStartTime() const;
		long getEndTime() const;


		/* ----------------------------------------------- */
		/* ------------------- SETTERS ------------------- */

		void setState(int state);
		void setName(const std::string &name);
		void setHash(const std::string &hash);
		void setPid(int pid);
		void setNbRestart(int nbRestart);
		void setStartTime(long startTime);
		void setEndTime(long endTime);


		/* ----------------------------------------------- */
		/* ------------------- METHODS ------------------- */

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
		long startTime_;
		long endTime_;
};

#endif // PROC_INFO_HPP