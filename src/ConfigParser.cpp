/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:24:03 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/05 10:33:03 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

#include "ProgramBlock.hpp"

#include <list>
#include <set>
#include <fstream>
#include <algorithm>
#include <ctype.h>
#include <iostream>

#define MAX_LINE_LEN 1024
#define MAX_NAME_LEN 20
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

/**
 * Create a ConfigParser object. Copy the environment of the parent process that
 * launched taskmaster in order to populate the environment of ProgramBlocks.
*/
ConfigParser::ConfigParser(char **env) :
	cfPath_(), vecParentEnv_(), mapParentEnv_()
{
	int i = -1;
	int j;
	std::string var;
	std::pair<std::string, std::string> p;

	while (env[++i]) {
		j = 0;
		var = std::string(env[i]);
		vecParentEnv_.push_back(var);

		while (var[j] && var[j] != '=')
			++j;
		p.first = var.substr(0, j);
		p.second = "\"" + var.substr(j + 1, std::string::npos) + "\"";
		mapParentEnv_.insert(p);
	}
}

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

/**
 * Print line number when a parsing error occurs.
*/
static inline std::string lNbStr()
{
	return "(line "	+ std::to_string(lineNb) + ")\n";
}

/**
 * Remove whitespaces at the end of a string.
*/
static inline std::string &revWsTrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(),
	    s.rend(),
	    std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
	    s.end());
	return s;
}

/**
 * Remove the two quotes that surround an environment variable from argument of
 * token "env" in config file.
*/
static inline std::string quotesTrim(const std::string &str)
{
	return str.substr(1, str.size() - 2);
}


/* ----------------------------------------------- */
/* ------------------- METHODS ------------------- */

/**
 * Parse the configuration file and generate the list of ProgramBlocks. If an
 * error occurs, abort the parsing and throw a std::runtime_error exception.
*/
std::list<ProgramBlock> ConfigParser::load(const std::string &cfPath)
{
	ProgramBlock pb;
	std::list<ProgramBlock> pbList;
	bool inProgramBlock = false;
	std::string line;
	std::ifstream ifs(cfPath);

	checkFileOpening(&ifs, cfPath);
	lineNb = 0;
	cfPath_ = cfPath;

	while (std::getline(ifs, line)) {
		++lineNb;
		revWsTrim(line);

		if (!line.empty()) {
			if (line[0] == '#')
				continue;

			if (!inProgramBlock) {
				parseProgramName(pbList, &pb, line);
				inProgramBlock = true;
			} else {
				parseToken(&pb, line);
			}
		} else if (line.empty() && inProgramBlock) {
			savePb(&pbList, &pb, &inProgramBlock);
		}
	}
	if (inProgramBlock)
		savePb(&pbList, &pb, &inProgramBlock);

	if (pbList.empty())
		throw std::runtime_error("Config file is empty\n");

	generateProcInfos(&pbList);
	ifs.close();
	return pbList;
}

/**
 * Parse again the configuration file and generate the list of ProgramBlocks. If
 * an error occurs, abort the parsing and throw a std::runtime_error exception.
*/
std::list<ProgramBlock> ConfigParser::reload()
{
	return load(cfPath_);
}


/* ----------------------------------------------- */
/* --------------- PRIVATE METHODS --------------- */

/**
 * Check if the path to configuration file is correct.
*/
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

/**
 * Parse the program name which must be:
 * - First line of a program block.
 * - Surrounded by brackets ([programname]).
 * - Long of 1 to 20 alphanumeric characters inside brackets.
 * - Unique in the configuration file.
*/
void ConfigParser::parseProgramName(const std::list<ProgramBlock> &pbList,
		ProgramBlock *pb, const std::string &line)
{
	// std::cout << "Parsing program name: |" << line << "|\n";
	size_t i = 0;
	std::string name;

	if (line.empty() || line[0] != '[' || line[line.size() - 1] != ']') {
		throw std::runtime_error("Program name: must be surrounded by "
				"brackets - e.g. [name] " + lNbStr());
	}

	/* -2 for '[' and ']' characs */
	name = line.substr(1, line.size() - 2);

	/* +1 for '\0' charac */
	if (name.empty() || name.size() > MAX_NAME_LEN + 1) {
		throw std::runtime_error("Program name: between 1 and "
				+ std::to_string(MAX_NAME_LEN) + " characters "
				"- e.g. [1-20 characs] " + lNbStr());
	}
	while (i < name.size()) {
		if (!isalnum(name[i])) {
			throw std::runtime_error("Program name: only "
					"alphanumeric characters between "
					"brackets - e.g. [name] " + lNbStr());
		}
		++i;
	}
	for (std::list<ProgramBlock>::const_iterator it = pbList.begin();
	    it != pbList.end(); ++it) {
		if (it->getName() == name) {
			throw std::runtime_error("Program name: already used by"
					" another program block " + lNbStr());
		}
	}
	pb->setName(name);
}

