#include "bgfxTools.h"

#include <subprocess/subprocess.h>

namespace
{
    long readAll(FILE* file, std::string* content)
    {
        // Get size
        fseek(file, 0, SEEK_END); 
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // If the file has a problem
        if (size <= 0)
            return size;

        // Create buffer to hold data
        char* buffer = new char[size];

        // Extract the bytes and create a new string
        fgets(buffer, size, file);
        *content = std::string(buffer, size);

        // Cleanup buffer to not leak memory
        delete[] buffer;

        // Return the size
        return size;
    }

    bool runProcess(const char* program, const std::vector<std::string>& arguments, std::string* output, std::string* error, bool autonull)
    {
        // Create arguments array
        int size = autonull 
                 ? arguments.size() + 2 
                 : arguments.size() + 1;
        char** args = new char*[size];

        // Fill arguments array
        args[0] = (char*)program;
        for (int i = 0; i < arguments.size(); ++i)
            args[i + 1] = (char*)arguments[i].c_str();
        if (autonull)
            args[size - 1] = NULL;

        // Create process
        struct subprocess_s process;
        int result = subprocess_create(args, subprocess_option_no_window, &process);
        // Clean up args array before checking result
        delete[] args;
        if (result != 0)
            return false;

        // Wait for process to exit
        int process_return;
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
        result = subprocess_destroy(&process);
        if (result != 0)
        {
            subprocess_terminate(&process);
            return false;
        }

        return true;
    }
}

namespace bgfx
{
    namespace tools
    {
        bool shaderc(const std::vector<std::string>& arguments, std::string* output, std::string* error, bool autonull)
        {
            return runProcess("shaderc", arguments, output, error, autonull);
        }
    }
}
