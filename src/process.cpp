#include "process.h"

#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include <boost/log/trivial.hpp>

#include "stdiohandler.h"

std::string exec(const char* cmd)
{
    char buffer[128];
    std::string result;

    FILE* pipe = popen(cmd, "r");
    if (!pipe)
    {
        return "popen failed!";
    }

    while (!feof(pipe))
    {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }

    pclose(pipe);
    return result;
}

int createChild(std::vector<std::string> arguments)
{
    int pid = fork();
    if (pid == 0)
    {
        int length = (int)arguments.size();
        const char** argv = new const char*[length + 1];
        for (int j = 0; j < length; ++j)
            argv[j] = arguments[j].c_str();
        argv[length] = NULL;
        StdErrHandler err([](const char* line)
                          { BOOST_LOG_TRIVIAL(error) << line; });
        StdOutHandler out([](const char* line)
                          {
                              if (std::string(line).find("warn") != std::string::npos)
                              {
                                  BOOST_LOG_TRIVIAL(warning) << line;
                              }
                              else
                              {
                                  if (std::string(line).find("Players:**") == std::string::npos)
                                  {
                                      BOOST_LOG_TRIVIAL(info) << line;
                                  }
                              } });
        execv(arguments[0].c_str(), (char**)argv);

        BOOST_LOG_TRIVIAL(error) << "Exec error: " << errno << ", " << strerror(errno);

        exit(1);
    }
    else if (pid > 0)
    {
        signal(SIGCHLD, SIG_IGN);
        return pid;
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Fork error: " << errno << ", " << strerror(errno);
        return -1;
    }
}
