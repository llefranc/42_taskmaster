/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramBlock.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 14:54:11 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/19 18:03:37 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROGRAM_BLOCK_HPP
#define PROGRAM_BLOCK_HPP

#include <string>
#include <vector>

#include "ProcInfo.hpp"

class ProgramBlock
{
	public:

		static enum {
			PB_STATE_REMOVE,
			PB_STATE_CHANGE_REMOVE,
			PB_STATE_NEW,
			PB_STATE_CHANGE,
			PB_STATE_UNCHANGE,
		} E_prgmBlockState;

		static enum {
			AUTO_FALSE,
			AUTO_TRUE,
			AUTO_UNEXP,
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
		const std::vector<ProcInfo>& getProcInfos() const;
		const std::string& getName() const;
		const std::string& getCmd() const;
		const int& getNumprocs() const;
		const std::string& getUmask() const;
		const std::string& getWorkDir() const;
		const bool& getAutoStart() const;
		const int& getAutoRestart() const;
		const int& getStartRetries() const;
		const int& getStartTime() const;
		const std::vector<int>& getExitCodes() const;
		const int& getStopSignal() const;
		const std::string& getLogOut() const;
		const std::string& getLogErr() const;
		const std::vector<std::string>& getEnv() const;


		/* ----------------------------------------------- */
		/* ------------------- SETTERS ------------------- */

		void setState(const int state);
		void setProcInfos(const std::vector<ProcInfo> &procInfos);
		void setName(const std::string &name);
		void setCmd(const std::string &cmd);
		void setNumProcs(const int numprocs);
		void setUmask(const std::string &umask);
		void setWorkDir(const std::string &workdir);
		void setAutoStart(const bool autoStart);
		void setAutoRestart(const int autoRestart);
		void setStartRetries(const int retries);
		void setStartTime(const int startTime);
		void setExitCodes(const std::vector<int> &exitCodes);
		void setStopSignal(const int stopSignal);
		void setLogOut(const std::string &logOut);
		void setLogErr(const std::string &logErr);
		void setEnv(const std::vector<std::string> &env);


		/* ----------------------------------------------- */
		/* ------------------- METHODS ------------------- */

		bool isCorrect() const;
		void clear();
		void print() const;


		/* ----------------------------------------------- */
		/* ---------------- HELPER FUNCS------------------ */

		friend void swap(ProgramBlock &a, ProgramBlock &b);


		/* ----------------------------------------------- */
		/*------------------ ATTRIBUTES ------------------ */

	private:

		int state_;			// default: PB_STATE_NEW
		std::vector<ProcInfo> procInfos_;
		std::string name_;		// mandatory
		std::string cmd_; 		// mandatory
		int numProcs_;			// default: 1
		std::string umask_;		// default: "022"
		std::string workDir_;		// default: ""
		bool autoStart_;		// default: false
		int autoRestart_;		// default: false
		int startRetries_;		// default: 0
		int startTime_;			// default: 0
		std::vector<int> exitCodes_;	// default: std::vector(1, 0)
		int stopSignal_;		// default: SIGTERM
		std::string logOut_;		// default: "/tmp"
		std::string logErr_;		// default: "/tmp"
		std::vector<std::string> env_;	// default: std::vector()
};

#endif // PROGRAM_BLOCK_HPP