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
        std::filesystem::path destination = targetDirectory.GetPath() / filepath.filename();

        try {
            if (std::filesystem::is_regular_file(filepath)) {
                //copy file to target directory
                std::filesystem::copy(filepath, destination);
            }
            else if (std::filesystem::is_directory(filepath)) {
                //create the destination directory if it doesn't exist
                if (!std::filesystem::exists(destination)) {
                    std::filesystem::create_directory(destination);
                }
                //recursively copy directory contents
                for (const auto& entry : std::filesystem::recursive_directory_iterator(filepath)) {
                    std::filesystem::copy(entry.path(), destination / entry.path().filename());
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e) {
            //handle filesystem error
            std::cerr << "filesystem error: " << e.what() << '\n';
        }
    }
    

    void MoveFileTo(const std::filesystem::path& filepath, DirectoryIterator& targetDirectory)
    {
        std::filesystem::rename(filepath, targetDirectory.GetPath() / filepath.filename());
    }

    void DeleteFile(const std::filesystem::path& filepath)
    {
        try {
            std::filesystem::remove_all(filepath);
        }
        catch (const std::filesystem::filesystem_error& e) {
            //handle filesystem error
            std::cerr << "filesystem error: " << e.what() << "\n";
        }
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
    
    void SyncSubDirectory(DirectoryInfo& mainDirectory, DirectoryInfo& subDirectory)
    {
        subDirectory.iterator.ResetIterator();// iterators need to be reset before use here or it may fail to detect new or changed files
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
        subDirectory.iterator.ResetIterator();// iterators also need to be reset after being used or they will only detect the final file
    }

    void SyncMainDirectory(DirectoryInfo& mainDirectory, DirectoryInfo& subDirectory)
    {
        mainDirectory.iterator.ResetIterator();// iterators need to be reset before use here or it may fail to detect new or changed files
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
        mainDirectory.iterator.ResetIterator();// iterators also to be reset after being used or they will only detect the final file
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

        std::cout << "FM: initialising MonitorDirectory\n";

        std::filesystem::file_time_type lastModificationTime = std::filesystem::last_write_time(direcToMonitor.iterator.GetPath());

        std::filesystem::path pathToMonitor = direcToMonitor.iterator.GetPath();

        while (!stoken.stop_requested()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "FM: Monitor Directory is active\n";
            if (std::filesystem::exists(pathToMonitor))
            {
                std::filesystem::file_time_type currentModificationTime = std::filesystem::last_write_time(pathToMonitor);

                if (currentModificationTime != lastModificationTime)                //if a file has been modified
                {
                    std::cout << pathToMonitor.filename() << " has been modified.\n";
                    std::cout << "id is: " <<direcToMonitor.directoryID << '\n';
                    // perform actions based on directory changes
                    std::lock_guard<std::mutex> lock(mtx);                          //mutex lock
                    if (direcToMonitor.name == "Main")                            //check if the directory that was modified was the main one via the name
                    {
                        std::cout << "FM: name was Main, running sync all\n";
                        SyncAllDirectories(mainDirectory, subDirectories);          //if it was, call a full scale sync
                    }
                    else
                    {

                        std::cout << "FM: name was not Main, running sync sub then sync main\n";
                        SyncSubDirectory(mainDirectory, subDirectories[direcToMonitor.directoryID]); //originally -1 because maindirec and subs were in the same array where main was always 0 
                        SyncMainDirectory(mainDirectory, subDirectories[direcToMonitor.directoryID]);

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
        std::cout << "FM: MonitorDirectory complete\n";
    }



    int runFM(std::stop_token stopToken)
    {

        DirectoryVector& directoryVector = DirectoryVector::getInstance(); //singleton of directoryvector, one call in fileManager.cpp and one in imguiGUI.cpp

        bool hasStarted{ false };//make sure functionality only runs once
        bool hasRequestedStop{ false };//make sure functionality only runs once

        std::stop_source ssource;
        std::vector<std::jthread> threads;

        {//try make gui and filemanager run as close to the same time as possible
            std::lock_guard<std::mutex> lck(mtx);
            ready = true;
            cv.notify_one();
        }
        while (!directoryVector.isExiting() && !stopToken.stop_requested())// && directoryVector.isGuiAlive())
        {
            //directoryVector.guiCheck();
            if (directoryVector.isSyncing())
            {   
                if (!hasStarted)
                {
                    std::cout << "FM: directoryvector is syncing\n";
                    if (ssource.stop_requested()) { std::cout << "FM stop requested"; };
                    hasStarted = true;

                    DirectoryInfo& mainDirectory{ directoryVector.GetMainDirectory() };
                    std::vector<DirectoryInfo>& subDirectories{ directoryVector.GetSubdirectories() };


                    ssource = std::stop_source(); // assign new stop source
                    std::stop_token monitorDirecStoken= ssource.get_token(); //create new token every time this condition is ran

                    std::jthread mainMonitorThread(MonitorDirectory, std::ref(mainDirectory), monitorDirecStoken, std::ref(mainDirectory), std::ref(subDirectories));
                    threads.emplace_back(std::move(mainMonitorThread));

                    for (DirectoryInfo& subDirectory : subDirectories) {
                        std::jthread monitorThread(MonitorDirectory, std::ref(subDirectory), monitorDirecStoken, std::ref(mainDirectory), std::ref(subDirectories));
                        threads.emplace_back(std::move(monitorThread));
                    }

                    SyncAllDirectories(mainDirectory, subDirectories);
                    hasRequestedStop = false;
                }
            }
            else
            {
                
                if (!hasRequestedStop)
                {
                    std::cout << "FM: directoryvector is not syncing\n";
                    ssource.request_stop(); //request stop for all stop tokens in ssource 

                    // Wait for all threads to finish, jthreads automatically join when destroyed but just in case
                    for (auto& thread : threads)
                    {
                        if (thread.joinable()) 
                        {
                            thread.join();
                        }
                    }
                        hasStarted = false;
                        hasRequestedStop = true;
                }
            }
        }
        ssource.request_stop();//just in case it didnt already call for whatever reason

        return 0;


    }

}