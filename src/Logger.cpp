/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 15:30:31 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/19 18:56:08 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

#include <iomanip>
#include <ctime>
#include <chrono>
#include <sstream>


/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

/**
 * Default constructor.
*/
Logger::Logger()
	: path_(), logStream_() {}

/**
 * Destructor, close the stream to the logfile if this one is open.
*/
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

/**
 * init() - Open the log file.
 * @path: The path to the log file.
 *
 * Open the log file used by this class. Throw a std::runtime_error exception
 * if Logger is already associated to a file, or if file opening fails.
*/
void Logger::init(const std::string &path)
{
	if (logStream_.is_open()) {
		throw std::runtime_error("[ERROR] Logger is already associated "
				"to another log file (path: " + path_ + ")\n");
	}

	path_ = path;
	logStream_.open(path, std::ofstream::out | std::ofstream::app);
	if (!logStream_.is_open()) {
		throw std::runtime_error("[ERROR] Failed to open log file "
				"(path: " + path + ")\n");
	}
}

/**
 * iUser() - Output an INFO log to stdout.
 * @str: The INFO message to output.
*/
void Logger::iUser(const std::string &str)
{
	std::cout << "[INFO ] " << str;
}

/**
 * iFile() - Output an INFO log to logfile.
 * @str: The INFO message to output.
 *
 * Output an INFO log to logfile with a timestamp. Throw a std::runtime_error
 * exception if logfile isn't open or if an error occured during the write.
*/
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

/**
 * iAll() - Output an INFO log to stdout and to logfile.
 * @str: The INFO message to output.
 *
 * Calls iUser() and iFile(). Refers to this methods for more information.
*/
void Logger::iAll(const std::string &str)
{
	iUser(str);
	iFile(str);
}

/**
 * eUser() - Output an ERROR log to stdout.
 * @str: The ERROR message to output.
*/
void Logger::eUser(const std::string &str)
{
	std::cerr << "[ERROR] " << str;
}

/**
 * eFile() - Output an ERROR log to logfile.
 * @str: The ERROR message to output.
 *
 * Output an ERROR log to logfile with a timestamp. Throw a std::runtime_error
 * exception if logfile isn't open or if an error occured during the write.
*/
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

/**
 * eAll() - Output an ERROR log to stdout and to logfile.
 * @str: The ERROR message to output.
 *
 * This method calls eUser() and eFile(). Refers to this methods for more
 * information.
*/

void Logger::eAll(const std::string &str)
{
	eUser(str);
	eFile(str);
}

/**
 * dateToStr() - Return a timestamp (ex: "2023-02-17 13:46:58: ").
 *
 * @return A timestamp as a string.
*/
std::string Logger::dateToStr()
{
	std::stringstream ss;
	auto time = std::time(nullptr);

	ss << std::put_time(std::localtime(&time), "%Y-%m-%d %X");
	return ss.str() + ": ";
}