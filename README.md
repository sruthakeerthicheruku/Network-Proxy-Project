# Custom Network Proxy Project
## About the Project
This project is a basic network proxy implemented in C++.  
The proxy works as a middleman between a client and a server by receiving client requests, forwarding them to the target server, and sending the server’s response back to the client.
It supports both **HTTP** and **HTTPS (CONNECT tunneling)**. Blocked hosts can be mentioned through a `blocked.txt` file.

---

## Features
- Accepts client connections over TCP on port **8080** (default)  
- Supports **HTTP** and **HTTPS (CONNECT)** requests  
- Blocks requests to hosts listed in `blocked.txt`  
- Handles **multiple clients concurrently** using threads  
- Logs client connections and blocked hosts  

---

## Technologies & Concepts
- **Language:** C++  
- **Libraries:** `<winsock2.h>`, `<ws2tcpip.h>`, `<windows.h>`, `<iostream>`, `<fstream>`, `<set>`  
- **Concepts:** TCP/IP Networking, Socket Programming, Multi-threading, Client–Server Architecture  

---

## Prerequisites
- Windows OS  
- g++ with WinSock2 support (e.g., via MinGW) or Visual Studio C++  
- `blocked.txt` file in the same directory for blocked hosts  

---

## How to Run

### Step 1: Compile the Project

```bash
g++ proxy.cpp -o proxy.exe -lws2_32
```
### Step 2: Run the Proxy

```bash
proxy.exe
```
- Proxy runs on: 

```bash
127.0.0.1:8080
```
- Configure your browser or client to use this proxy address.
- Requests to hosts listed in blocked.txt will be blocked automatically.

## How It Works
### HTTP Requests

1. Client sends an HTTP request to the proxy
2. Proxy extracts the Host header
3. If the host is blocked, the connection is closed
- Otherwise:
1. Proxy connects to the target server on port 8080
2. Forwards the request
3. Sends the response back to the client

### HTTPS Requests (CONNECT)

1. Client sends a CONNECT host:443 request
2. Proxy checks if the host is blocked

- If allowed:

1. Connects to the target server
2. Sends HTTP/1.1 200 Connection Established
3. Starts bi directional forwarding of encrypted data using threads
---
### Blocked Hosts

- Blocked hosts are listed in a plain text file called blocked.txt

---
### Logging

- The proxy logs activity directly to the console.
---
## Learning Outcome
- Learned how a network proxy works internally
- Learned WinSock2 socket programming
- Understood HTTP and HTTPS client-server communication
- Implemented multi threading to handle concurrent clients
- Improved networking, and C++ programming skills

---
