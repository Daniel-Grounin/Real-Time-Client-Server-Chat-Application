#pragma once
#include <string>
#include <memory>
#include <asio.hpp>
#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <imgui.h>

using namespace std;

#define SERVER_PORT 60000 // Listening port
#define BUF_SIZE 4096
#define MAX_CLNT 256     // Maximum number of connections

// Forward declare ChatWindow
class ChatWindow;

class Server {
public:
    Server(ChatWindow& chatWindow); // Constructor
    ~Server();                      // Destructor

    int run();                      // Starts the server
    void stop();                    // Stops the server

    asio::io_context context;

private:
    // Core server functions
    void accept_connections();
    void handle_client(std::shared_ptr<asio::ip::tcp::socket> clnt_sock);
    void broadcast_message(const std::string& msg);

    // Client management
    void add_client(const std::string& name, std::shared_ptr<asio::ip::tcp::socket> sock);
    void remove_client(const std::string& name);
    void broadcast_client_list();

    // Utility functions
    void append_to_chat_log(const std::string& message);

    // Members
    std::unordered_map<std::string, std::shared_ptr<asio::ip::tcp::socket>> clients;
    std::vector<std::thread> client_threads;
    std::mutex mtx;
    std::vector<std::string> connected_clients;
    asio::ip::tcp::acceptor acceptor;
    ChatWindow& chatWindowRef;
};
