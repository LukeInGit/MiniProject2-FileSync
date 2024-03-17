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
public:

    DirectoryIterator(std::string filepath)
        :m_iterator{ filepath }, m_path{ filepath }
    {}

    void ResetIterator() { m_iterator = std::filesystem::directory_iterator{ m_path }; }

    void ChangePath(std::string path)
    {
        m_path = path;
        ResetIterator();
    }

    const std::filesystem::directory_iterator& GetIterator() const {return m_iterator;}
    const std::filesystem::path& GetPath() const{ return m_path;}

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
