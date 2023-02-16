/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 15:30:31 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/16 18:42:14 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"


/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

Logger::Logger() : path_(), logStream_() {}

Logger::~Logger()
{
	if (logStream_.is_open())
		logStream_.close();
}


/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */

int Logger::init(const std::string &path)
{
	if (logStream_.is_open()) {
		eAll("Logger is already associated to another log file (path: "
				+ path_ + ")\n");
		return -1;
	}

	path_ = path;
	logStream_.open(path, std::ofstream::out | std::ofstream::app);
	if (!logStream_.is_open()) {
		throw std::runtime_error("[FATAL] Failed to open log file "
				"(path: " + path + ")\n");
	}
	return 0;
}

void Logger::iUser(const std::string &str)
{
	std::cout << "[INFO ] " << str;
}

void Logger::iFile(const std::string &str)
{
	if (!logStream_.is_open())
		throw std::runtime_error("[FATAL] Log file isn't open\n");

	logStream_ << "[INFO ] " << str;

	if (logStream_.fail()) {
		throw std::runtime_error("[FATAL] Write to log file failed "
			"(path " + path_ + ")\n");
	}
}

void Logger::iAll(const std::string &str)
{
	iUser(str);
	iFile(str);
}

void Logger::eUser(const std::string &str)
{
	std::cerr << "[ERROR] " << str;
}

void Logger::eFile(const std::string &str)
{
	if (!logStream_.is_open())
		throw std::runtime_error("[FATAL] Log file isn't open\n");

	logStream_ << "[ERROR] " << str;
	if (logStream_.fail()) {
		throw std::runtime_error("[FATAL] Write to log file failed "
			"(path " + path_ + ")\n");
	}
}

void Logger::eAll(const std::string &str)
{
	eUser(str);
	eFile(str);
}