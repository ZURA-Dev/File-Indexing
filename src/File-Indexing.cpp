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
#include "/home/dev/Documents/projects/ZURA-Lib/File-Operations/src/FileOperations.cpp"
#include <unordered_map>
struct IndexEntry
{
    std::streampos offset; // offset in the original data file
    std::streamsize size;  // size of the chunk
};
class fileIndexer : public ZURA::fileOperations
{
private:
    std::vector<std::streampos> lineOffsets;
    std::string fileName; // need to initialize with constructor
    std::string line;
    // able to do both in and out streams with fstream, might switch this later
    std::fstream userFile; //.txt file
    std::fstream dataFile; //.dat file, just use the same filenam
public:
    fileIndexer(std::string filename) : fileName()
    {
        fileName = filename;
        createDataFile(fileName);
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
    void createDataFile(std::string filename)
    {

        std::string searchText = ".txt";
        size_t found = filename.find(searchText);
        while (found != std::string::npos)
        {
            filename.erase(found, searchText.length());
            found = filename.find(searchText, found);
        }

        ZURA::fileOperations::s_createFile(filename + ".data", false); // creates file with .data extention, s_createFile takes care of if file exist and whether to overwrite defualt is false
    }
    void openUserFile()
    {
        if (!userFile.is_open())
            userFile.open(fileName);
        else
        {
            std::cout << "Error file already opened / file cant be opened" << std::endl;
        }
    }
    void openDataFile()
    {
        if (!dataFile.is_open())
            dataFile.open(fileName);
        else
        {
            std::cout << "Error file already opened / file cant be opened / file not created" << std::endl;
        }
    }
};

int main()
{
    fileIndexer indexfile("testfile.txt");
    return 0;
}

/*fileIndexer indexFile; // this is the index file, with the offsets
    fileIndexer dataFile;  // this is just user data file without offsets
    ZURA::fileOperations::s_createFile("indexfile.txt", false);
    indexFile.openfile("indexfile.txt", false);*/

// MVP prototype, this reads file stream and grabs its offsets, now i need to print these into a file
// then search a file for a specific thing like a certain line using the offset
/*
std::string filename = "testfile.txt";
std::ifstream inputFile(filename);
std::vector<std::streampos> lineOffsets;
std::string line;
lineOffsets.push_back(inputFile.tellg()); // this will get the initial offset of the beginning of the file, so index 0 should be first line of file
while (std::getline(inputFile, line))
{
    lineOffsets.push_back(inputFile.tellg());
}
inputFile.close();

// printing all offsets
for (std::size_t i = 0; i < lineOffsets.size(); ++i)
{
    std::cout << "LINE: " << i << std::endl;
    std::cout << "OFFSET: " << lineOffsets[i] << std::endl;
    std::cout << std::endl;
}
// Search for a specific line number
int lineNumberToSearch = 3; // Replace with the desired line number
// this only searched THAT line
if (lineNumberToSearch >= 1 && lineNumberToSearch <= lineOffsets.size())
{
    // Seek to the offset of the desired line
    inputFile.open(filename); // Reopen the file
    inputFile.seekg(lineOffsets[lineNumberToSearch - 1]);

    // Read and print the desired line
    std::getline(inputFile, line);
    std::cout << "Line " << lineNumberToSearch << ": " << line << std::endl;
    inputFile.close();
}
else
{
    std::cout << "Invalid line number." << std::endl;
}
*/