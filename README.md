
# MQTT Communication in C++ Without External Libraries

This repository contains a simple C++ code example for establishing MQTT communication without using external libraries. The code demonstrates how to connect to an MQTT broker, send MQTT messages, and receive MQTT messages using the Winsock library (`ws2_32.lib`) on Windows.

## Prerequisites

Before running the code, ensure that you have the following:

1. Code::Blocks IDE installed.
2. GNU GCC compiler configured in Code::Blocks.
3. A Windows environment for Winsock support.
4. Basic knowledge of MQTT.

## Steps to Build and Run

1. Create a new C++ project in Code::Blocks.

2. In the project settings, configure the project to use the GNU GCC compiler.

3. Link the `ws2_32` library to resolve Winsock-related dependencies. This library is required for socket communication.

4. Write or copy the provided C++ code into your project source files.

5. Build and run the project to execute the MQTT communication code.

## Code Overview

- The code establishes a socket connection to an MQTT broker running on `localhost` at port `1883`.

- It sends an MQTT CONNECT packet to establish a connection.

- It sends an MQTT PUBLISH packet to publish a message to the specified topic.

- The code is self-contained and does not rely on external MQTT libraries. It implements MQTT communication from scratch.

## File Structure

- `main.cpp`: Contains the C++ code for MQTT communication using Winsock.

## Usage

1. Clone or download this repository.

2. Open the project in Code::Blocks.

3. Build and run the project to execute the MQTT communication code.

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute it as needed.

If you encounter any issues or have questions, please feel free to reach out.
