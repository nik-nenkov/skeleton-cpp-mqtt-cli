#include <iostream>
#include <vector>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

class Packet {
public:
    Packet(uint8_t type) {
        packet_.push_back(type); // Add packet type at construction
    }

    void push(uint8_t byte) {
        packet_.push_back(byte);
    }

    void push(const std::string& str) {
        push(static_cast<uint8_t>(str.size())); // Add length byte
        for (char c : str) push(static_cast<uint8_t>(c)); // Add string bytes
    }

    void finalize() {
        packet_.insert(packet_.begin() + 1, packet_.size() - 1); // Add remaining length
    }

    const std::vector<uint8_t>& data() const {
        return packet_;
    }

    size_t size() const {
        return packet_.size();
    }

private:
    std::vector<uint8_t> packet_;
};

// Constants for MQTT Protocol
const uint8_t MQTT_CONNECT = 0x10;
const uint8_t MQTT_PUBLISH = 0x30;
const uint8_t MQTT_PROTOCOL_LEVEL = 0x04; // 4 for MQTT 3.1.1
const uint8_t MQTT_CONNECT_FLAGS = 0x02; // Clean Session
const uint8_t NEXT_BIT = 0x00;
const uint8_t KEEP_ALIVE_SIZE = 0x3C;

// Network Configuration
const char* SERVER_ADDRESS = "127.0.0.1";
const uint16_t SERVER_PORT = 1883;

// MQTT Topics and Messages
const std::string MQTT_PROTOCOL_NAME = "MQTT";
const std::string CLIENT_ID = "YourClientID";
const std::string TOPIC = "test/topic";
const std::string MESSAGE = "Hello, MQTT!";

// Function Declarations
void initWinsock();
SOCKET createSocket();
void sendPacket(SOCKET sockfd, const Packet& packet, const char* packetType);
void sendConnectPacket(SOCKET sockfd);
void sendPublishPacket(SOCKET sockfd, const std::string& topic, const std::string& message);

// Main Program
int main() {
    std::cout << "Enter messages to send to the broker. Type '/exit' to quit.\n";

    initWinsock();
    SOCKET sockfd = createSocket();

    sendConnectPacket(sockfd);

    while (true) {
        std::string input;
        std::cout << "Enter message: ";
        std::getline(std::cin, input);

        if (input == "/exit") {
            break;
        }

        sendPublishPacket(sockfd, TOPIC, input);
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;
}

// Function Definitions
void initWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock init failed\n";
        exit(1);
    }
}
SOCKET createSocket() {
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << '\n';
        WSACleanup();
        exit(1);
    }

    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    if (connect(sockfd, reinterpret_cast<SOCKADDR*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed with error code: " << WSAGetLastError() << '\n';
        closesocket(sockfd);
        WSACleanup();
        exit(1);
    }
    return sockfd;
}
void sendPacket(SOCKET sockfd, const Packet& packet, const char* packetType) {
    if (send(sockfd, reinterpret_cast<const char*>(packet.data().data()), packet.size(), 0) == SOCKET_ERROR) {
        std::cerr << packetType << " packet send failed: " << WSAGetLastError() << '\n';
    } else {
        std::cout << packetType << " packet sent successfully.\n";
    }
}

void sendConnectPacket(SOCKET sockfd) {
    Packet packet(MQTT_CONNECT);

    // Variable Header
    packet.push(NEXT_BIT);
    packet.push(MQTT_PROTOCOL_NAME);

    packet.push(MQTT_PROTOCOL_LEVEL);
    packet.push(MQTT_CONNECT_FLAGS);

    packet.push(NEXT_BIT); // Keep Alive MSB
    packet.push(KEEP_ALIVE_SIZE);

    packet.push(NEXT_BIT); // Client ID MSB
    packet.push(CLIENT_ID);

    packet.finalize();

    sendPacket(sockfd, packet, "CONNECT");
}

void sendPublishPacket(SOCKET sockfd, const std::string& topic, const std::string& message) {
    Packet packet(MQTT_PUBLISH);

    packet.push(NEXT_BIT);
    packet.push(topic);

    packet.push(message); // Payload

    packet.finalize();

    sendPacket(sockfd, packet, "PUBLISH");

    }

