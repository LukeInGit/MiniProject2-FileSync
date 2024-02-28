#include <iostream>
#include <filesystem>

class DirectoryIterator
{
private:
    std::filesystem::directory_iterator m_iterator{};
    std::string m_path{};

    DirectoryIterator() = delete;                                           // prevent default constructer usage
    DirectoryIterator(const DirectoryIterator&) = delete;                   // delete copy constructor
    DirectoryIterator& operator=(const DirectoryIterator&) = delete;        // delete assignment constructor
public:

    explicit DirectoryIterator(std::string filepath)                        // must provide a string when initialising
        :m_iterator{ filepath }, m_path{ filepath }
    {}

    const std::filesystem::directory_iterator& GetIterator() const
    {
        return m_iterator;
    }

    void ResetIterator()
    {
        m_iterator = std::filesystem::directory_iterator{ m_path };
    }

    void CheckDirectory()                                                   // print out filenames in directory
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

void basicloop(DirectoryIterator& iterator)
{
    while (true)
    {
        iterator.CheckDirectory();
    }
}

int main()
{

    DirectoryIterator bob{ "C:/Users/Luke/source/repos/MiniProject_2_FileSync/CopyFrom" };

    basicloop(bob);


    return 0;
}