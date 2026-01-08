# Custom Network Proxy Project
## About the Project
This project is a basic network proxy implemented in C++.  
The proxy works as a middleman between a client and a server by receiving client requests, forwarding them to the target server, and sending the server’s response back to the client.

---

## What This Project Does
- Accepts client connections using TCP sockets  
- Forwards client requests to the destination server  
- Receives responses from the server and sends them back to the client  
- Handles multiple clients using threads  
- Demonstrates basic client-server communication  

---

## Tech Stack & Concepts Used
- **Language:** C++  
- **Concepts:**
  - Socket Programming  
  - TCP/IP Networking  
  - Multi-threading  
  - Client–Server Architecture  
- **Libraries Used:**
  - `<iostream>`
  - `<thread>`
  - `<netdb.h>`
  - `<arpa/inet.h>`
  - `<unistd.h>`

---

## How to Run the Project

### Step 1: Compiler
Make sure `g++` is installed.


### Step 2: Compile the Code
```bash
g++ proxy.cpp -o proxy -pthread
```

### Step 3: Run the Proxy
```bash
./proxy
```

### Step 4: Use the Proxy
- Connect a client or test program to the proxy’s IP and port
- The proxy will forward requests to the destination server and return the response

---

## How It Works
1. The proxy listens for incoming client connections
2. A new thread is created for each client
3. Client requests are read by the proxy
4. The proxy connects to the destination server
5. Requests are forwarded to the server
6. Server responses are sent back to the client

---

## Learning Outcome
- Learned how network proxies work
- Gained experience with socket programming in C++
- Understood TCP client server communication
- Used multi-threading to handle multiple connections
- Improved debugging and networking skills

---
