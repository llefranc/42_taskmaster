#ifndef SPAWNER_HPP
# define SPAWNER_HPP

#include "ProgramBlock.hpp"
#include "ProcInfo.hpp"
#include "Logger.hpp"

class Spawner
{
        public:
                Spawner();
                ~Spawner();
                Spawner(const Spawner&) = delete;

		Logger *getLogger() const;
		void setLogger(Logger *log);

                void startProgramBlock(ProgramBlock&);
                void startProcess(ProcInfo&, const ProgramBlock&);
                void unSpawn();


        private:
                char **setExecveEnv(const std::vector<std::string> &);
                char **setExecveArg(std::string const &);
                void freeExecveArg(char**, char**, size_t);

        private:
                Logger* logger_;
};


#endif