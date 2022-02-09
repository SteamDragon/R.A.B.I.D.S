#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

class StdioHandler
{
private:
    pid_t pid = 0;
    int origfd;
    int streamid;
    int pipefd[2];

public:
    enum class Stream
    {
        stdout = STDOUT_FILENO,
        stderr = STDERR_FILENO
    };
    StdioHandler(Stream stream, std::function<void(const char *)> callback)
        : streamid(static_cast<int>(stream))
    {
        origfd = dup(streamid);

        pipe(pipefd); // create pipe
        pid = fork(); // spawn a child process to handle output of pipe
        if (pid == 0)
        {
            char line[256];
            FILE *output;

            close(pipefd[1]);
            output = fdopen(pipefd[0], "r");
            if (output)
            {
                while (fgets(line, sizeof(line), output))
                {

                    int n = strlen(line);
                    if (n > 0)
                        if (line[n - 1] == '\n')
                            line[n - 1] = 0;
                    callback(line);
                }
                fclose(output);
            }
            abort();
        }
        else
        {
            // connect input of pipe to
            close(pipefd[0]);
            dup2(pipefd[1], streamid);
        }
    }

    ~StdioHandler()
    {
        int status;

        usleep(10000);

        close(pipefd[1]);
        kill(pid, SIGINT);

        waitpid(pid, &status, 0);

        dup2(origfd, streamid);
    }
};

class StdOutHandler : public StdioHandler
{
public:
    StdOutHandler(std::function<void(const char *)> callback) : StdioHandler(Stream::stdout, callback)
    {
    }
};

class StdErrHandler : public StdioHandler
{
public:
    StdErrHandler(std::function<void(const char *)> callback) : StdioHandler(Stream::stderr, callback)
    {
    }
};