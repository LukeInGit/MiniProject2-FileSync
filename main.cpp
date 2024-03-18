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
        imguiGUI::imguiMainLoop(__argc, __argv);
        mainssource.request_stop();
        });

    // Create a jthread for fManager::runFM
    threads.emplace_back([&]() {
        fManager::runFM(mainssource.get_token()); // Pass imguiStopSource token to runFM
        });

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
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