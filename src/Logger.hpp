/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 15:29:18 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/10 19:55:35 by llefranc         ###   ########.fr       */
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


		/* ----------------------------------------------- */
		/* ------------------- GETTERS ------------------- */

	public:

		const std::string& getPath() const;


		/* ----------------------------------------------- */
		/* ------------------- METHODS ------------------- */

		void init(const std::string &path);
		void iUser(const std::string &str);
		void iFile(const std::string &str);
		void iAll(const std::string &str);
		void eUser(const std::string &str);
		void eFile(const std::string &str);
		void eAll(const std::string &str);
		void flushFile();
		static std::string dateToStr();


		/* ----------------------------------------------- */
		/*------------------ ATTRIBUTES ------------------ */

	private:

		std::string path_;
		std::ofstream logStream_;

};

#endif // LOGGER_HPP