// this will create indexed files.
// index files are files where you can look for records in a text file without reprocessing the entire file each time you want to find a certain record
// i will use marshalling, offsets, and indexes

// goal for now: create a indexer where i can skip to points in the file with offsets

// notes:
// tellg() returns offset pointer position from file stream in file

// todo
// read user file 1 by 1, store into index file with offset : user-data
// ok so read user file and find each offset, then paste that information directly into data file..

// reimplement my main MVP into a more reuseable form

// !! what i need to do is create each file using my fileoperations object, it will be easier to maintain i think.
// this is because i can just use fileropations close and open files and file stream checkers
// i also want to learn how pointers and file streams work, and maybe recreate from scratch
// create support for json files, or create anohter library for indexing and reading json files bc json already has a prebuilt indexers ig
#include "/home/dev/projects/projects/ZURA-Lib/File-Operations/src/FileOperations.hpp"
#include <unordered_map>
#include <algorithm>
#include <random>
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
    void createDataFile(std::string filename = "")
    {
        std::string searchText = ".txt";
        size_t found = filename.find(searchText);
        while (found != std::string::npos)
        {
            filename.erase(found, searchText.length());
            found = filename.find(searchText, found);
        }
        ZURA::fileOperations::s_createFile(filename + ".data", false); // creates file with .data extention, s_createFile takes care of if file exist and whether to overwrite defualt is false
        dataFileName = filename + ".data";
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
        openUserFile();
        openDataFile();
        Record records;
        int i = 0;
        Record temp;
        while (!dataFile.eof())
        {
            dataFile >> temp.position >> temp.data;
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
    indexfile.altSeek(17286491);
    /*
    for (int i = 0; i < 100000000; i++)
    {
        std::string text = fileIndexer::generateRandomString(10);
        indexfile.stringToFile(text);
    }*/
    std::cout;
    return 0;
}
