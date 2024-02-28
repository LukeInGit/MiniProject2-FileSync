#include <iostream>
#include <filesystem>

class DirectoryIterator
{
private:
    std::filesystem::directory_iterator m_iterator{};
    std::filesystem::path m_path{};
    bool m_isMainDirectory{};

    DirectoryIterator() = delete;                                           // prevent default constructer usage
    DirectoryIterator(const DirectoryIterator&) = delete;                   // delete copy constructor
    DirectoryIterator& operator=(const DirectoryIterator&) = delete;        // delete assignment constructor
public:

    explicit DirectoryIterator(std::string filepath, bool isMain=false)    
        :m_iterator{ filepath }, m_path{ filepath }, m_isMainDirectory{isMain}
    {}

    void ResetIterator()
    {
        m_iterator = std::filesystem::directory_iterator{ m_path };
    }

    const std::filesystem::directory_iterator& GetIterator() const
    {
        return m_iterator;
    }

    const std::filesystem::path& GetPath() const
    {
        return m_path;
    }

    bool IsMainDirectory()
    {
        return m_isMainDirectory;
    }


    void CheckDirectory()      // print out filenames in directory
    {

        for (const auto& entry : m_iterator)
        {
            if (std::filesystem::is_regular_file(entry))
            {
                std::cout << entry.path().filename() << '\n';
            }
        }
        ResetIterator();
    }



};

bool IsFileInDirectory(const std::string& filename, const std::filesystem::path& directory)         // check if filename is in target directory
{
    std::filesystem::path targetFilePath = directory / filename;
    return std::filesystem::exists(targetFilePath);
}

void SyncDirectories( DirectoryIterator& sourceDirectory,  DirectoryIterator& targetDirectory) { // compare all filenames between two directories, call relevant copying functions-
    for (const auto& entry : sourceDirectory.GetIterator()) {                                    // depending on if filenames exist.
                                                                                                 // different usage if directory is main directory - not ideal - may change
        std::string filename = entry.path().filename().string();   


        if (IsFileInDirectory(filename, targetDirectory.GetPath())) {

                std::cout << filename << " exists in target directory.\n";
        }
        else {
            if (sourceDirectory.IsMainDirectory())
            {
                std::cout << filename << " does not exist in target directory. Copying...\n";
            }
            else
            {
                std::cout << filename << " does not exist in main directory. Deleting...\n";
            }
        }
    }
    sourceDirectory.ResetIterator();    // iterators need to be reset or they will only detect the final file
    targetDirectory.ResetIterator();
}


void basicloop(DirectoryIterator& sourceDirectory, DirectoryIterator& targetDirectory) //temporary loop
{
    while (true) {
        SyncDirectories(sourceDirectory, targetDirectory);
        SyncDirectories(targetDirectory, sourceDirectory);

    }
}

int main()
{

    DirectoryIterator sourceDirectory{ "C:/Users/Luke/source/repos/MiniProject_2_FileSync/DirectoryA",true };
    DirectoryIterator targetDirectory{ "C:/Users/Luke/source/repos/MiniProject_2_FileSync/DirectoryB"};
    DirectoryIterator binDirectory{ "C:/Users/Luke/source/repos/MiniProject_2_FileSync/DirectoryBin"};

    SyncDirectories(sourceDirectory, targetDirectory);
    SyncDirectories(targetDirectory, sourceDirectory);

    //basicloop(sourceDirectory, targetDirectory);


    return 0;
}