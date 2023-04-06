/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 13:24:32 by llefranc          #+#    #+#             */
/*   Updated: 2023/04/06 14:07:14 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>
#include <list>
#include <map>

#include "ProgramBlock.hpp"

class ConfigParser
{
	public:

		/* ----------------------------------------------- */
		/* ---------------- COPLIEN FORM ----------------- */

		ConfigParser() = delete;
		ConfigParser(char **env);
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
		std::list<ProgramBlock> reload();


		/* ----------------------------------------------- */
		/* ---------------- HELPER FUNCS------------------ */

		friend void swap(ConfigParser &a, ConfigParser &b);


		/* ----------------------------------------------- */
		/* --------------- PRIVATE METHODS --------------- */

	private:

		void checkFileOpening(std::ifstream *ifs,
				const std::string& path);
		void parseProgramName(const std::list<ProgramBlock> &pbList,
				ProgramBlock *pb, const std::string &line);
		void parseToken(ProgramBlock *pb, const std::string &line);
		void parseCmd(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void parseNumProcs(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void parseUmask(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void parseWorkingDir(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void parseAutoStart(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void parseAutoRestart(ProgramBlock *pb,
				const std::string &token,
				const std::string &param);
		void parseStartRetries(ProgramBlock *pb,
				const std::string &token,
				const std::string &param);
		void parseStartTime(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void parseExitCodes(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void parseStopSignal(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void parseStopTime(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void parseStdout(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void parseStderr(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void parseEnv(ProgramBlock *pb, const std::string &token,
				const std::string &param);
		void savePb(std::list<ProgramBlock> *pbList,
			ProgramBlock *pb, bool *inProgramBlock);
		void generateProcInfos(std::list<ProgramBlock> *pbList);
		bool isDigitStr(const std::string &str);
		bool isOctalStr(const std::string &str);
		std::vector<std::string> splitStr(const std::string &param,
				char c);
		int checkEnvKey(const std::string &varKey);
		int checkEnvVal(const std::string &varVal);
		std::pair<std::string, std::string> splitEnvVar(
				const std::string &var);
		std::vector<std::string> splitEnv(const std::string &param);


		/* ----------------------------------------------- */
		/* ----------------- ATTRIBUTES ------------------ */

	private:

		typedef void (ConfigParser::*methodPtr)(ProgramBlock *,
				const std::string &, const std::string &);

		/* Configuration file path. */
		std::string cfPath_;

		/*
		 * vecParentEnv_ and mapParentEnv_ are used to populate the
		 * ProgramBlocks environment with parent environment.
		 */
		std::vector<std::string> vecParentEnv_;
		std::map<std::string, std::string> mapParentEnv_;

		/* Parse methods associated to config file tokens. */
		const std::pair<std::string, methodPtr> tokMeths_[14] = {
			std::make_pair<std::string, methodPtr>("cmd=",
					&ConfigParser::parseCmd),
			std::make_pair<std::string, methodPtr>("numprocs=",
					&ConfigParser::parseNumProcs),
			std::make_pair<std::string, methodPtr>("workingdir=",
					&ConfigParser::parseWorkingDir),
			std::make_pair<std::string, methodPtr>("umask=",
					&ConfigParser::parseUmask),
			std::make_pair<std::string, methodPtr>("autostart=",
					&ConfigParser::parseAutoStart),
			std::make_pair<std::string, methodPtr>("autorestart=",
					&ConfigParser::parseAutoRestart),
			std::make_pair<std::string, methodPtr>("startretries=",
					&ConfigParser::parseStartRetries),
			std::make_pair<std::string, methodPtr>("starttime=",
					&ConfigParser::parseStartTime),
			std::make_pair<std::string, methodPtr>("exitcodes=",
					&ConfigParser::parseExitCodes),
			std::make_pair<std::string, methodPtr>("stopsignal=",
					&ConfigParser::parseStopSignal),
			std::make_pair<std::string, methodPtr>("stoptime=",
					&ConfigParser::parseStopTime),
			std::make_pair<std::string, methodPtr>("stdout=",
					&ConfigParser::parseStdout),
			std::make_pair<std::string, methodPtr>("stderr=",
					&ConfigParser::parseStderr),
			std::make_pair<std::string, methodPtr>("env=",
					&ConfigParser::parseEnv)
		};
};

#endif