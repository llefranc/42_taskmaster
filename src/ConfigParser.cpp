/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:24:03 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/19 19:24:15 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

#include <list>
#include <fstream>

#include "ProgramBlock.hpp"

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
/* ------------------- METHODS ------------------- */
#include <iostream>
std::list<ProgramBlock> ConfigParser::load(const std::string &cfPath)
{
	ProgramBlock pb;
	std::list<ProgramBlock> pbList;
	bool inProgramBlock = false;
	std::string line;
	std::ifstream ifs(cfPath);

	if (!ifs.is_open()) {
		throw std::runtime_error("Failed to open config file (path: "
			+ cfPath + ")\n");
	}

	while (std::getline(ifs, line)) {
		++lineNb;
	}
		// std::cout << "line " << ++i << ": " << line << "\n";

	if (inProgramBlock) {
		if (pb.isCorrect()) {
			pbList.push_back(pb);
		} else if (!pb.getName().empty() && pb.getCmd().empty()) {
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
/* ---------------- HERLPER FUNCS ---------------- */

void swap(ConfigParser &a, ConfigParser &b)
{
	std::swap(a.cfPath_, b.cfPath_);
}