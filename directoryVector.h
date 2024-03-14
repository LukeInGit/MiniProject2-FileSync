#ifndef DIRECTORYVECTOR_H
#define DIRECTORYVECTOR_H

#include "directoryIterator.h"
#include <vector>  
#include <stdexcept>




struct DirectoryInfo // for making it obvious in the code that a direc is meant to be main or sub
{
    std::string name;
    int directoryID;
    DirectoryIterator iterator;

    DirectoryInfo(std::string path, int directoryID, std::string name = "Sub")
        : iterator{ path }, directoryID{ directoryID }, name{ name } {}
};


//class simply for holding(inc adding/deleting/editing) instances of directoryiterator
class DirectoryVector
{
private:
    std::vector<DirectoryInfo> mainDirectory;
    std::vector<DirectoryInfo> subDirectories;

    DirectoryVector()
    {}
public:
    static DirectoryVector& getInstance() {
        static DirectoryVector instance; // Singleton instance. only allow one of this class to exist
        return instance;
    }

    DirectoryVector(const DirectoryVector&) = delete; // Delete copy constructor.
    DirectoryVector& operator=(const DirectoryVector&) = delete; // Delete assignment operator.
    DirectoryVector(DirectoryVector&&) = delete; // Delete move constructor.
    DirectoryVector& operator=(DirectoryVector&&) = delete; // Delete move assignment operator.

    void SetMainDirectory(std::string path, int directoryID)
    {
        if (mainDirectory.empty())
        {
            mainDirectory.emplace_back(path, directoryID, "Main");
        }
    }
    void AddSubDirectory(std::string path, int directoryID)
    {
        subDirectories.emplace_back(path, directoryID);
            
    }
    const DirectoryInfo& GetDirectory() const
    {

    }

    bool test()
    {
        return 1;
    }

#pragma warning(push)
#pragma warning(disable : 4715) // Disable warning C4715 for this function
     DirectoryInfo& GetMainDirectory() ////cant be const, iterator needs to be reset
    {
        if (!mainDirectory.empty())
        {
            if (!mainDirectory[0].iterator.GetPath().empty())
            {
                return mainDirectory[0];
            }
        }
        else{ throw std::runtime_error("Main directory path is empty"); }
    }
#pragma warning(pop)

     std::vector<DirectoryInfo>& GetSubdirectories() //cant be const, iterator needs to be reset
    {
        return subDirectories;
    }

    void DeleteDirectory(int directoryID)
    {
        subDirectories.erase(subDirectories.begin() + directoryID);
    }
    void EditDirectory()
    {

    }


};


#endif