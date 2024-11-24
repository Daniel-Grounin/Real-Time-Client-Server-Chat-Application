#pragma once
#include "imgui.h"
#include "client.h"
#include "server.h"
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <optional>

using namespace std;

class ChatWindow {
public:
    ChatWindow(); // Constructor
    void append_to_chat_log(const std::string& message);
    void RenderUI(); // Render the UI components
    string GetChatLog(); // Get the chat log
    void SetChatLog(const string& chat_log); // Set the chat log
private:
    // Use unique pointers for Client and Server instances
    std::unique_ptr<Client> client;
    std::unique_ptr<Server> server;

    vector<string> connected_clients;

    // Chat-related variables
    std::string chat_log;
    std::mutex log_mutex;

    // Network details
    std::string host;
    std::string port;
    std::string name;

    // Input buffer
    static char input_buf[256];

    // Threads for server and client
    std::thread server_thread;
    std::thread client_thread;
};
