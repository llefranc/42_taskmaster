/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:24:32 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/19 19:07:03 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <list>

#include "ProgramBlock.hpp"

class ConfigParser
{
	public:

		static enum {
			ERR_COULD_NOT_OPEN_FILE = -1,
			ERR_PARSING = -2,
		} E_errStatus;


		/* ----------------------------------------------- */
		/* ---------------- COPLIEN FORM ----------------- */

		ConfigParser();
		ConfigParser(const std::string &cfPath);
		ConfigParser(const ConfigParser &c);
		~ConfigParser();
		ConfigParser& operator=(ConfigParser a);


		/* ----------------------------------------------- */
		/* ------------------- GETTERS ------------------- */

		const std::string& getCfPath() const;


		/* ----------------------------------------------- */
		/* ------------------- SETTERS ------------------- */

		void setCfPath(const std::string &cfPath);


		/* ----------------------------------------------- */
		/* ------------------- METHODS ------------------- */

		std::list<ProgramBlock> load(const std::string &cfPath);
		// int reload(Logger *log, const std::string &cfPath);


		/* ----------------------------------------------- */
		/* ---------------- HELPER FUNCS------------------ */

		friend void swap(ConfigParser &a, ConfigParser &b);


	private:

		std::string cfPath_;

};

#endif