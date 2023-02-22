/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TaskMaster.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:21:22 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/22 10:48:42 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TASK_MASTER_HPP
#define TASK_MASTER_HPP

#include <list>

#include "Logger.hpp"
#include "ConfigParser.hpp"
#include "Spawner.hpp"
#include "ProgramBlock.hpp"

extern int g_nbProcessZombies;

class TaskMaster
{
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


		/* ----------------------------------------------- */
		/*------------------ ATTRIBUTES ------------------ */

	private:

		Logger* log_;
		ConfigParser configParser_;
		Spawner spawner_;

		std::list<ProgramBlock> pbList_;
		std::list<ProgramBlock> pb_;
		char **env_;
};

#endif // TASK_MASTER_HPP