#include <iostream>
#include <vector>
#include <string>
#include <winsock2.h>
#include <cstdint>
#include <algorithm>

// Constants for MQTT Protocol
const uint8_t MQTT_CONNECT = 0x10;
const uint8_t MQTT_PUBLISH = 0x30;
const uint8_t MQTT_PROTOCOL_LEVEL = 0x04; // 5 for MQTT 5.0
const uint8_t MQTT_CONNECT_FLAGS = 0x02; // Clean Session

// Network Configuration
const char *MQTT_PROTOCOL_NAME = "MQTT";
const char *SERVER_ADDRESS = "127.0.0.1";
const uint16_t SERVER_PORT = 1883;

// MQTT Topics and Messages
const std::string CLIENT_ID = "cpp-mqtt-cli-client";
const std::string TOPIC = "test/topic";
const std::string FINAL_MESSAGE = "/exit";

// Class declaration
class PACKET {

private:
    std::vector<uint8_t> packet_;
public:
    explicit PACKET(uint8_t type) {
        packet_.push_back(type); // Add packet type at construction
    }

    void push(uint8_t byte) {
        packet_.push_back(byte);
    }

    void push(const std::string &str) {
        push(static_cast<uint8_t>(str.size())); // Add length byte
        for (char c: str) push(static_cast<uint8_t>(c)); // Add string bytes
    }

    void finalize() {
        packet_.insert(packet_.begin() + 1, packet_.size() - 1); // Add remaining length
    }

    const std::vector<uint8_t> &data() const {
        return packet_;
    }

    int size() const {
        unsigned long long value = packet_.size();
        if (value > static_cast<unsigned long long>(INT_MAX)) {
            std::cerr << "Warning: Value is too large to be converted to int safely." << std::endl;
            return -1;
        }
        return static_cast<int>(value);
    }

};

// Function Declarations
void initWinSock();

SOCKET createSocket();

void sendPacket(SOCKET sockFd, const PACKET &packet, const char *packetType);

void sendConnectPacket(SOCKET sockFd);

void sendPublishPacket(SOCKET sockFd, const std::string &topic, const std::string &message);

// Main Program
int main() {
    std::cout << "Enter messages to send to the broker. Type '/exit' to quit.\n";

    initWinSock();
    SOCKET sockFd = createSocket();

    sendConnectPacket(sockFd);

    while (true) {
        std::string input;
        std::cout << "Enter message: ";
        std::getline(std::cin, input);

        if (input == FINAL_MESSAGE) {
            break;
        }

        sendPublishPacket(sockFd, TOPIC, input);
    }

    closesocket(sockFd);
    WSACleanup();
    return 0;
}

// Function Definitions
void initWinSock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WinSock init failed\n";
        exit(1);
    }
}

SOCKET createSocket() {
    SOCKET sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << '\n';
        WSACleanup();
        exit(1);
    }

    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    if (connect(sockFd, reinterpret_cast<SOCKADDR *>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed with error code: " << WSAGetLastError() << '\n';
        closesocket(sockFd);
        WSACleanup();
        exit(1);
    }
    return sockFd;
}

void sendPacket(SOCKET sockFd, const PACKET &packet, const char *packetType) {
    if (
            send(
                    sockFd,
                    reinterpret_cast<const char *>(packet.data().data()),
                    packet.size(),
                    0

                    )
                    == SOCKET_ERROR
                    )
    {
        std::cerr << packetType << " packet send failed: " << WSAGetLastError() << '\n';
    } else {
        std::cout << packetType << " packet sent successfully.\n";
    }
}

void sendConnectPacket(SOCKET sockFd) {
    PACKET packet(MQTT_CONNECT);

    // Variable Header
    packet.push(0x00);
    packet.push(MQTT_PROTOCOL_NAME);

    packet.push(MQTT_PROTOCOL_LEVEL);
    packet.push(MQTT_CONNECT_FLAGS);

    packet.push(0x00);
    packet.push(0x3C); // 60 seconds

    packet.push(0x00);
    packet.push(CLIENT_ID);

    packet.finalize();

    sendPacket(sockFd, packet, "CONNECT");
}

void sendPublishPacket(SOCKET sockFd, const std::string &topic, const std::string &message) {
    PACKET packet(MQTT_PUBLISH);

    packet.push(0x00);
    packet.push(topic);

    packet.push(message);

    packet.finalize();

    sendPacket(sockFd, packet, "PUBLISH");
}