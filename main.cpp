#include <iostream>
#include <string>           //for string
#include <filesystem>       //for filesystem
#include <chrono>           //for sleep_for
#include <thread>           //for jthread
#include <vector>           //for vector
#include "directoryIterator.h"

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

//void SyncDirectories( DirectoryIterator& sourceDirectory,  DirectoryIterator& targetDirectory, DirectoryIterator& binDirectory, bool sourceIsMain) { // compare all filenames between two directories, call relevant copying functions-
//    for (const auto& entry : sourceDirectory.GetIterator()) {                                    // depending on if filenames exist.
//                                                                                                 // bool passed to tell if source is main directory, ideally through directory.IsMainDirectory()
//        std::string filename = entry.path().filename().string();   
//
//
//        if (FileIsInDirectory(filename, targetDirectory.GetPath())) {
//
//                std::cout << filename << " exists in target directory.\n";
//        }
//        else {
//            if (sourceIsMain)
//            {
//                std::cout << filename << " does not exist in target directory. Copying...\n";
//                CopyFileTo(entry.path(), targetDirectory);
//            }
//            else
//            {
//                std::cout << filename << " does not exist in main directory. Deleting...\n";
//                MoveFileTo(entry.path(), binDirectory);
//                
//            }
//        }
//    }
//    sourceDirectory.ResetIterator();    // iterators need to be reset or they will only detect the final file
//    targetDirectory.ResetIterator();
//}


struct DirectoryInfo // for making it obvious in the code that a direc is meant to be bin/main/sub
{
    std::string name;
    DirectoryIterator iterator;

    DirectoryInfo(std::string path, std::string name = "Sub")
        : name(name), iterator(path) {}
};


//TODO: Ideally reduce duplicate code without introducing complexity
void SyncDirectories(DirectoryInfo& mainDirectory, std::vector<DirectoryInfo>& subDirectories)
{

        for (auto& subdirectory : subDirectories)                                                        //for each subdirectory in subdirectories vector
        {
            for (const auto& fileInMain : mainDirectory.iterator.GetIterator()) {                        //for each file in the main directory                   

                std::string mainfilename = fileInMain.path().filename().string();                            // depending on if filenames exist.

                if (FileIsInDirectory(mainfilename, subdirectory.iterator.GetPath())) {                      //check if subdirec has the file from main

                    std::cout << mainfilename << " exists in " << subdirectory.iterator.GetPath().filename().string() << "\n";
                }
                else {                                                                                   //copy it if it does not
                    std::cout << mainfilename << " does not exist in " << subdirectory.iterator.GetPath().filename().string() << ", Copying...\n";
                    CopyFileTo(fileInMain.path(), subdirectory.iterator);
                }
            }
            for (const auto& fileInSub : subdirectory.iterator.GetIterator())                           //for each file in subdirectory
            {
                std::string subfilename = fileInSub.path().filename().string();
                if (FileIsInDirectory(subfilename, mainDirectory.iterator.GetPath())) {                 //check if a file in subdirec is in main

                    std::cout << subfilename << " exists in " << mainDirectory.iterator.GetPath().filename().string() << "\n";
                }
                else {                                                                                  //delete it if it does not
                    std::cout << subfilename << " does not exist in " << mainDirectory.iterator.GetPath().filename().string() << ", Deleting...\n";
                    DeleteFile(fileInSub.path());
                }
                
            }

            mainDirectory.iterator.ResetIterator();    // iterators need to be reset after being used or they will only detect the final file
            subdirectory.iterator.ResetIterator();  // iterators need to be reset after being used or they will only detect the final file
        }
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


//void basicloop(DirectoryIterator& sourceDirectory, DirectoryIterator& targetDirectory) //temporary loop
//{
//    while (true) {
//
//        //SyncDirectories(sourceDirectory, targetDirectory, binDirectory, sourceDirectory.IsMainDirectory());
//        //SyncDirectories(targetDirectory, sourceDirectory, binDirectory, targetDirectory.IsMainDirectory());
//
//    }
//}




int main()
{
   // DirectoryInfo binDirectory  {"C:/Users/Luke/source/repos/MiniProject_2_FileSync/BinDirectory",  "Bin",  false };
    DirectoryInfo mainDirectory{ "C:/Users/Luke/source/repos/MiniProject_2_FileSync/MainDirectory", "Main" };

    std::vector<DirectoryInfo> subDirectories =
    {
        {"C:/Users/Luke/source/repos/MiniProject_2_FileSync/SubDirectory1", "Sub"},
        {"C:/Users/Luke/source/repos/MiniProject_2_FileSync/SubDirectory2", "Sub"},
        {"C:/Users/Luke/source/repos/MiniProject_2_FileSync/SubDirectory3", "Sub"},
    };


    SyncDirectories(mainDirectory, subDirectories);




    //basicloop(sourceDirectory, targetDirectory);

    //std::string directoryPath = "C:/Users/Luke/source/repos/MiniProject_2_FileSync/DirectoryA";

    //std::stop_source ssource;
    //std::jthread monitorThread(MonitorDirectory, sourceDirectory.GetPath(), ssource.get_token());
    //std::jthread monitorThread(MonitorDirectory, targetDirectory.GetPath(), ssource.get_token());

    //std::this_thread::sleep_for(std::chrono::seconds(10));
    //ssource.request_stop();

    return 0;
}