#include "imguiGui.h"

namespace imguiGUI {
    // Data
    static LPDIRECT3D9              g_pD3D = nullptr;
    static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
    static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
    static D3DPRESENT_PARAMETERS    g_d3dpp = {};

    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



    //used for checking if window is being moved
    bool g_Moving{ false };
    // Main code
    int imguiMainLoop(int, char**)
    {
        //// // Create application window
        // ImGui_ImplWin32_EnableDpiAwareness();
        //WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
        //::RegisterClassExW(&wc);
        //HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Filesync", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

         //Create application window
        ImGui_ImplWin32_EnableDpiAwareness();
        WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
        ::RegisterClassExW(&wc);
        HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_POPUP, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

        //WS_SIZEBOX

        // Initialize Direct3D
        if (!CreateDeviceD3D(hwnd))
        {
            CleanupDeviceD3D();
            ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
            return 1;
        }

        // Show the window
        ::ShowWindow(hwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(hwnd);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX9_Init(g_pd3dDevice);

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



        DirectoryVector& directoryVector = DirectoryVector::getInstance(); //singleton of directoryvector, one call in fileManager.cpp and one in imguiGUI.cpp

        ImGui::FileBrowser fileDialog(ImGuiFileBrowserFlags_SelectDirectory| ImGuiFileBrowserFlags_HideRegularFiles);
        //default window size
        float windowWidth{ 1280 };
        float windowHeight{ 800 };
        
        int buttonAmount{ 5 };
        std::vector<bool> showButton(buttonAmount, true);
        int selectedButton{ -1 };

        bool done = false;
        bool notExiting{ true };
       // static char filePathBuffer[256]; // to put filedialog choice in textbox
      //  char filePathBuffer[10][256]; // to put filedialog choice in textbox
        std::vector<std::string> filePathBuffer;

        // Main loop
        while (!done)
        {
            // Poll and handle messages (inputs, window resize, etc.)
            // See the WndProc() function below for our to dispatch events to the Win32 backend.

            MSG msg;
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    done = true;
                if (msg.message == WM_LBUTTONUP)
                    g_Moving = false;
            }
            if (done)
                break;

            // Handle window resize (we don't resize directly in the WM_SIZE handler)
            {
                    ::SetWindowPos(hwnd, nullptr, 0, 0, static_cast<int>(windowWidth), static_cast<int>(windowHeight), SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
                if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
                {


                    g_d3dpp.BackBufferWidth = g_ResizeWidth;
                    g_d3dpp.BackBufferHeight = g_ResizeHeight;

                    g_ResizeWidth = g_ResizeHeight = 0;
                    ResetDevice();

                }
            }

            // Start the Dear ImGui frame
            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            
            // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
            {
                static float f = 0.0f;
                static int counter = 0;
                ImGui::SetNextWindowPos({ 0,0 });
                      
                // Create a window called "FileSync" and append into it.                
                //if window is being moved, dont allow it to be moved and resized at the same time
                if (g_Moving) {
                    ImGui::Begin("FileSync", &notExiting, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
                    
                }
                else {
                    ImGui::Begin("FileSync", &notExiting, ImGuiWindowFlags_NoMove);
                }

                //exit if x button pressed
                if (!notExiting)
                {
                    ::PostQuitMessage(0);
                }

                //get imgui window size for dx9 window resizing later
                    ImVec2 windowSize = ImGui::GetWindowSize();
                    windowWidth = windowSize.x;
                    windowHeight = windowSize.y;

                    if (filePathBuffer.size() <= buttonAmount)
                    {
                        for (size_t i{ filePathBuffer.size() }; i < buttonAmount; i++)
                        {
                            filePathBuffer.emplace_back();
                        }
                    }
                    for (int buttonId = 0; buttonId < buttonAmount; ++buttonId)
                    {
                        if (showButton[buttonId])
                        {
                            if (ImGui::Button(("Add Subdirectory " + std::to_string(buttonId)).c_str())) {
                                fileDialog.Open();
                                selectedButton = buttonId;
                            }
                            ImGui::SameLine();
                            if (ImGui::Button(("Delete Subdirectory " + std::to_string(buttonId)).c_str()))
                            {
                                showButton[buttonId] = false;
                                filePathBuffer[buttonId] = "";
                                directoryVector.DeleteDirectory(buttonId);
                            }
                            // ImGui::Text(("File Path " + std::to_string(buttonId)).c_str(), filePathBuffer[buttonId].data(), filePathBuffer[buttonId].size() + 1);
                            ImGui::SameLine();
                            std::string labelText = "Subdirectory " + std::to_string(buttonId) + " FilePath: " + filePathBuffer[buttonId];
                            ImGui::Text(labelText.c_str());
                        }
                    }


                    if (ImGui::Button(("add")))
                    {
                        bool found = false;
                        for (size_t i = 0; i < showButton.size(); ++i) { //cant use for range loop on bool vector for this
                            if (!showButton[i]) {
                                showButton[i] = true;
                                found = true;
                                break;
                            }
                        }
                        if (!found&& buttonAmount < 50) 
                        {
                                showButton.emplace_back(true);
                                buttonAmount++;
                        }
                    }
                    if (buttonAmount >= 50)
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red color
                        ImGui::Text("You have reached the max limit of 50");
                        ImGui::PopStyleColor();
                    }
              //  ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

               // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
               // ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

                //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                //  counter++;
                //ImGui::SameLine();
                //ImGui::Text("counter = %d", counter);

                //if (g_Moving)
                //{
                //  counter++;
                //}
              //  ImGui::InputText("File Path", filePathBuffer, sizeof(filePathBuffer));
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::End();

                fileDialog.Display();

                

                //if (fileDialog.HasSelected())
                //{
                //    std::cout << "Selected filename" << fileDialog.GetSelected().string() << selectedButton<<'\n';
                //    strcpy_s(filePathBuffer[selectedButton], fileDialog.GetSelected().string().c_str());
                //    fileDialog.ClearSelected();
                //}

                if (fileDialog.HasSelected())
                {

                    std::cout << "Selected filename: " << fileDialog.GetSelected().string() << ", Button: " << selectedButton << '\n';
                    filePathBuffer[selectedButton] = fileDialog.GetSelected().string();
                    if(directoryVector.FindDirectory(selectedButton)==-1)
                    { 
                        directoryVector.AddSubDirectory(filePathBuffer[selectedButton], selectedButton);
                        std::cout << "sub directory added\n";
                    }
                    else
                    {
                        directoryVector.EditSubDirectory(filePathBuffer[selectedButton], selectedButton);
                        std::cout << "sub directory edited \n";
                    }

                    fileDialog.ClearSelected();
                }


                //if (fileDialog.HasSelected()) {
                //    // Get the title to determine which button was clicked
                //    

                //    fileDialog.ClearSelected();
                //}


            }


            // Rendering
            ImGui::EndFrame();
            g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
            g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
            D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
            g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
            if (g_pd3dDevice->BeginScene() >= 0)
            {
                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
                g_pd3dDevice->EndScene();
            }
            HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);

            // Handle loss of D3D9 device
            if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
                ResetDevice();
        }

        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

        return 0;
    }

