/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:24:03 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/17 17:26:37 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

#include <fstream>

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
int ConfigParser::load(Logger *log, const std::string &cfPath)
{
	std::string line;
	std::ifstream ifs(cfPath);

	if (!ifs.is_open())
		return ERR_COULD_NOT_OPEN_FILE;

	while (std::getline(ifs, line));
		// std::cout << "line " << ++i << ": " << line << "\n";

	ifs.close();
	log->iUser("Pasring finished\n");

	return 0;
}


/* ----------------------------------------------- */
/* ---------------- HERLPER FUNCS ---------------- */

void swap(ConfigParser &a, ConfigParser &b)
{
	std::swap(a.cfPath_, b.cfPath_);
}