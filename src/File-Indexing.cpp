// notes:
// stream .good .fail, etc
// throw / catch FileOpenError(), and FileReadError()
// .clear() failbit, once a file stream error happens a flag continues to stay preventing further file streams, use .clear() to continue the stream
#include "/home/dev/projects/projects/ZURA-Lib/File-Operations/src/FileOperations.hpp"
#include <unordered_map>
#include <algorithm>
#include <random>
#include <filesystem>
struct Record
{
    int position;
    std::string data;
};
class fileIndexer : public ZURA::fileOperations
{
private:
    //  std::vector<std::streampos> lineOffsets;
    std::string fileName; // need to initialize with constructor
    std::string dataFileName;
    std::string line;
    // able to do both in and out streams with fstream, might switch this later
    std::fstream userFile; //.txt file
    std::fstream dataFile; //.dat file, just use the same filename

public:
    fileIndexer(std::string filename) : fileName()
    {
        fileName = filename;
        createDataFile(fileName);
        openUserFile();
        openDataFile();
    }
    ~fileIndexer()
    {
        if (userFile.is_open())
        {
            userFile.close();
        }
        if (dataFile.is_open())
        {
            dataFile.close();
        }
    } // user inputs text file
    // openfile fileOperations function
    //
    // must call this first before gathering offsets

    // update this to remove anything after a .
    void createDataFile(std::string filename = "", bool overwrite = false)
    {
        std::string searchText = ".txt";
        size_t found = filename.find(searchText);
        while (found != std::string::npos)
        {
            filename.erase(found, searchText.length());
            found = filename.find(searchText, found);
        }
        dataFileName = filename + ".data";
        if (!std::filesystem::exists(dataFileName) || overwrite == true) // std::filesystem::exists checks if file exists and returns a bool true or false, actually a pretty cool c++ feature.
        {
            ZURA::fileOperations::s_createFile(dataFileName, false); // creates file with .data extention, s_createFile takes care of if file exist and whether to overwrite defualt is false
        }
        else
        {
            std::cout << "File: " << dataFileName << " already exists." << std::endl;
        }
    }
    bool openUserFile()
    {
        if (userFile.is_open())
        {
            userFile.close();
        }
        userFile.open(fileName);
        if (!userFile.is_open())
        {
            std::cout << "ERROR OPENING USERFILE" << std::endl;
            return false;
        }
        return true;
    }
    bool openDataFile()
    {
        if (dataFile.is_open())
        {
            dataFile.close(); // Close the dataFile if it's already open
        }

        dataFile.open(dataFileName); // Open or reopen the data file
        if (!dataFile.is_open())
        {
            std::cout << "Error opening or reopening the data file." << std::endl;
            return false;
        }
        return true;
    }

    // this pushes everything from userFile to indexFile and creates the index
    void pushIndex()
    {
        openDataFile();
        openUserFile();
        std::vector<Record> records;
        std::string line;
        while (std::getline(userFile, line))
        {
            Record temp;
            temp.position = userFile.tellg();
            temp.data = line;
            records.push_back(temp);
        }
        for (Record &tempp : records)
        {
            dataFile.write(reinterpret_cast<const char *>(&tempp), sizeof(Record));
        }
    }
    void altPushIndex()
    {
        openUserFile();
        openDataFile();
        std::vector<Record> records;
        std::string line;
        int i = 0;
        while (std::getline(userFile, line))
        {
            Record temp;
            temp.position = i;
            i++;
            temp.data = line;
            records.push_back(temp);
        }
        for (int i = 0; i < records.size(); i++)
        {
            dataFile << records[i].position << " " << records[i].data << std::endl;
        }
    }
    void altSeek(int pos)
    {
        openDataFile();
        if (!dataFile.is_open())
        {
            std::cout << "not open faggot" << std::endl;
            return;
        }
        Record temp;
        while (!dataFile.eof())
        {
            if (!(dataFile >> temp.position >> temp.data))
            {
                if (!userFile.good())
                {
                    std::cout << "file stream error" << std::endl;
                    return;
                }
                std::cout << "error with reading stream" << std::endl;
            }
            else
            {

                // dataFile >> temp.position >> temp.data;
                if (pos == temp.position)
                {
                    std::cout << temp.position << std::endl;
                    std::cout << temp.data << std::endl;
                    return;
                }
                temp.position = 0;
                temp.data = "";
            }
        }
    }
    void seek(int pos)
    {
        openDataFile();

        int seekPos = pos;
        int recordSize = sizeof(Record);
        int seekSIZE = seekPos * recordSize;
        std::streampos seekOffset = static_cast<std::streampos>(seekSIZE);
        dataFile.seekg(seekOffset);
        Record recordd = {0, ""};

        if (!dataFile.read(reinterpret_cast<char *>(&recordd), sizeof(Record)))
        {
            std::cout << "Error reading the element at the specified position." << std::endl;
        }
        else
        {
            std::cout << "pos: " << recordd.position << std::endl;
            std::cout << "data: " << recordd.data << std::endl;
        }
    }
    static std::string generateRandomString(int length)
    {
        const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> distribution(0, characters.size() - 1);

        std::string randomString;
        for (int i = 0; i < length; i++)
        {
            randomString += characters[distribution(generator)];
        }

        return randomString;
    }
    void stringToFile(std::string text)
    {
        userFile << text << std::endl;
    }
};

int main()
{
    fileIndexer indexfile("testfile.txt");
    // indexfile.altPushIndex();
    indexfile.altSeek(1);
    /*
    for (int i = 0; i < 100000000; i++)
    {
        std::string text = fileIndexer::generateRandomString(10);
        indexfile.stringToFile(text);
    }*/
    return 0;
}
