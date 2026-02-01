#include "bgfxTools.h"

#include <subprocess/subprocess.h>

#include <sstream>
#include <iostream>

namespace
{
    std::string constructPath(const char* program)
    {
    	std::stringstream ss;
    	ss << "./" << program;
    	return ss.str();
    }

    long readAll(FILE* file, std::string* content)
    {
        char buffer[1024];
        std::stringstream ss;
        while (fgets(buffer, sizeof(buffer), file) != nullptr)
            ss << buffer;
        *content = ss.str();
        return content->size();
    }

    bool runProcess(const char* program, const std::vector<std::string>& arguments, std::string* output, std::string* error)
    {
    	std::string programPath = constructPath(program);
    
        // Create arguments array
        int size = arguments.size() + 2;
        char** args = new char*[size];

        // Fill arguments array
        args[0] = (char*)programPath.c_str();
        for (int i = 0; i < arguments.size(); ++i)
            args[i + 1] = (char*)arguments[i].c_str();
        args[size - 1] = NULL;

        // Create process
        struct subprocess_s process;
        int result = subprocess_create(args, 0, &process);
        // Clean up args array before checking result
        delete[] args;
        if (result != 0)
            return false;

        // Wait for process to exit
        int process_return = 0;
        result = subprocess_join(&process, &process_return);
        if (result != 0)
        {
            subprocess_terminate(&process);
            return false;
        }

        // Get process output if desired
        FILE* processOutput = subprocess_stdout(&process);
        FILE* processError = subprocess_stderr(&process);
        if (output)
            readAll(processOutput, output);
        if (error)
            readAll(processError, error);

        // Destroy process
        subprocess_destroy(&process);

        return true;
    }
}

namespace bgfx
{
    namespace tools
    {
        bool shaderc(const std::vector<std::string>& arguments, std::string* output, std::string* error)
        {
            return runProcess("shaderc", arguments, output, error);
        }

        bool geometryc(const std::vector<std::string>& arguments, std::string* output, std::string* error)
        {
            return runProcess("geometryc", arguments, output, error);
        }

        bool texturec(const std::vector<std::string>& arguments, std::string* output, std::string* error)
        {
            return runProcess("texturec", arguments, output, error);
        }
    }
}
