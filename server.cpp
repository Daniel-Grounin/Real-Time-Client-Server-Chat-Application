#include "server.h"
#include "chat.h"

Server::Server(ChatWindow& chatWindow)
    : chatWindowRef(chatWindow),
    acceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), SERVER_PORT)) {
    std::cout << "Server initialized on port " << SERVER_PORT << std::endl;
}

Server::~Server() {
    stop();
}

int Server::run() {
    try {
        std::cout << "Server is running on port " << SERVER_PORT << std::endl;
        accept_connections(); // Start accepting connections
        context.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in Server run: " << e.what() << std::endl;
        stop(); // Ensure server stops gracefully on exception
    }
    return 0;
}

void Server::stop() {
    context.stop();
    for (auto& thread : client_threads) {
        if (thread.joinable())
            thread.join();
    }
    std::cout << "Server stopped." << std::endl;
}

void Server::accept_connections() {
    auto clnt_sock = std::make_shared<asio::ip::tcp::socket>(context);
    acceptor.async_accept(*clnt_sock, [this, clnt_sock](const asio::error_code& error) {
        if (!error) {
            client_threads.emplace_back(&Server::handle_client, this, clnt_sock);
        }
        // Continue accepting next connections
        accept_connections();
        });
}

void Server::handle_client(std::shared_ptr<asio::ip::tcp::socket> clnt_sock) {
    char msg[BUF_SIZE];
    asio::error_code error;

    while (true) {
        size_t len = clnt_sock->read_some(asio::buffer(msg), error);
        if (error == asio::error::eof || error) {
            std::cerr << "Receive failed: " << error.message() << "\n";
            break;
        }

        msg[len] = '\0';
        std::string message(msg);
        broadcast_message(message);  // Broadcast received message to all clients
    }

    // Handle client disconnect
    std::lock_guard<std::mutex> lock(mtx);
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (it->second == clnt_sock) {
            remove_client(it->first);
            break;
        }
    }
}

void Server::add_client(const std::string& name, std::shared_ptr<asio::ip::tcp::socket> sock) {
    std::lock_guard<std::mutex> lock(mtx);
    clients[name] = sock;
    connected_clients.push_back(name);
    broadcast_client_list();
}

void Server::remove_client(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    clients.erase(name);
    connected_clients.erase(std::remove(connected_clients.begin(), connected_clients.end(), name), connected_clients.end());
    broadcast_client_list();
}

void Server::broadcast_client_list() {
    std::string client_list = "#client_list:";
    for (const auto& client_name : connected_clients) {
        client_list += client_name + ",";
    }
    broadcast_message(client_list);
}

void Server::broadcast_message(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx);


    for (const auto& [name, sock] : clients) {
        std::cout << "Preparing to broadcast message: " << msg << " to " << name << std::endl;

        asio::error_code error;
        asio::write(*sock, asio::buffer(msg), error);

        if (!error) {
            append_to_chat_log(msg);  // Log each successful broadcast to the chat log
            std::cout << "Broadcasted message: " << msg << std::endl;
        }
        else {
            std::cerr << "Broadcast failed to " << name << ": " << error.message() << "\n";
        }
    }
}


void Server::append_to_chat_log(const std::string& message) {
    chatWindowRef.SetChatLog(message); // Update chat log in UI
    std::cout << "Server log: " << message << std::endl;  // Also print to server console

}
