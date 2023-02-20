/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:24:03 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/20 14:50:55 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

#include <list>
#include <fstream>
#include <algorithm>
#include <ctype.h>

#include "ProgramBlock.hpp"

#define MAX_LINE_LEN 1024
#define MAX_NUMPROCS 100
#define MAX_STARTRETRIES 100
#define MAX_STARTTIME 86400
#define MAX_STOPTIME 86400

static inline std::string lNbStr();
static inline std::string &revWsTrim(std::string &s);

static int lineNb = 0;


/* ----------------------------------------------- */
/* ---------------- COPLIEN FORM ----------------- */

ConfigParser::ConfigParser() : cfPath_() {}

ConfigParser::ConfigParser(const std::string &cfPath) : cfPath_(cfPath) {}

ConfigParser::ConfigParser(const ConfigParser &c) : cfPath_(c.cfPath_) {}

ConfigParser::~ConfigParser() {}

ConfigParser& ConfigParser::operator=(ConfigParser a)
{
	swap(*this, a);
	return *this;
}


/* ----------------------------------------------- */
/* ------------------- GETTERS ------------------- */

const std::string& ConfigParser::getCfPath() const
{
	return cfPath_;
}


/* ----------------------------------------------- */
/* ------------------- SETTERS ------------------- */

void ConfigParser::setCfPath(const std::string &cfPath)
{
	cfPath_ = cfPath;
}


/* ----------------------------------------------- */
/* ---------------- HERLPER FUNCS ---------------- */

void swap(ConfigParser &a, ConfigParser &b)
{
	std::swap(a.cfPath_, b.cfPath_);
}

static inline std::string lNbStr()
{
	return "(line "	+ std::to_string(lineNb) + ")\n";
}

static inline std::string &revWsTrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(),
			s.rend(),
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
			s.end());
	return s;
}


/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */


#include <iostream>
void ConfigParser::parseToken(ProgramBlock *pb, const std::string &line)
{
	methodPtr m;
	std::string token;
	std::string tmpT;
	std::string param;

	for (size_t i = 0; i < (sizeof(tokMeths_) / sizeof(*tokMeths_)); ++i) {
		tmpT = tokMeths_[i].first;
		if (std::search(tmpT.begin(), tmpT.end(), line.begin(),
				line.begin() + tmpT.size()) != tmpT.end()) {

			m = tokMeths_[i].second;
			token = tmpT.substr(0, tmpT.size() - 1);
			param = line.substr(tmpT.size());

			if (param.empty()) {
				throw std::runtime_error(token + ": missing "
						"value " + lNbStr());
			} else if (std::isspace(param[0])) {
				throw std::runtime_error(token + ": no "
						"whitespaces at beginning "
						"of value " + lNbStr());
			}
			(this->*m)(pb, token, param);
			return ;
		}
	}
	throw std::runtime_error("Error while parsing key " + lNbStr());
}

std::list<ProgramBlock> ConfigParser::load(const std::string &cfPath)
{
	ProgramBlock pb;
	std::list<ProgramBlock> pbList;
	bool inProgramBlock = false;
	std::string line;
	std::ifstream ifs(cfPath);

	checkFileOpening(&ifs, cfPath);

	while (std::getline(ifs, line)) {
		++lineNb;
		revWsTrim(line);
		if (!line.empty()) {
			parseToken(&pb, line);
		}
	}

	if (inProgramBlock) {
		if (pb.isCorrect()) {
			pbList.push_back(pb);
		} else {
			throw std::runtime_error(pb.getName() + " is missing "
					"cmd line\n");
		}
	}
	if (pbList.empty())
		throw std::runtime_error("Config file is empty\n");

	ifs.close();
	return pbList;
}


/* ----------------------------------------------- */
/* --------------- PRIVATE METHODS --------------- */


void ConfigParser::checkFileOpening(std::ifstream *ifs, const std::string& cfPath)
{
	ifs->seekg(0, std::ios::end);
	if (!ifs->is_open() || !ifs->good()) {
		throw std::runtime_error("Failed to open config file (path: "
			+ cfPath + ")\n");
	}
	ifs->clear();
	ifs->seekg(0);
}

void ConfigParser::parseCmd(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	std::cout << "Parsing " << token << "\n";
	if (param.size() > MAX_LINE_LEN) {
		throw std::runtime_error(token + ": line too long, max "
				+ std::to_string(MAX_LINE_LEN) + " characters "
				+ lNbStr());
	}
	pb->setCmd(param);
}

