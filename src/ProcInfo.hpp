#ifndef PROCINFO_HPP
# define PROCINFO_HPP

#include <string>

class ProcInfo
{
	public:
		ProcInfo(std::string const&, int state);
		~ProcInfo();
		ProcInfo(ProcInfo&);
		ProcInfo& operator=(ProcInfo&);
		
		std::string& getName();
		int getPid();
		int getNbRestart();
		int getState();
		long getStartTime();
		long getEndTime();

		void setName(std::string&);
		void setPid(int);
		void setNbRestart(int);
		void setState(int);
		void setStartTime(long);
		void setEndTime(long);
		void toString();

	private:
		std::string name_;
		pid_t pid_;
		int state_;
		int nbRestart_;
		long startTime_;
		long endTime_;
};

#endif