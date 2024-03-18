#ifndef DIRECTORYVECTOR_H
#define DIRECTORYVECTOR_H

//poorly named for a communication header, may change

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


//class simply for filemanager/gui communication
class DirectoryVector
{
private:
    std::vector<DirectoryInfo> m_mainDirectory;
    std::vector<DirectoryInfo> m_subDirectories;
    bool m_isSyncing{ false };
    bool m_isExiting{ false };
    int m_guiHP{ 500 }; //heartbeat 

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
        if (m_mainDirectory.empty())
        {
            m_mainDirectory.emplace_back(path, directoryID, "Main");
        }
        else
        {
            m_mainDirectory[0].iterator.ChangePath(path);
        }
    }
    bool AddSubDirectory(std::string path, int directoryID)
    {
        if (FindDirectory(directoryID)==-1)
        {
            m_subDirectories.emplace_back(path, directoryID);
            return true;
        }
        return false;
    }

#pragma warning(push)
#pragma warning(disable : 4715) // Disable warning C4715 for this function
     DirectoryInfo& GetMainDirectory() ////cant be const, iterator needs to be reset
    {
        if (!m_mainDirectory.empty())
        {
            if (!m_mainDirectory[0].iterator.GetPath().empty())
            {
                return m_mainDirectory[0];
            }
        }
        else{ throw std::runtime_error("Main directory path is empty"); }
    }
#pragma warning(pop)

     bool isMainSet()
     {
         return !m_mainDirectory.empty();
     }

     std::vector<DirectoryInfo>& GetSubdirectories() //cant be const, iterator needs to be reset
    {
        return m_subDirectories;
    }

    void DeleteDirectory(int directoryID)
    {
        int index = FindDirectory(directoryID);
        if (index == -1)
        {
            std::cerr << "Cannot delete, No directory with that ID was found\n";
        }
        else
        {
            m_subDirectories.erase(m_subDirectories.begin() + index);
            std::cout << "subdirec deleted\n";
        }
    }
    void EditMainDirectory()
    {
        //add later
    }
    void EditSubDirectory(std::string path, int directoryID)
    {
        int index = FindDirectory(directoryID);
        m_subDirectories[index].iterator.ChangePath(path);
    }
    void PrintSubdirectories()
    {
        int pos{ 0 };
        for (const auto& thing : m_subDirectories)
        {
            std::cout << "in position: "<<pos<<" name: " << thing.name << " & ID : " << thing.directoryID << '\n';
            pos++;
        }
    }

    int FindDirectory(int directoryId)
    {
        int pos{ 0 };
        for (const auto& directory : m_subDirectories)
        {
            if (directory.directoryID == directoryId)
            {
                return pos; //return direc pos if found
            }
            pos++;
        }
        return -1; //return -1 if no direc found
    }

    bool isSyncing(){return m_isSyncing;}

    void startSyncing()
    {
        if(!m_isSyncing)
        {
            m_isSyncing = true;
        }
    }
    void stopSyncing()
    {
        if (m_isSyncing)
        {
            m_isSyncing = false;
        }
    }

    void exitProgram() { m_isExiting = true; }
    bool isExiting() { return m_isExiting; }

    //probably dont need this, stop token works instead
    //void guiCheck() { m_guiHP--; }
    //void guiImAlive(){ m_guiHP = 500; }
    //bool isGuiAlive() { return(m_guiHP <= 0); }//heartbeat check for filemanager to see if gui has crashed or altf4'd etc
};


#endif