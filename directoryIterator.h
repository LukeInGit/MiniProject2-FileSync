#ifndef DIRECTORYITERATOR_H
#define DIRECTORYITERATOR_H
#include <iostream>         //for std::cout
#include <filesystem>       //for std::filesystem
#include <string>           //for std::string


class DirectoryIterator
{
private:
    std::filesystem::directory_iterator m_iterator{};
    std::filesystem::path m_path{};
   // bool m_isMainDirectory{};

    //storing in vector calls copy function and this is unlikely to be needed anyway.
    //DirectoryIterator() = delete;                                           // prevent default constructer usage
    //DirectoryIterator(const DirectoryIterator&) = delete;                   // delete copy constructor
    //DirectoryIterator& operator=(const DirectoryIterator&) = delete;        // delete assignment constructor
public:

    DirectoryIterator(std::string filepath)//, bool isMain = false)
        :m_iterator{ filepath }, m_path{ filepath }//, m_isMainDirectory{ isMain }
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

    //bool IsMainDirectory()
    //{
    //    return m_isMainDirectory;
    //}


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

#endif // !DIRECTORYITERATOR_H
