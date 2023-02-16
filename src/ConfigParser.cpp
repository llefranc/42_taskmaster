/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:24:03 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/16 14:22:47 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"


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