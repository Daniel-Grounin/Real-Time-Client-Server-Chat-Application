#include "client.h"
#include "chat.h"



Client::Client(ChatWindow& chatWindow) : chatWindowRef(chatWindow), resolver(context)
{
    this->host = host;
    this->port = port;
    this->name = name;
}

Client::~Client()
{
    if (sock) {
        asio::error_code error;
        sock->shutdown(asio::ip::tcp::socket::shutdown_both, error);
        sock->close(error);
    }
}

int Client::run(const std::string& host, const std::string& port, const std::string& name) {
    asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, port);
    this->name = name;
    try {
        sock = std::make_shared<asio::ip::tcp::socket>(context);
        asio::connect(*sock, endpoints);
        std::cout << "Connected to the server.\n";
        connected = true;
        // Send initial name broadcast to the server
        std::string initial_message = "#new client:" + name;
        send_message(initial_message);

        // Start a thread for receiving messages
        std::thread recv_thread([this] () { receive_message(sock); });
        recv_thread.join();  

    }
    catch (std::exception& e) {
        std::cerr << "Client exception: " << e.what() << "\n";
        return -1;
    }
    return 0;
}

void Client::send_message(const std::string& message) {
    if (!sock || !sock->is_open()) return; // Avoid sending if socket is not ready

    std::string full_message = "[" + this->name + "] " + message;
    asio::error_code error;
    asio::write(*sock, asio::buffer(full_message), error);

    cout << "Sent message: " << full_message << endl;

    if (error) {
        std::cerr << "Send failed: " << error.message() << "\n";
    }
}


void Client::receive_message(std::shared_ptr<asio::ip::tcp::socket> sock) {
    char msg[BUF_SIZE];
    asio::error_code error;

    while (true) {
        size_t len = sock->read_some(asio::buffer(msg), error);
        if (error == asio::error::eof) {
            break;
        }
        else if (error) {
            std::cerr << "Receive failed: " << error.message() << "\n";
            break;
        }
        msg[len] = '\0';
        std::string message(msg);
        std::cout << "Received message on client: " << message << std::endl;
        
    }
}


void Client::update_client_list(const std::string& client_list_str) {
    std::lock_guard<std::mutex> lock(client_mutex);
    connected_clients.clear();

    std::stringstream ss(client_list_str);
    std::string client_name;
    while (std::getline(ss, client_name, ',')) {
        if (!client_name.empty()) {
            connected_clients.push_back(client_name);
        }
    }
}

bool Client::is_connected() {
    return connected;
}


