/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramBlock.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 14:54:11 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/08 15:13:08 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROGRAM_BLOCK_HPP
#define PROGRAM_BLOCK_HPP

#include <string>
#include <vector>


class ProgramBlock
{
	public:

		enum {
			STATE_STOP,
			STATE_START,
			STATE_NEW,
			STATE_UPDATED,
			STATE_NEED_RM,
		} E_prgmState;

		enum {
			AUTO_FALSE,
			AUTO_TRUE,
			AUTO_UNEXP,
		} E_auto;

		ProgramBlock();
		ProgramBlock(const ProgramBlock& copyObj);
		~ProgramBlock();
		ProgramBlock& operator=(const ProgramBlock& assignObj);
		// operator==

		/* SETTERS & GETTERS */
		void setState(int state);
		const int& getState() const;
		void setProcs(const std::vector<std::pair<int, long> >& proc);
		const std::vector<std::pair<int, long> >& getProcs() const;
		void setName(const std::string& name);
		const std::string& getName() const;
		void setCommand(const std::string& cmd);
		const std::string& getCommand() const;
		void setNumprocs(const int numprocs);
		const int& getNumprocs() const;
		void setUnamsk(const std::string& umask);
		const std::string& getUnamsk() const;
		void setWorkdir(const std::string& workdir);
		const std::string& getWorkdir() const;
		void setAutoStart(const bool autoStart);
		const bool& getAutoStart() const;
		void setAutoRestart(const int autoRestart);
		const int& getAutoRestart() const;
		void setStartRetries(const int retries);
		const int& getStartRetries() const;
		void setStartTime(const int startTime);
		const int& getStartTime() const;
		void setExitCode(const std::vector<int>& exitCode);
		const std::vector<int>& getExitCode() const;
		void setStopSignal(const int stopSignal);
		const int& getStopSignal() const;
		void setLogOut(const std::string& logOut);
		const std::string& getLogOut() const;
		void setLogErr(const std::string& logErr);
		const std::string& getLogErr() const;
		void setEnv(const std::vector<std::string>& env);
		const std::vector<std::string>& getEnv() const;


	private:

		int state_;
		std::vector<std::pair<int, long> > procs_; // pair de pids + time
		std::string name_;
		std::string cmd_;
		int numprocs_;
		std::string umask_;
		std::string workDir_;
		bool autoStart_;
		int autoRestart_;
		int startRetries_;
		int startTime_;			// SHOULD BE in proc obj
		std::vector<int> exitCodes_;
		int stopSignal_;
		std::string logOut_;
		std::string logErr_;
		std::vector<std::string> env_;

};

#endif