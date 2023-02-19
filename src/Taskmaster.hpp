/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Taskmaster.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:21:22 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/17 17:04:46 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TASK_MASTER_HPP
#define TASK_MASTER_HPP

#include "Logger.hpp"
#include "ConfigParser.hpp"


extern int g_nbSigChldReceived;

class TaskMaster
{
		/* ----------------------------------------------- */
		/* ---------------- COPLIEN FORM ----------------- */
	public:

		TaskMaster();
		~TaskMaster();

	private:

		TaskMaster(const TaskMaster &c);
		TaskMaster& operator=(const TaskMaster &a);

	public:

		/* ----------------------------------------------- */
		/* ------------------- METHODS ------------------- */

		void initLogger(const std::string &logPath);
		void initConfigParser(const std::string &cfPath);
		void shellRoutine();


		/* ----------------------------------------------- */
		/*------------------ ATTRIBUTES ------------------ */

	private:

		Logger log_;
		ConfigParser configParser_;

};

#endif // TASK_MASTER_HPP