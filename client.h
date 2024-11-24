#pragma once
#include <iostream>
#include <string>
#include <asio.hpp>
#include <memory>
#include <thread>
#include <vector>
#include <mutex>
#include <sstream>
using namespace std;

#define BUF_SIZE 4096

// Forward declare ChatWindow
class ChatWindow;

class Client {
public:
    Client(ChatWindow& chatWindow);
    ~Client();

    int run(const std::string& host, const std::string& port, const std::string& name);
    shared_ptr<asio::ip::tcp::socket> sock;
    void send_message(const std::string& message);
    bool is_connected();
private:
    // Core client functions
    void receive_message(std::shared_ptr<asio::ip::tcp::socket> sock);
    void update_client_list(const std::string& client_list_str);

    // Data members
    std::string host, port, name;
    asio::io_context context;
    std::mutex client_mutex;
    vector<string> connected_clients;
    asio::ip::tcp::resolver resolver;
    bool connected = false;
    ChatWindow& chatWindowRef;

};
