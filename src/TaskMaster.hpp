/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TaskMaster.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:21:22 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/05 11:22:49 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TASK_MASTER_HPP
#define TASK_MASTER_HPP

#include <list>

#include "Logger.hpp"
#include "ConfigParser.hpp"
#include "Spawner.hpp"
#include "ProgramBlock.hpp"

class TaskMaster
{
	private:
		enum {
			SHELL_CONTINUE,
			SHELL_EXIT,
		} E_shellStatus;

		/* ----------------------------------------------- */
		/* ---------------- COPLIEN FORM ----------------- */
	public:

		TaskMaster(char **env);
		~TaskMaster();

	private:

		TaskMaster(const TaskMaster &c);
		TaskMaster& operator=(const TaskMaster &a);

	public:

		/* ----------------------------------------------- */
		/* ------------------- GETTERS ------------------- */

		Logger *getLogger() const;


		/* ----------------------------------------------- */
		/* ------------------- SETTERS ------------------- */

		void setLogger(Logger *log);


		/* ----------------------------------------------- */
		/* ------------------- METHODS ------------------- */

		void initConfigParser(const std::string &cfPath);
		void shellRoutine();

	private:

		std::vector<std::string> splitEntry(const std::string line);
		void updatePbList(std::list<ProgramBlock> *newPbList);
		std::list<ProgramBlock>::iterator findPbByName(
				std::list<ProgramBlock> *pbList,
				const std::string &name);
		void getProgExecutionInfoByName(const std::string& name,
				std::pair<ProgramBlock*, ProcInfo*> &info);

		int execCmd(const std::vector<std::string> &tokens);
		int execStatus(const std::vector<std::string> &tokens);
		int execStart(const std::vector<std::string> &tokens);
		int execStop(const std::vector<std::string> &tokens);
		int execRestart(const std::vector<std::string> &tokens);
		int execReload(const std::vector<std::string> &tokens);
		int execExit(const std::vector<std::string> &tokens);

		/* ----------------------------------------------- */
		/*------------------ ATTRIBUTES ------------------ */

	private:

		Logger* log_;
		ConfigParser configParser_;
		Spawner spawner_;
		std::list<ProgramBlock> pbList_;

		typedef int (TaskMaster::*methodPtr)(
				const std::vector<std::string> &);

		const std::pair<std::string, methodPtr> cmdMeths_[6] = {
			std::make_pair<std::string, methodPtr>("status",
					&TaskMaster::execStatus),
			std::make_pair<std::string, methodPtr>("start",
					&TaskMaster::execStart),
			std::make_pair<std::string, methodPtr>("stop",
					&TaskMaster::execStop),
			std::make_pair<std::string, methodPtr>("restart",
					&TaskMaster::execRestart),
			std::make_pair<std::string, methodPtr>("reload",
					&TaskMaster::execReload),
			std::make_pair<std::string, methodPtr>("exit",
					&TaskMaster::execExit),
		};

};

#endif // TASK_MASTER_HPP