/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramBlock.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 14:54:11 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/11 15:00:24 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROGRAM_BLOCK_HPP
#define PROGRAM_BLOCK_HPP

#include <string>
#include <vector>
#include <set>

#include "ProcInfo.hpp"

class ProgramBlock
{
	public:

		/**
		 * The 5 possible states of a ProgramBlock (PB) :
		 *
		 * @E_STATE_NEW:
		 * 	This PB was seen for the first tiime in config file.
		 * 	No special case to treat.
		 * @E_STATE_UNCHANGE:
		 * 	A reload occured and nothing was changed in this PB.
		 * 	No special case to treat.
		 * @E_STATE_CHANGE:
		 * 	A reload occured and this PB was modified. This is the
		 * 	new version of PB that will now be used for taskmaster
		 * 	commands.
		 * @E_STATE_CHANGE_REMOVE :
		 * 	A reload occured and this PB was modified. This is the
		 * 	previous PB version that need to be destroyed.
		 * @E_STATE_REMOVE:
		 * 	A reload occured and this PB is not existing anymore in
		 * 	config file, it will be removed.
		*/
		static enum {
			E_STATE_NEW,
			E_STATE_UNCHANGE,
			E_STATE_CHANGE,
			E_STATE_CHANGE_REMOVE,
			E_STATE_REMOVE,
		} E_prgmBlockState;

		/**
		 * The 3 possible values for autoStart_ attribute.
		*/
		static enum {
			E_AUTO_FALSE,
			E_AUTO_TRUE,
			E_AUTO_UNEXP,
		} E_auto;


		/* ----------------------------------------------- */
		/* ---------------- COPLIEN FORM ----------------- */

		ProgramBlock();
		ProgramBlock(const ProgramBlock &c);
		~ProgramBlock();
		ProgramBlock& operator=(ProgramBlock a);


		/* ----------------------------------------------- */
		/* ------------------- GETTERS ------------------- */

		const int& getState() const;
		std::vector<ProcInfo>& getProcInfos();
		const std::string& getName() const;
		const std::string& getCmd() const;
		const int& getNumprocs() const;
		const int& getUmask() const;
		const std::string& getWorkDir() const;
		const bool& getAutoStart() const;
		const int& getAutoRestart() const;
		const int& getStartRetries() const;
		const int& getStartTime() const;
		const std::set<int>& getExitCodes() const;
		const int& getStopSignal() const;
		const int& getStopTime() const;
		const std::string& getLogOut() const;
		const std::string& getLogErr() const;
		const std::vector<std::string>& getEnv() const;
		ProcInfo *getProcInfoByPid(int pid);
		ProcInfo *getProcInfoByName(const std::string &name);


		/* ----------------------------------------------- */
		/* ------------------- SETTERS ------------------- */

		void setState(const int state);
		void setProcInfos(const std::vector<ProcInfo> &procInfos);
		void setName(const std::string &name);
		void setCmd(const std::string &cmd);
		void setNumProcs(const int numprocs);
		void setUmask(const int umask);
		void setWorkDir(const std::string &workdir);
		void setAutoStart(const bool autoStart);
		void setAutoRestart(const int autoRestart);
		void setStartRetries(const int retries);
		void setStartTime(const int startTime);
		void setExitCodes(const std::set<int> &exitCodes);
		void setStopSignal(const int stopSignal);
		void setStopTime(const int stopTime);
		void setLogOut(const std::string &logOut);
		void setLogErr(const std::string &logErr);
		void setEnv(const std::vector<std::string> &env);


		/* ----------------------------------------------- */
		/* ------------------- METHODS ------------------- */

		bool isCorrect() const;
		bool isRestartNeeded(int status) const;
		void clear();
		void print() const;


		/* ----------------------------------------------- */
		/* ------- NON-MEMBER FUNCTION OVERLOADS --------- */

		friend bool operator==(const ProgramBlock& lhs,
				const ProgramBlock& rhs);


		/* ----------------------------------------------- */
		/* ---------------- HELPER FUNCS ----------------- */

		friend void swap(ProgramBlock &a, ProgramBlock &b);


		/* ----------------------------------------------- */
		/*------------------ ATTRIBUTES ------------------ */

	private:

		int state_;			// default: E_STATE_NEW
		std::vector<ProcInfo> procInfos_;
		std::string name_;		// mandatory
		std::string cmd_; 		// mandatory
		int numProcs_;			// default: 1
		int umask_;			// default: 0022 (octal value)
		std::string workDir_;		// default: ""
		bool autoStart_;		// default: false
		int autoRestart_;		// default: false
		int startRetries_;		// default: 0
		int startTime_;			// default: 0
		std::set<int> exitCodes_;	// default: std::set(1, 0)
		int stopSignal_;		// default: SIGTERM
		int stopTime_;			// default: 0
		std::string logOut_;		// default: ""
		std::string logErr_;		// default: ""
		std::vector<std::string> env_;	// default: std::vector()
};

#endif // PROGRAM_BLOCK_HPP