/**
 * Parse a line looking for a token at the beginning of the line, and if a
 * correct token exist call the parsing method associated to it. Line can't be
 * empty and token's format must be "token=xxx" with no spaces around the '='.
*/
void ConfigParser::parseToken(ProgramBlock *pb, const std::string &line)
{
	methodPtr m;
	std::string tmpT;
	std::string token;
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

/**
 * Parse argument of token "cmd" which must be:
 * - Less than MAX_LINE_LEN characters.
*/
void ConfigParser::parseCmd(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";
	if (param.size() > MAX_LINE_LEN) {
		throw std::runtime_error(token + ": line too long, max "
				+ std::to_string(MAX_LINE_LEN) + " characters "
				+ lNbStr());
	}
	pb->setCmd(param);
}

/**
 * Parse argument of token "numprocs" which must be:
 * - Only digit characters.
 * - With a value between 1 and MAX_NUMPROCS.
*/
void ConfigParser::parseNumProcs(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";
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

/**
 * Parse argument of token "umask" which must be:
 * - 3 octals digit characters.
*/
void ConfigParser::parseUmask(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	int tmp;
	// std::cout << "Parsing " << token << "\n";

	if (param.size() == 3 && isOctalStr(param)) {
		tmp = stoi(param, 0, 8);
		pb->setUmask(tmp);
	} else {
		throw std::runtime_error(token + ": must be 3 octal digits "
				"- e.g. 002,024 " + lNbStr());
	}
}

/**
 * Parse argument of token "workingdir" which must be:
 * - Less than MAX_LINE_LEN characters.
*/
void ConfigParser::parseWorkingDir(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";

	if (param.size() > MAX_LINE_LEN) {
		throw std::runtime_error(token + ": line too long, max "
				+ std::to_string(MAX_LINE_LEN) + " characters "
				+ lNbStr());
	}
	pb->setWorkDir(param);
}

/**
 * Parse argument of token "autostart" which must be:
 * - false|true.
*/
void ConfigParser::parseAutoStart(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";

	if (param == "false") {
		pb->setAutoStart(false);
	} else if (param == "true") {
		pb->setAutoStart(true);
	} else {
		throw std::runtime_error(token + ": must be false|true "
				+ lNbStr());
	}
}

/**
 * Parse argument of token "autorestart" which must be:
 * - false|true|unexpected.
*/
void ConfigParser::parseAutoRestart(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";

	if (param == "false") {
		pb->setAutoRestart(ProgramBlock::E_AUTO_FALSE);
	} else if (param == "true") {
		pb->setAutoRestart(ProgramBlock::E_AUTO_TRUE);
	} else if (param == "unexpected") {
		pb->setAutoRestart(ProgramBlock::E_AUTO_UNEXP);
	} else {
		throw std::runtime_error(token + ": must be "
				"false|true|unexpected " + lNbStr());
	}
}

/**
 * Parse argument of token "startretries" which must be:
 * - Only digit characters.
 * - With a value between 0 and MAX_STARTRETRIES.
*/
void ConfigParser::parseStartRetries(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";
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

/**
 * Parse argument of token "starttime" which must be:
 * - Only digit characters.
 * - With a value between 0 and MAX_STARTTIME.
*/
void ConfigParser::parseStartTime(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";
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

/**
 * Parse argument of token "exitcodes" which must be:
 * - One or many exit codes, which are numbers between 0 and 255.
 * - Exit codes must be separated by ',' character.
 * - No spaces around ',' separator character.
*/
void ConfigParser::parseExitCodes(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";
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

/**
 * Parse argument of token "starttime" which must be:
 * - HUP|INT|QUIT|TERM|KILL|USR1|USR2.
*/
void ConfigParser::parseStopSignal(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";
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

/**
 * Parse argument of token "stoptime" which must be:
 * - Only digit characters.
 * - With a value between 0 and MAX_STOPTIME.
*/
void ConfigParser::parseStopTime(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";
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

/**
 * Parse argument of token "stdout" which must be:
 * - Less than MAX_LINE_LEN characters.
*/
void ConfigParser::parseStdout(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";
	if (param.size() > MAX_LINE_LEN) {
		throw std::runtime_error(token + ": line too long, max "
				+ std::to_string(MAX_LINE_LEN) + " characters "
				+ lNbStr());
	}
	pb->setLogOut(param);
}

/**
 * Parse argument of token "stderr" which must be:
 * - Less than MAX_LINE_LEN characters.
*/
void ConfigParser::parseStderr(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";
	if (param.size() > MAX_LINE_LEN) {
		throw std::runtime_error(token + ": line too long, max "
				+ std::to_string(MAX_LINE_LEN) + " characters "
				+ lNbStr());
	}
	pb->setLogErr(param);
}

/**
 * Parse argument of token "env" which must be:
 * - One or many environment variables with the format KEY="value".
 * - Environment variables must be separated by ',' character.
 * - No spaces around ',' separator character.
*/
void ConfigParser::parseEnv(ProgramBlock *pb, const std::string &token,
		const std::string &param)
{
	// std::cout << "Parsing " << token << "\n";
	std::vector<std::string> vars;
	std::pair<std::string, std::string> p;
	std::map<std::string, std::string>m = mapParentEnv_;
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

/**
 * If the newly parsed ProgramBlock is correct, add it to the list of saved
 * ProgramBlocks.
*/
void ConfigParser::savePb(std::list<ProgramBlock> *pbList, ProgramBlock *pb,
		bool *inProgramBlock)
{
	if (pb->isCorrect()) {
		*inProgramBlock = false;
		if (pb->getEnv().empty())
			pb->setEnv(vecParentEnv_);
		pbList->push_back(*pb);
		pb->clear();
	} else {
		throw std::runtime_error("Program block [" + pb->getName()
				+ "] is missing cmd\n");
	}
}

/**
 * Generate x empty ProcInfos for a ProgramBlock based on its numprocs
 * attribute. Each ProcInfo will be name "ProgramBlock::name_x".
*/
void ConfigParser::generateProcInfos(std::list<ProgramBlock> *pbList)
{
	std::vector<ProcInfo> vPis;

	for (std::list<ProgramBlock>::iterator it = pbList->begin();
	    it != pbList->end(); ++it) {
		vPis.clear();
		for (int i = 0; i < it->getNumprocs(); ++i) {
			ProcInfo pi(it->getName() + + "_" + std::to_string(i));
			vPis.push_back(pi);
		}
		it->setProcInfos(vPis);
	}
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

/**
 * Parse key of an environment variable from argument of token "env".
*/
int ConfigParser::checkEnvKey(const std::string &varKey)
{
	int i = 0;

	if (varKey.empty())
		return -1;
	if (isdigit(varKey[0]))
		return -1;
	while (varKey[++i]) {
		if (!(isalnum(varKey[i]) || varKey[i] == '_'))
			return -1;
	}
	return 0;
}

/**
 * Parse value of an environment variable from argument of token "env".
*/
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

/**
 * Split an environment variable from argument of token "env" based on '='
 * character. Return an empty pair if no '=' character.
*/
std::pair<std::string, std::string> ConfigParser::splitEnvVar(
		const std::string &var)
{
	size_t i = 0;
	std::pair<std::string, std::string> p;

	while (var[i] && (isalnum(var[i]) || var[i] == '_'))
		++i;
	if (var[i] == '=') {
		p.first = var.substr(0, i);
		p.second = var.substr(i + 1, std::string::npos);
	}
	return p;
}

/**
 * Split the argument of token "env" in several environment variables based on
 * ',' separator.
*/
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