#include "embedExampleShaders.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace
{
    struct Operation
    {
        std::string shaderName;
        std::string shaderWindowsPath;
        std::string shaderMacPath;
        std::string shaderLinuxPath;
        std::string arrayName;
    };

    struct FileSet
    {
        char* windowsData = nullptr;
        char* macData = nullptr;
        char* linuxData = nullptr;
        int windowsSize = 0;
        int macSize = 0;
        int linuxSize = 0;

        void deleteBuffers()
        {
            if (windowsData)
                delete[] windowsData;
            if (macData)
                delete[] macData;
            if (linuxData)
                delete[] linuxData;
        }
    };

    std::string findExamplesPath()
    {
        fs::path current = __FILE__;
        while (current.filename() != "CppSDL3")
        current = current.parent_path();
        current = current.string() + "/vendor/bgfx/bgfx/examples/runtime/shaders";
        return current.string();
    }
    
    std::string examplesPath;

    bool promptOperation(std::vector<Operation>& operations)
    {
        Operation newOperation;

        std::cout << "Enter shader name: ";
        std::cin >> newOperation.shaderName;

        newOperation.shaderWindowsPath = examplesPath + "/dx11/" + newOperation.shaderName;
        newOperation.shaderMacPath = examplesPath + "/metal/" + newOperation.shaderName;
        newOperation.shaderLinuxPath = examplesPath + "/spirv/" + newOperation.shaderName;
        if (!fs::exists(newOperation.shaderWindowsPath))
        {
            std::cout << "ERROR: Unable to find shader path for Windows:\n";
            std::cout << "    '" << newOperation.shaderWindowsPath << "'\n";
            return false;
        }
        if (!fs::exists(newOperation.shaderMacPath))
        {
            std::cout << "ERROR: Unable to find shader path for Mac:\n";
            std::cout << "    '" << newOperation.shaderMacPath << "'\n";
            return false;
        }
        if (!fs::exists(newOperation.shaderLinuxPath))
        {
            std::cout << "ERROR: Unable to find shader path for Linux:\n";
            std::cout << "    '" << newOperation.shaderLinuxPath << "'\n";
            return false;
        }

        std::cout << "Enter array name: ";
        std::cin >> newOperation.arrayName;

        for (auto& op : operations)
        {
            if (op.arrayName == newOperation.arrayName)
            {
                std::cout << "ERROR: Output array name '" << newOperation.arrayName << "' already used.\n";
                return false;
            }
        }

        operations.push_back(newOperation);
        return true;
    }

    bool promptOperations(std::vector<Operation>& operations)
    {
        std::string result = "y";
        while (result == "y" || result == "yes")
        {
            promptOperation(operations);

            if (operations.size() > 0)
            {
                std::cout << "Shaders to embed:\n";
                for (auto& op : operations)
                    std::cout << "    '" << op.shaderName << "' -> '" << op.arrayName << "'\n";
                std::cout << "Add another shader? (y/n): ";
            }
            else
            {
                std::cout << "Try again? (y/n): ";
            }

            std::cin >> result;
            std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c){ return std::tolower(c); });
        }

        return operations.size() > 0;
    }

    bool promptOutputFile(std::string& outputPath)
    {
        std::string result = "y";
        while (result == "y" || result == "yes")
        {
            std::string filename;
            std::cout << "Enter name for output file: ";
            std::cin >> filename;
            std::string path = "./" + filename;

            if (fs::exists(path))
            {
                std::string overwrite = "y";
                std::cout << "File already exists, overwrite it? (y/n): ";
                std::cin >> overwrite;

                std::transform(overwrite.begin(), overwrite.end(), overwrite.begin(),
                    [](unsigned char c){ return std::tolower(c); });

                if (overwrite != "y" && overwrite != "yes")
                {
                    std::string result = "";
                    std::cout << "Try again? (y/n): ";
                    std::cin >> result;

                    std::transform(result.begin(), result.end(), result.begin(),
                        [](unsigned char c){ return std::tolower(c); });
                    continue;
                }
            }

            outputPath = path;
            return true;
        }

        return false;
    }

    bool readFileBytes(const std::string& file, char** destination, int* size)
    {
        if (!fs::exists(file))
            return false;

        std::ifstream input(file, std::ios::binary | std::ios::ate);
        *size = input.tellg();
        if (*size < 0)
            return false;
        *destination = new char[*size];

        input.seekg(0, std::ios::beg);
        if (input.fail())
        {
            delete[] *destination;
            return false;
        }
        input.read(*destination, *size);
        if (input.fail())
        {
            delete[] *destination;
            return false;
        }        

        return true;
    }

    bool createFromOperation(const Operation& operation, FileSet& output)
    {
        bool winResult = readFileBytes(
            operation.shaderWindowsPath, 
            &output.windowsData, 
            &output.windowsSize);
        bool macResult = readFileBytes(
            operation.shaderMacPath, 
            &output.macData, 
            &output.macSize);
        bool linuxResult = readFileBytes(
            operation.shaderLinuxPath, 
            &output.linuxData, 
            &output.linuxSize);

        return winResult && macResult && linuxResult;
    }

    std::string toHex(uint8_t value)
    {
        const std::string HEX = "0123456789abcdef";

        uint8_t low = value & 0x0f;
        uint8_t high = (value & 0xf0) >> 4;

        std::string result = "0x00";
        result[2] = HEX[high];
        result[3] = HEX[low];
        return result;
    }

    bool embedBuffer(std::ofstream& file, const std::string& arrayName, const uint8_t* buffer, const int size)
    {
        file << "    static const int " << arrayName << "Size = " << size << ";\n";
        file << "    static const uint8_t " << arrayName << "[" << size << "] = \n";
        file << "    { ";
        
        std::stringstream ss;
        for (int i = 0; i < size; ++i)
        {
            int column = i % 16;
            if (column == 0)
            {
                std::string comment = ss.str();
                if (comment.size() > 0)
                    file << "// " << comment;
                file << "\n        ";
                ss.str("");
                ss.clear();
            }

            uint8_t value = buffer[i];
            file << toHex(value) << ", ";
            if (value < 33 || value > 126)
                ss << '.';
            else
                ss << (char)value;
        }

        file << "\n";
        file << "    };\n";
        return true;
    }

    bool embedOperations(const std::vector<Operation>& operations, const std::string& outputPath)
    {
        std::ofstream outputFile(outputPath, std::ios::trunc);
        if (!outputFile.is_open())
        {
            std::cout << "ERROR: Failed to open/create output file '" << outputPath << "'\n";
            return false;
        }

        std::vector<FileSet> operationFiles;
        for (auto& op : operations)
        {
            FileSet fileSet;
            bool result = createFromOperation(op, fileSet);
            if (!result)
            {
                std::cout << "ERROR: Failed to create file set for '" << op.shaderName << "'\n";
                for (auto& set : operationFiles)
                    set.deleteBuffers();
                fileSet.deleteBuffers();
                return false;
            }
            operationFiles.push_back(fileSet);
        }

        outputFile << "namespace\n";
        outputFile << "{\n";
        outputFile << "#ifdef OS_WINDOWS\n";
        for (int i = 0; i < operations.size(); ++i)
            embedBuffer(
                outputFile, 
                operations.at(i).arrayName,
                (uint8_t*)operationFiles.at(i).windowsData,
                operationFiles.at(i).windowsSize);
        outputFile << "#elif OS_MAC\n";
        for (int i = 0; i < operations.size(); ++i)
            embedBuffer(
                outputFile, 
                operations.at(i).arrayName,
                (uint8_t*)operationFiles.at(i).macData,
                operationFiles.at(i).macSize);
        outputFile << "#elif OS_LINUX\n";
        for (int i = 0; i < operations.size(); ++i)
            embedBuffer(
                outputFile, 
                operations.at(i).arrayName,
                (uint8_t*)operationFiles.at(i).linuxData,
                operationFiles.at(i).linuxSize);
        outputFile << "#endif\n";
        outputFile << "}\n";

        for (auto& set : operationFiles)
            set.deleteBuffers();

        return true;
    }
}

int embedExampleShaders()
{
    examplesPath = findExamplesPath();
    if (!fs::exists(examplesPath))
    {
        std::cout << "ERROR: Unable to find example shader path:\n";
        std::cout << "    '" << examplesPath << "'\n";
        return EXIT_SUCCESS;
    }

    std::vector<Operation> operations;
    bool result = promptOperations(operations);
    if (!result)
    {
        std::cout << "No operations, quitting program...\n";
        return EXIT_SUCCESS;
    }

    std::string outputPath = "";
    result = promptOutputFile(outputPath);
    if (!result)
    {
        std::cout << "No output file, quitting program...\n";
        return EXIT_SUCCESS;
    }

    result = embedOperations(operations, outputPath);
    if (!result)
    {
        std::cout << "ERROR: embed failed.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

