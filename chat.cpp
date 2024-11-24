#include "chat.h"
#include "client.h"

// Static buffer for input
char ChatWindow::input_buf[256] = "";

ChatWindow::ChatWindow() : host("127.0.0.1"), port("60000"), name("Client") {}

void ChatWindow::RenderUI() {
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
            ImGui::MenuItem("Padding", NULL, &opt_padding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode;
            }
            if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit;
            }
            if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking;
            }
            if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            }
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            }
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen))
            {
                dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            }
            ImGui::Separator();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();

    // Set a custom style for ImGui
    ImGuiStyle& style = ImGui::GetStyle();

    // Customizing the window and UI elements
    style.WindowRounding = 5.0f;          // Rounding for windows
    style.FrameRounding = 4.0f;           // Rounding for frames/buttons
    style.GrabRounding = 4.0f;            // Rounding for slider grab bars
    style.WindowBorderSize = 1.0f;        // Border thickness for windows
    style.FrameBorderSize = 1.0f;         // Border thickness for frames/buttons
    style.WindowPadding = ImVec2(15, 15); // Padding inside windows
    style.FramePadding = ImVec2(10, 5);   // Padding inside frames/buttons
    style.ItemSpacing = ImVec2(12, 8);    // Spacing between items (buttons, sliders, etc.)

    // Set custom colors
    ImGui::StyleColorsDark(); // Use the dark theme as a base

    // Customize specific colors (e.g., buttons, text, windows)
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.18f, 1.00f);      // Dark background for the window
    colors[ImGuiCol_TitleBg] = ImVec4(0.25f, 0.28f, 0.30f, 1.00f);       // Title bar background
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.35f, 0.38f, 0.40f, 1.00f); // Active title bar
    colors[ImGuiCol_Button] = ImVec4(0.45f, 0.45f, 0.60f, 1.00f);        // Button color
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.55f, 0.55f, 0.70f, 1.00f); // Button hover
    colors[ImGuiCol_ButtonActive] = ImVec4(0.65f, 0.65f, 0.80f, 1.00f);  // Button active click
    colors[ImGuiCol_Text] = ImVec4(0.85f, 0.85f, 0.90f, 1.00f);          // Text color
    colors[ImGuiCol_Header] = ImVec4(0.40f, 0.40f, 0.50f, 1.00f);        // Headers
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.50f, 0.50f, 0.60f, 1.00f); // Headers hover

    // You can also adjust window transparency
    colors[ImGuiCol_WindowBg].w = 0.90f; // Window background transparency (last value is alpha)



    ImGui::Begin("Chat Window");

    // Button to start server
    if (ImGui::Button("Start Server")) {
        if (!server) { // Ensure only one server instance is created
            server = std::make_unique<Server>(*this);
            server_thread = std::thread([this]() {
                try {
                    server->run();
                }
                catch (const std::exception& e) {
                    std::cerr << "Server run exception: " << e.what() << std::endl;
                }
                });
            server_thread.detach();
        }
    }

    ImGui::InputText("Host", &host[0], host.capacity() + 1);
    ImGui::InputText("Port", &port[0], port.capacity() + 1);
    ImGui::InputText("Name", &name[0], name.capacity() + 20);

    // Button to start client
    if (ImGui::Button("Start Client")) {
        client = std::make_unique<Client>(*this);
            client_thread = std::thread([&](){
                try {
                    client->run(host, port, name);
                }
                catch (const std::exception& e) {
                    std::cerr << "Client run exception: " << e.what() << std::endl;
                }
                });
               
            client_thread.detach();
        
    }

    // Chat log display
    ImGui::BeginChild("Chat Log", ImVec2(0, -40), true);
    std::lock_guard<std::mutex> lock(log_mutex); // Ensure thread-safe access to chat log
    ImGui::TextUnformatted(chat_log.c_str());
    ImGui::EndChild();

    // Input field and Send button
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 70);
    ImGui::InputText("##input", input_buf, IM_ARRAYSIZE(input_buf), ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();

    if (ImGui::Button("SEND", ImVec2(60, 0)) || ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
        if (strlen(input_buf) > 0) { // Ensure client is connected
            std::string message(input_buf);
            client->send_message(message); // Use client instance to send message
            input_buf[0] = '\0'; // Clear input buffer after sending
        }
    }

    ImGui::End();

    // Connected Users Window
    ImGui::Begin("Connected Users");
    for (const auto& client_name : connected_clients) {
        ImGui::Text("%s", client_name.c_str());
    }
    ImGui::End();
}

string ChatWindow::GetChatLog()
{
    return chat_log;
}

void ChatWindow::SetChatLog(const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    chat_log += message + '\n';
    cout << "Chat log: " << chat_log << endl;
}

void ChatWindow::append_to_chat_log(const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex);
    chat_log += message + "\n";
}


