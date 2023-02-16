/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 15:29:18 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/16 18:23:12 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <iostream>

class Logger
{
		/* ----------------------------------------------- */
		/* ---------------- COPLIEN FORM ----------------- */
	public:

		Logger();
		~Logger();

	private:

		Logger(const Logger &c);
		Logger& operator=(const Logger &a);

	public:


		/* ----------------------------------------------- */
		/* ------------------- METHODS ------------------- */

		int init(const std::string &path);
		void iUser(const std::string &str);
		void iFile(const std::string &str);
		void iAll(const std::string &str);
		void eUser(const std::string &str);
		void eFile(const std::string &str);
		void eAll(const std::string &str);


		/* ----------------------------------------------- */
		/*------------------ ATTRIBUTES ------------------ */

	private:

		std::string path_;
		std::ofstream logStream_;

};

#endif // LOGGER_HPP