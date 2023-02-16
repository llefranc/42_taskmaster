/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:24:32 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/16 14:22:16 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>

class ConfigParser
{
	public:

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


		/* ----------------------------------------------- */
		/* ---------------- HELPER FUNCS------------------ */

		friend void swap(ConfigParser &a, ConfigParser &b);


	private:

		std::string cfPath_;

};

#endif