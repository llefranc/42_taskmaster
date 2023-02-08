/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramBlock.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: llefranc <llefranc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 14:54:11 by llefranc          #+#    #+#             */
/*   Updated: 2023/02/08 15:13:08 by llefranc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROGRAM_BLOCK_HPP
#define PROGRAM_BLOCK_HPP

#include <string>
#include <vector>


class ProgramBlock
{
	public:

		enum {
			STATE_STOP,
			STATE_START,
			STATE_NEW,
			STATE_UPDATED,
			STATE_NEED_RM,
		};

		enum {
			AUTO_FALSE,
			AUTO_TRUE,
			AUTO_UNEXP,
		};

		ProgramBlock();
		ProgramBlock(const ProgramBlock& copyObj);
		~ProgramBlock();
		ProgramBlock& operator=(const ProgramBlock assignObj);
		// operator==

	private:

		int state_;
		std::vector<std::pair<int, long> > procs_; // pair de pids + time
		std::string name_;
		std::string cmd_;
		int numprocs_;
		std::string umask_;
		std::string workDir_;
		bool autoStart_;
		int autoRestart_;
		int startRetries_;
		int startTime_;
		std::vector<int> exitCodes_;
		int stopSignal_;
		std::string logOut_;
		std::string logErr_;
		char** env_;

};

#endif