/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:24:03 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/21 16:03:33 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

#include <list>
#include <set>
#include <fstream>
#include <algorithm>
#include <ctype.h>

#include "ProgramBlock.hpp"

#define MAX_LINE_LEN 1024
#define MAX_NUMPROCS 100
#define MAX_STARTRETRIES 100
#define MAX_STARTTIME 86400
#define MAX_STOPTIME 86400
#define MAX_EXITCODE 255

static inline std::string lNbStr();
static inline std::string &revWsTrim(std::string &s);
static inline std::string quotesTrim(const std::string &str);

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

static inline std::string quotesTrim(const std::string &str)
{
	return str.substr(1, str.size() - 2);
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
				throw std::runtime_error(token + ": no spaces "
						"before value " + lNbStr());
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
		if (line[0] == '#')
			continue;
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

void ConfigParser::checkFileOpening(std::ifstream *ifs, const std::string& path)
{
	ifs->seekg(0, std::ios::end);
	if (!ifs->is_open() || !ifs->good()) {
		throw std::runtime_error("Failed to open config file (path: "
			+ path + ")\n");
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
	int tmp;
	std::cout << "Parsing " << token << "\n";

	if (param.size() == 3 && isOctalStr(param)) {
		tmp = stoi(param, 0, 8);
		pb->setUmask(tmp);
	} else {
		throw std::runtime_error(token + ": must be 3 octal digits "
				"- e.g. 002,024 " + lNbStr());
	}
}

void ConfigParser::parseWorkingDir(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	std::cout << "Parsing " << token << "\n";

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
	std::cout << "Parsing " << token << "\n";

	if (param == "false") {
		pb->setAutoStart(false);
	} else if (param == "true") {
		pb->setAutoStart(true);
	} else {
		throw std::runtime_error(token + ": must be false|true "
				+ lNbStr());
	}
}

void ConfigParser::parseAutoRestart(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	std::cout << "Parsing " << token << "\n";

	if (param == "false") {
		pb->setAutoRestart(ProgramBlock::AUTO_FALSE);
	} else if (param == "true") {
		pb->setAutoRestart(ProgramBlock::AUTO_TRUE);
	} else if (param == "unexpected") {
		pb->setAutoRestart(ProgramBlock::AUTO_UNEXP);
	} else {
		throw std::runtime_error(token + ": must be "
				"false|true|unexpected " + lNbStr());
	}
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
		throw std::runtime_error(token + ": must be between 0 and "
				+ std::to_string(MAX_STARTRETRIES)
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
		throw std::runtime_error(token + ": must be between 0 "
				"and " + std::to_string(MAX_STARTTIME)
				+ " seconds " + lNbStr());
	}
	pb->setStartTime(val);
}

void ConfigParser::parseExitCodes(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	std::cout << "Parsing " << token << "\n";
	int nb;
	bool err = false;
	std::set<int> s;
	std::vector<std::string> v = splitStr(param, ',');

	for (std::vector<std::string>::iterator it = v.begin(); it != v.end();
	    ++it) {
		if (it->empty() || !isDigitStr(*it))
			err = true;
		try {
			if (!err)
				nb = std::stoi(*it);
		} catch (const std::out_of_range &e) {
			err = true;
		}

		if (err || nb > MAX_EXITCODE) {
			throw std::runtime_error(token + ": must be numbers "
					"between 0 and 255 separated by ',' "
					"character " + lNbStr());
		}
		s.insert(nb);
	}

	pb->setExitCodes(s);
}

void ConfigParser::parseStopSignal(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	std::cout << "Parsing " << token << "\n";
	static const std::pair<int, std::string> p[7] = {
		{1, "HUP"},
		{2, "INT"},
		{3, "QUIT"},
		{4, "TERM"},
		{9, "KILL"},
		{30, "USR1"},
		{31, "USR2"}
	};

	for (size_t i = 0; i < sizeof(p) / sizeof(*p); ++i) {
		if (p[i].second == param) {
			pb->setStopSignal(p[i].first);
			return ;
		}
	}
	throw std::runtime_error(token + ": must be HUP|INT|QUIT|TERM|"
			"KILL|USR1|USR2 " + lNbStr());
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
		throw std::runtime_error(token + ": must be between 0 "
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
	std::cout << "Parsing " << token << "\n";
	std::vector<std::string> vars;
	std::pair<std::string, std::string> p;
	std::map<std::string, std::string>m;
	std::pair<std::map<std::string, std::string>::iterator, bool> ret;

	vars = splitEnv(param);
	for (std::vector<std::string>::iterator it = vars.begin();
	    it != vars.end(); ++it) {
		if (it->empty()) {
			goto err_parsing;
		}
		p = splitEnvVar(*it);
		if (checkEnvKey(p.first) == -1 || checkEnvVal(p.second) == -1) {
			goto err_parsing;
		}
		ret = m.insert(p);
		if (ret.second == false) {
			m.erase(ret.first);
			m.insert(p);
		}
	}
	vars.clear();
	for (std::map<std::string, std::string>::iterator it = m.begin();
	    it != m.end(); ++it) {
		vars.push_back(it->first + "=" + quotesTrim(it->second));
	}
	pb->setEnv(vars);
	return ;

err_parsing:
	throw std::runtime_error(token  + ": must be valid shell environment "
			"variable(s) of form KEY=\"value\" (value must be "
			"between quotes) separated by ',' character - e.g. "
			"KEY1=\"value1\",KEY2=\"value2\" " + lNbStr());
}

bool ConfigParser::isDigitStr(const std::string &str)
{
	int i = -1;

	while (str[++i])
		if (!isdigit(str[i]))
			return false;
	return true;
}

bool ConfigParser::isOctalStr(const std::string &str)
{
	int i = -1;

	while (str[++i])
		if (str[i] < '0' || str[i] > '7')
			return false;
	return true;
}

std::vector<std::string> ConfigParser::splitStr(const std::string &param,
		char c)
{
	size_t len;
	size_t prev = 0;
	size_t next = -1;
	std::vector<std::string> v;

	do {
		++next;
		prev = next;
		next = param.find(c, next);

		if (next == std::string::npos)
			len = std::string::npos;
		else
			len = next - prev;
		v.push_back(param.substr(prev, len));
	} while (next != std::string::npos);

	return v;
}

int ConfigParser::checkEnvKey(const std::string &varKey)
{
	int i = 0;

	if (varKey.empty())
		return -1;
	if (isdigit(varKey[0]))
		return -1;
	while (varKey[++i]) {
		if (!isalnum(varKey[i]))
			return -1;
	}
	return 0;
}

int ConfigParser::checkEnvVal(const std::string &varVal)
{
	int i = -1;
	int nbQuotes = 0;
	bool escape = false;

	if (varVal.empty())
		return -1;
	if (varVal[0] != '"' || varVal[varVal.size() - 1] != '"')
		return -1;

	while (varVal[++i]) {
		if (varVal[i] == '\\' && !escape) {
			escape = true;
			continue;
		}
		else if (varVal[i] == '"' && !escape) {
			++nbQuotes;
		}
		escape = false;
	}
	if (nbQuotes != 2)
		return -1;
	return 0;
}

// If nothing to split return empty pair
std::pair<std::string, std::string> ConfigParser::splitEnvVar(
		const std::string &var)
{
	size_t i = 0;
	std::pair<std::string, std::string> p;

	while (var[i] && isalnum(var[i]))
		++i;
	if (var[i] == '=') {
		p.first = var.substr(0, i);
		p.second = var.substr(i + 1, std::string::npos);
	}
	return p;
}

std::vector<std::string> ConfigParser::splitEnv(const std::string &param)
{
	bool inQuotes = false;
	bool escape = false;
	size_t len;
	size_t prev = 0;
	size_t i = -1;
	std::vector<std::string> v;

	while (param[++i]) {
		if (param[i] == '\\' && !escape) {
			escape = true;
			continue;

		} else if (param[i] == '"' && !escape) {
			inQuotes = !inQuotes;

		} else if (!inQuotes && param[i] == ',') {
			len = i - prev;
			v.push_back(param.substr(prev, len));
			prev = i + 1;
		}
		escape = false;
	}
	v.push_back(param.substr(prev, std::string::npos));
	return v;
}