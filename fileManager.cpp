#include "directoryIterator.h"
#include "fileManager.h"

namespace fManager {
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
        std::filesystem::rename(filepath, targetDirectory.GetPath() / filepath.filename());
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

    struct DirectoryInfo // for making it obvious in the code that a direc is meant to be bin/main/sub
    {
        std::string name;
        int directoryID;
        DirectoryIterator iterator;

        DirectoryInfo(std::string path, int directoryID, std::string name = "Sub")
            : iterator{ path }, directoryID{ directoryID }, name{ name } {}
    };

    void SyncSubDirectory(DirectoryInfo& mainDirectory, DirectoryInfo& subDirectory)
    {
        for (const auto& fileInSub : subDirectory.iterator.GetIterator())                                                                        //for each file in subdirectory
        {
            std::string subfilename = fileInSub.path().filename().string();
            if (FileIsInDirectory(subfilename, mainDirectory.iterator.GetPath())) {                                                              //check if a file in subdirec is in main

                std::cout << subfilename << " exists in " << mainDirectory.iterator.GetPath().filename().string() << "\n";
            }
            else {                                                                                                                               //delete it if it does not
                std::cout << subfilename << " does not exist in " << mainDirectory.iterator.GetPath().filename().string() << ", Deleting...\n";
                DeleteFile(fileInSub.path());
            }
        }
        subDirectory.iterator.ResetIterator();// iterators need to be reset after being used or they will only detect the final file
    }

    void SyncMainDirectory(DirectoryInfo& mainDirectory, DirectoryInfo& subDirectory)
    {
        for (const auto& fileInMain : mainDirectory.iterator.GetIterator()) {                                                                    //for each file in the main directory                   

            std::string mainfilename = fileInMain.path().filename().string();                                                                    // depending on if filenames exist.

            if (FileIsInDirectory(mainfilename, subDirectory.iterator.GetPath())) {                                                              //check if subdirec has the file from main

                std::cout << mainfilename << " exists in " << subDirectory.iterator.GetPath().filename().string() << "\n";
            }
            else {                                                                                                                               //copy it if it does not
                std::cout << mainfilename << " does not exist in " << subDirectory.iterator.GetPath().filename().string() << ", Copying...\n";
                CopyFileTo(fileInMain.path(), subDirectory.iterator);
            }
        }
        mainDirectory.iterator.ResetIterator();// iterators need to be reset after being used or they will only detect the final file
    }

    void SyncAllDirectories(DirectoryInfo& mainDirectory, std::vector<DirectoryInfo>& subDirectories)
    {
        for (auto& subdirectory : subDirectories) // perform the sync actions for every subdirectory and main
        {
            SyncMainDirectory(mainDirectory, subdirectory);
            SyncSubDirectory(mainDirectory, subdirectory);
        }
    }

    std::mutex mtx;
    void MonitorDirectory(DirectoryInfo& direcToMonitor, std::stop_token stoken, DirectoryInfo& mainDirectory, std::vector<DirectoryInfo>& subDirectories) {//monitor a directory and detect changes.

        std::filesystem::file_time_type lastModificationTime = std::filesystem::last_write_time(direcToMonitor.iterator.GetPath());

        std::filesystem::path pathToMonitor = direcToMonitor.iterator.GetPath();

        while (!stoken.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            if (std::filesystem::exists(pathToMonitor))
            {
                std::filesystem::file_time_type currentModificationTime = std::filesystem::last_write_time(pathToMonitor);

                if (currentModificationTime != lastModificationTime)                //if a file has been modified
                {
                    std::cout << pathToMonitor.filename() << " has been modified.\n";
                    // perform actions based on directory changes
                    std::lock_guard<std::mutex> lock(mtx);                          //mutex lock
                    if (direcToMonitor.directoryID == 0)                            //check if the directory that was modified was the main one via the ID
                    {
                        SyncAllDirectories(mainDirectory, subDirectories);          //if it was, call a full scale sync
                    }
                    else
                    {
                        SyncSubDirectory(mainDirectory, subDirectories[direcToMonitor.directoryID - 1]); //-1 because maindirec is 0 and is not included in subDirectories vector 
                        SyncMainDirectory(mainDirectory, subDirectories[direcToMonitor.directoryID - 1]);//^
                    }
                    lastModificationTime = currentModificationTime;                 //reset the check variables
                }
            }
            else
            {
                std::cout << "directory does not exist.\n";                         //oh no
                break;
            }
        }
    }


    int runFM()
    {
        DirectoryInfo mainDirectory{ "C:/Users/Luke/source/repos/MiniProject_2_FileSync/MainDirectory",0, "Main" };

        std::vector<DirectoryInfo> subDirectories =
        {
            {"C:/Users/Luke/source/repos/MiniProject_2_FileSync/SubDirectory1",1, "Sub"},
            {"C:/Users/Luke/source/repos/MiniProject_2_FileSync/SubDirectory2",2, "Sub"},
            {"C:/Users/Luke/source/repos/MiniProject_2_FileSync/SubDirectory3",3, "Sub"},
        };

        std::vector<std::jthread> threads;
        std::stop_source ssource;

        std::jthread mainMonitorThread(MonitorDirectory, std::ref(mainDirectory), ssource.get_token(), std::ref(mainDirectory), std::ref(subDirectories));
        threads.emplace_back(std::move(mainMonitorThread));

        for (DirectoryInfo& subDirectory : subDirectories) {
            std::jthread monitorThread(MonitorDirectory, std::ref(subDirectory), ssource.get_token(), std::ref(mainDirectory), std::ref(subDirectories));
            threads.emplace_back(std::move(monitorThread));
        }

        SyncAllDirectories(mainDirectory, subDirectories);
        std::this_thread::sleep_for(std::chrono::seconds(10));

        ssource.request_stop();


        return 0;


    }

}