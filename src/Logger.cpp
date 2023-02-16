/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 15:30:31 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/16 19:30:11 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

#include <iomanip>
#include <ctime>
#include <chrono>
#include <sstream>


/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

Logger::Logger() : path_(), logStream_() {}

Logger::~Logger()
{
	if (logStream_.is_open())
		logStream_.close();
}


/* ----------------------------------------------- */
/* ------------------- GETTERS ------------------- */

const std::string& Logger::getPath() const
{
	return path_;
}


/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */

void Logger::init(const std::string &path)
{
	if (logStream_.is_open()) {
		throw std::runtime_error("Logger is already associated to "
				"another log file (path: " + path_ + ")\n");
	}

	path_ = path;
	logStream_.open(path, std::ofstream::out | std::ofstream::app);
	if (!logStream_.is_open()) {
		throw std::runtime_error("[FATAL] Failed to open log file "
				"(path: " + path + ")\n");
	}
}

void Logger::iUser(const std::string &str)
{
	std::cout << "[INFO ] " << str;
}

void Logger::iFile(const std::string &str)
{
	if (!logStream_.is_open())
		throw std::runtime_error("[FATAL] Log file isn't open\n");

	logStream_ << "[INFO ] " << dateToStr() << str;
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

	logStream_ << "[ERROR] " << dateToStr() << str;
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

std::string Logger::dateToStr()
{
	std::stringstream ss;
	auto time = std::time(nullptr);

	ss << std::put_time(std::localtime(&time), "%Y-%m-%d %X");
	return ss.str() + ": ";
}