void ConfigParser::parseNumProcs(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	std::cout << "Parsing " << token << "\n";
	int val;
	bool outOfRange = false;

	if (!isDigitStr(param)) {
		throw std::runtime_error(token + ": only digit characters "
				+ lNbStr());
	}
	try {
		val = std::stoi(param);
	} catch (const std::out_of_range &e) {
		outOfRange = true;
	}
	if (outOfRange || val > MAX_NUMPROCS || val < 1) {
		throw std::runtime_error(token + ": value must be between 1 "
				"and " + std::to_string(MAX_NUMPROCS)
				+ " " + lNbStr());
	}
	pb->setNumProcs(val);
}

void ConfigParser::parseUmask(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	(void)pb;
	(void)param;
	(void)token;
	std::cout << "Parsing " << token << "\n";
}

void ConfigParser::parseWorkingDir(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	if (param.size() > MAX_LINE_LEN) {
		throw std::runtime_error(token + ": line too long, max "
				+ std::to_string(MAX_LINE_LEN) + " characters "
				+ lNbStr());
	}
	pb->setWorkDir(param);
}

void ConfigParser::parseAutoStart(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	(void)pb;
	(void)param;
	(void)token;
std::cout << "Parsing " << token << "\n";
}

void ConfigParser::parseAutoRestart(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	(void)pb;
	(void)param;
	(void)token;
	std::cout << "Parsing " << token << "\n";
}

void ConfigParser::parseStartRetries(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	std::cout << "Parsing " << token << "\n";
	int val;
	bool outOfRange = false;

	if (!isDigitStr(param)) {
		throw std::runtime_error(token + ": only digit characters "
				+ lNbStr());
	}
	try {
		val = std::stoi(param);
	} catch (const std::out_of_range  &e) {
		outOfRange = true;
	}
	if (outOfRange || val > MAX_STARTRETRIES) {
		throw std::runtime_error(token + ": value must be between 0 "
				"and " + std::to_string(MAX_STARTRETRIES)
				+ " " + lNbStr());
	}
	pb->setStartRetries(val);
}

void ConfigParser::parseStartTime(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	std::cout << "Parsing " << token << "\n";
	int val;
	bool outOfRange = false;

	if (!isDigitStr(param)) {
		throw std::runtime_error(token + ": only digit characters "
				+ lNbStr());
	}
	try {
		val = std::stoi(param);
	} catch (const std::out_of_range &e) {
		outOfRange = true;
	}
	if (outOfRange || val > MAX_STARTTIME) {
		throw std::runtime_error(token + ": value must be between 0 "
				"and " + std::to_string(MAX_STARTTIME)
				+ " seconds " + lNbStr());
	}
	pb->setStartTime(val);
}

void ConfigParser::parseExitCodes(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	(void)pb;
	(void)param;
	(void)token;
	std::cout << "Parsing " << token << "\n";
}

void ConfigParser::parseStopSignal(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	(void)pb;
	(void)param;
	(void)token;
	std::cout << "Parsing " << token << "\n";
}

void ConfigParser::parseStopTime(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	std::cout << "Parsing " << token << "\n";
	int val;
	bool outOfRange = false;

	if (!isDigitStr(param)) {
		throw std::runtime_error(token + ": only digit characters "
				+ lNbStr());
	}
	try {
		val = std::stoi(param);
	} catch (const std::out_of_range &e) {
		outOfRange = true;
	}
	if (outOfRange || val > MAX_STOPTIME) {
		throw std::runtime_error(token + ": value must be between 0 "
				"and " + std::to_string(MAX_STOPTIME)
				+ " seconds " + lNbStr());
	}
	pb->setStopTime(val);
}

void ConfigParser::parseStdout(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	std::cout << "Parsing " << token << "\n";
	if (param.size() > MAX_LINE_LEN) {
		throw std::runtime_error(token + ": line too long, max "
				+ std::to_string(MAX_LINE_LEN) + " characters "
				+ lNbStr());
	}
	pb->setLogOut(param);
}
void ConfigParser::parseStderr(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	std::cout << "Parsing " << token << "\n";
	if (param.size() > MAX_LINE_LEN) {
		throw std::runtime_error(token + ": line too long, max "
				+ std::to_string(MAX_LINE_LEN) + " characters "
				+ lNbStr());
	}
	pb->setLogErr(param);
}

void ConfigParser::parseEnv(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	(void)pb;
	(void)param;
	(void)token;
	std::cout << "Parsing " << token << "\n";
}

bool ConfigParser::isDigitStr(const std::string &str)
{
	int i = -1;

	while (str[++i])
		if (!isdigit(str[i]))
			return false;
	return true;
}