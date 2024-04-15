#include "imguiGui.h"
#include "fileManager.h"
#include <windows.h> //needed if using windows subsystem

//run with console showing up, requires linker>system>subsytem to be console
int main()
{
	std::stop_source mainssource;
	std::vector<std::jthread> threads;

	//imguiGUI::imguiMainLoop(__argc, __argv); //currently runs imgui's dx9 example
	//fManager::runFM(); //runs current main loop of filemanager



    // Create a jthread for imguiGUI::imguiMainLoop
    threads.emplace_back([&]() {
        std::cout << "initiating imguiMainLoop\n";
        imguiGUI::imguiMainLoop(__argc, __argv);
        std::cout << "imguiMainLoop complete \n";
        std::cout << "requesting mainssource stop\n";
        mainssource.request_stop(); //stop requested after imguimainloop ends, needed in case of unexpected gui termination, i.e crash or alt f4
        std::cout << "stop requested\n";
        });

    // Create a jthread for fManager::runFM
    threads.emplace_back([&]() {
        std::cout << "initiating runFM\n";
        fManager::runFM(mainssource.get_token()); // Pass imguiStopSource token to runFM
        std::cout << "runFM complete\n";
        });

    // Wait for all threads to finish
    for (auto& thread : threads) {
        std::cout << "waiting for thread to join\n";
        thread.join();
        std::cout << "thread joined\n";
    }

}

////run without console showing up, requires linker>system>subsytem to be windows
//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
//{
//
//    UNREFERENCED_PARAMETER(hInstance);
//    UNREFERENCED_PARAMETER(hPrevInstance);
//    UNREFERENCED_PARAMETER(lpCmdLine);
//    UNREFERENCED_PARAMETER(nCmdShow);
//
//    imguiGUI::imguiMainLoop(__argc, __argv);
//
//    return 0;
//}