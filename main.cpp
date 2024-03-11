#include "imguiGui.h"
#include "fileManager.h"
#include <windows.h>


//run with console showing up, requires linker>system>subsytem to be console
int main()
{
	//fManager::runFM(); //runs current main loop of filemanager
	imguiGUI::imguiMainLoop(__argc, __argv); //currently runs imgui's dx9 example
}




////run without console showing up, requires linker>system>subsytem to be windows
//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
//{
//
//    UNREFERENCED_PARAMETER(hInstance);
//    UNREFERENCED_PARAMETER(hPrevInstance);
// 
// 
// 
// 
// 
// 
// 
//    UNREFERENCED_PARAMETER(lpCmdLine);
//    UNREFERENCED_PARAMETER(nCmdShow);
//
//    imguiGUI::imguiMainLoop(__argc, __argv);
//
//    return 0;
//}