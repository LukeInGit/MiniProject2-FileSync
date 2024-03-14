#ifndef FILEMANAGER_H
#define FILEMANAGER_H


//std
#include <iostream>
#include <string>           //for string
#include <filesystem>       //for filesystem
#include <chrono>           //for sleep_for
#include <thread>           //for jthread
#include <vector>           //for vector
#include <mutex>            //for lock_guard

#include "directoryIterator.h"
#include "directoryVector.h"

namespace fManager {

	bool FileIsInDirectory(const std::string& filename, const std::filesystem::path& directory);         // check if filename is in target directory


	void CopyFileTo(const std::filesystem::path& filepath, DirectoryIterator& targetDirectory);


	void MoveFileTo(const std::filesystem::path& filepath, DirectoryIterator& targetDirectory);


	void DeleteFile(const std::filesystem::path& filepath);

	void DeleteAllFilesInDirec(DirectoryIterator& binDirectory);


	//struct DirectoryInfo;

	void SyncSubDirectory(DirectoryInfo& mainDirectory, DirectoryInfo& subDirectory);

	void SyncMainDirectory(DirectoryInfo& mainDirectory, DirectoryInfo& subDirectory);

	void SyncAllDirectories(DirectoryInfo& mainDirectory, std::vector<DirectoryInfo>& subDirectories);


	void MonitorDirectory(DirectoryInfo& direcToMonitor, std::stop_token stoken, DirectoryInfo& mainDirectory, std::vector<DirectoryInfo>& subDirectories);


	int runFM();

}
#endif // !FILEMANAGER_H