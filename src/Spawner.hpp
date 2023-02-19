#ifndef SPAWNER_HPP
# define SPAWNER_HPP

#include "ProgramBlock.hpp"
#include "ProcInfo.hpp"
#include "Logger.hpp"

class Spawner
{
        public:
                Spawner(Logger&);
                ~Spawner();
                Spawner(const Spawner&) = delete;

                void startProgramBlock(ProgramBlock&);
                void startProcess(ProcInfo&, const ProgramBlock&);
                void unSpawn();
        
        private:
                char **setExecveEnv(const std::vector<std::string> &);
                void freeExecveArg(char**, char**, size_t);
        
        private:
                Logger& logger_;
};


#endif