    // Helper functions

    bool CreateDeviceD3D(HWND hWnd)
    {
        if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
            return false;

        // Create the D3DDevice
        ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
        g_d3dpp.Windowed = TRUE;
        g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
        g_d3dpp.EnableAutoDepthStencil = TRUE;
        g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
        g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
        //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
        if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
            return false;

        return true;
    }

    void CleanupDeviceD3D()
    {
        if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
        if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
    }

    void ResetDevice()
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
        if (hr == D3DERR_INVALIDCALL)
            IM_ASSERT(0);
        ImGui_ImplDX9_CreateDeviceObjects();
    }

    // Win32 message handler
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
                return 0;
            g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
            g_ResizeHeight = (UINT)HIWORD(lParam);

            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;

        case WM_LBUTTONDOWN:
            position = MAKEPOINTS(lParam);
            return 0;
        case WM_MOUSEMOVE:
        {
            if (wParam == MK_LBUTTON)
            {
                const auto points = MAKEPOINTS(lParam);
                auto rect = ::RECT{};
                GetWindowRect(hWnd, &rect);

                // Calculate the new window position based on the mouse movement
                const int newLeft = rect.left + points.x - position.x;
                const int newTop = rect.top + points.y - position.y;

                // Check if the mouse is within the window width
                if (((position.x >= 0 && position.x <= (rect.right - rect.left) && position.y >= 0 && position.y <= 19))
                    ||
                    (position.x >= 0 && position.x <= 19 &&position.y >= 0 && position.y <= (rect.bottom - rect.top)))
                {
                    g_Moving = true;
                    ::SetWindowPos(hWnd, HWND_TOPMOST, newLeft, newTop, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
                }
            }
        }
        }
        return ::DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}