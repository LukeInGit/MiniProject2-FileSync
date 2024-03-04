#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <vector>


class DirectoryIterator
{
private:
    std::filesystem::directory_iterator m_iterator{};
    std::filesystem::path m_path{};
    bool m_isMainDirectory{};

    //DirectoryIterator() = delete;                                           // prevent default constructer usage
    //DirectoryIterator(const DirectoryIterator&) = delete;                   // delete copy constructor
    //DirectoryIterator& operator=(const DirectoryIterator&) = delete;        // delete assignment constructor
public:

     DirectoryIterator(std::string filepath, bool isMain=false)    
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




bool FileIsInDirectory(const std::string& filename, const std::filesystem::path& directory)         // check if filename is in target directory
{
    std::filesystem::path targetFilePath = directory / filename;
    return std::filesystem::exists(targetFilePath);
}



void CopyFileTo(const std::filesystem::path& filepath, DirectoryIterator& targetDirectory)
{
    std::filesystem::copy(filepath, targetDirectory.GetPath());
}

void MoveFileTo(const std::filesystem::path& filepath, DirectoryIterator& targetDirectory)
{
    std::filesystem::rename(filepath, targetDirectory.GetPath()/filepath.filename());
}

void DeleteFile(const std::filesystem::path& filepath)
{
    std::filesystem::remove(filepath);
}

void DeleteAllFilesInDirec(DirectoryIterator& binDirectory)
{
    for (const auto& entry : binDirectory.GetIterator())
    {
        if (std::filesystem::is_regular_file(entry))
        {
            DeleteFile(entry.path());
        }
    }
}

void SyncDirectories( DirectoryIterator& sourceDirectory,  DirectoryIterator& targetDirectory, DirectoryIterator& binDirectory, bool sourceIsMain) { // compare all filenames between two directories, call relevant copying functions-
    for (const auto& entry : sourceDirectory.GetIterator()) {                                    // depending on if filenames exist.
                                                                                                 // bool passed to tell if source is main directory, ideally through directory.IsMainDirectory()
        std::string filename = entry.path().filename().string();   


        if (FileIsInDirectory(filename, targetDirectory.GetPath())) {

                std::cout << filename << " exists in target directory.\n";
        }
        else {
            if (sourceIsMain)
            {
                std::cout << filename << " does not exist in target directory. Copying...\n";
                CopyFileTo(entry.path(), targetDirectory);
            }
            else
            {
                std::cout << filename << " does not exist in main directory. Deleting...\n";
                MoveFileTo(entry.path(), binDirectory);
                
            }
        }
    }
    sourceDirectory.ResetIterator();    // iterators need to be reset or they will only detect the final file
    targetDirectory.ResetIterator();
}


void MonitorDirectory(const std::filesystem::path& pathToMonitor, std::stop_token stoken) {                                 //monitor directory and detect changes.
    std::filesystem::file_time_type lastModificationTime = std::filesystem::last_write_time(pathToMonitor);

    while (!stoken.stop_requested()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (std::filesystem::exists(pathToMonitor)) 
        {
            std::filesystem::file_time_type currentModificationTime = std::filesystem::last_write_time(pathToMonitor);
            if (currentModificationTime != lastModificationTime)
            {
                std::cout << "directory has been modified.\n";
                // perform actions based on directory changes
                lastModificationTime = currentModificationTime;
            }
        }
        else 
        {
            std::cout << "directory does not exist.\n";
            break;
        }
    }
}

namespace DirecType {
    enum DirectoryType {
        Bin =0,
        Main=1,
        Sub1=2, 
        MaxDirectories 
    };
}
//void basicloop(DirectoryIterator& sourceDirectory, DirectoryIterator& targetDirectory) //temporary loop
//{
//    while (true) {
//
//        //SyncDirectories(sourceDirectory, targetDirectory, binDirectory, sourceDirectory.IsMainDirectory());
//        //SyncDirectories(targetDirectory, sourceDirectory, binDirectory, targetDirectory.IsMainDirectory());
//
//    }
//}

struct DirectoryInfo // for making it obvious in the code that a direc is meant to be bin/main/sub
{
    std::string name;
    DirectoryIterator iterator;

    DirectoryInfo(std::string path,  std::string name = "Sub-Directory", bool isMain = false)
        :  name(name), iterator(path, isMain) {}
};


int main()
{
    std::vector<DirectoryInfo> directories=
    {
        {"C:/Users/Luke/source/repos/MiniProject_2_FileSync/BinDirectory",      "Bin",      false   },
        {"C:/Users/Luke/source/repos/MiniProject_2_FileSync/MainDirectory",     "Main",     true    },
        {"C:/Users/Luke/source/repos/MiniProject_2_FileSync/SubDirectory1",     "Sub"               },
    };


   // DirectoryInfo test{ "C:/Users/Luke/source/repos/MiniProject_2_FileSync/BinDirectory",      "Bin",      false };

    //SyncDirectories(mainDirectory, subDirectory, binDirectory, mainDirectory.IsMainDirectory());
    //SyncDirectories(subDirectory, mainDirectory, binDirectory, subDirectory.IsMainDirectory());


    SyncDirectories(
        directories[DirecType::Main].iterator,
        directories[DirecType::Sub1].iterator,
        directories[DirecType::Bin].iterator,
        directories[DirecType::Main].iterator.IsMainDirectory()
    );

    SyncDirectories(
        directories[DirecType::Sub1].iterator,
        directories[DirecType::Main].iterator,
        directories[DirecType::Bin].iterator,
        directories[DirecType::Sub1].iterator.IsMainDirectory()
    );


    std::cout << directories[DirecType::Bin].name;
    std::cout << directories[DirecType::Main].name;
    std::cout << directories[DirecType::Sub1].name;
    //basicloop(sourceDirectory, targetDirectory);

    //std::string directoryPath = "C:/Users/Luke/source/repos/MiniProject_2_FileSync/DirectoryA";

    //std::stop_source ssource;
    //std::jthread monitorThread(MonitorDirectory, sourceDirectory.GetPath(), ssource.get_token());
    //std::jthread monitorThread(MonitorDirectory, targetDirectory.GetPath(), ssource.get_token());

    //std::this_thread::sleep_for(std::chrono::seconds(10));
    //ssource.request_stop();

    return 0;
}