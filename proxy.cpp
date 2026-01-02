#include <iostream>
#include <string>
#include <fstream>
#include <set>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const int port = 8080;
const int buffer_size = 8192;

set<string> blocked_hosts;

void load_blocked_hosts()
{
  ifstream f("blocked.txt");
  string line;
  while (getline(f, line))
  {
    if (!line.empty())
      blocked_hosts.insert(line);
  }
  f.close();
}

string extracthost(const char *buffer)
{
  string host;
  int n = strlen(buffer);

  for (int i = 0; i + 5 < n; ++i)
  {
    if (buffer[i] == 'H' && buffer[i + 1] == 'o' && buffer[i + 2] == 's' &&
        buffer[i + 3] == 't' && buffer[i + 4] == ':')
    {
      i += 6;
      while (i < n && buffer[i] != '\r' && buffer[i] != '\n')
        host.push_back(buffer[i++]);
      break;
    }
  }
  return host;
}

DWORD WINAPI forward(LPVOID param)
{
  SOCKET *s = (SOCKET *)param;
  SOCKET src = s[0];
  SOCKET dst = s[1];
  char buffer[buffer_size];
  int bytes;
  while ((bytes = recv(src, buffer, buffer_size, 0)) > 0)
  {
    send(dst, buffer, bytes, 0);
  }
  closesocket(src);
  closesocket(dst);
  delete[] s;
  return 0;
}

void handleclient(SOCKET clientsocket)
{
  char buffer[buffer_size] = {0};
  int bytes = recv(clientsocket, buffer, buffer_size, 0);
  if (bytes <= 0)
  {
    closesocket(clientsocket);
    return;
  }

  string data(buffer, bytes);

  if (data.substr(0, 7) == "CONNECT")
  {
    size_t sp1 = data.find(' ');
    size_t sp2 = data.find(':', sp1 + 1);
    string host = data.substr(sp1 + 1, sp2 - sp1 - 1);

    if (blocked_hosts.find(host) != blocked_hosts.end())
    {
      cout << "blocked https host -> " << host << endl;
      closesocket(clientsocket);
      return;
    }

    cout << "https client -> host: " << host << endl;

    SOCKET serversocket = socket(AF_INET, SOCK_STREAM, 0);
    hostent *hostinfo = gethostbyname(host.c_str());
    if (!hostinfo)
    {
      closesocket(clientsocket);
      return;
    }

    sockaddr_in serveraddr{};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(443);
    memcpy(&serveraddr.sin_addr, hostinfo->h_addr, hostinfo->h_length);

    if (connect(serversocket, (sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
      closesocket(clientsocket);
      closesocket(serversocket);
      return;
    }

    // send http 200 connection established to client
    const char *msg = "HTTP/1.1 200 Connection Established\r\n\r\n";
    send(clientsocket, msg, strlen(msg), 0);

    // forward bytes both ways
    SOCKET *s1 = new SOCKET[2]{clientsocket, serversocket};
    SOCKET *s2 = new SOCKET[2]{serversocket, clientsocket};
    CreateThread(nullptr, 0, forward, s1, 0, nullptr);
    CreateThread(nullptr, 0, forward, s2, 0, nullptr);
  }
  else
  {

    string host = extracthost(buffer);
    if (host.empty() || blocked_hosts.find(host) != blocked_hosts.end())
    {
      if (!host.empty())
        cout << "blocked http host -> " << host << endl;
      closesocket(clientsocket);
      return;
    }

    cout << "http client -> host: " << host << endl;

    hostent *hostinfo = gethostbyname(host.c_str());
    if (!hostinfo)
    {
      closesocket(clientsocket);
      return;
    }

    SOCKET serversocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serversocket == INVALID_SOCKET)
    {
      closesocket(clientsocket);
      return;
    }

    sockaddr_in serveraddr{};
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(80);
    memcpy(&serveraddr.sin_addr, hostinfo->h_addr, hostinfo->h_length);

    if (connect(serversocket, (sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
    {
      closesocket(serversocket);
      closesocket(clientsocket);
      return;
    }

    send(serversocket, buffer, bytes, 0);

    int br;
    while ((br = recv(serversocket, buffer, buffer_size, 0)) > 0)
      send(clientsocket, buffer, br, 0);

    closesocket(serversocket);
    closesocket(clientsocket);
  }
}

DWORD WINAPI clientthread(LPVOID param)
{
  SOCKET clientsocket = (SOCKET)param;
  handleclient(clientsocket);
  return 0;
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  load_blocked_hosts();

  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
  {
    cout << "winsock init failed" << endl;
    return 1;
  }

  SOCKET proxysocket = socket(AF_INET, SOCK_STREAM, 0);
  if (proxysocket == INVALID_SOCKET)
  {
    cout << "socket creation failed" << endl;
    WSACleanup();
    return 1;
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(proxysocket, (sockaddr *)&addr, sizeof(addr)) < 0)
  {
    cout << "bind failed" << endl;
    closesocket(proxysocket);
    WSACleanup();
    return 1;
  }

  if (listen(proxysocket, 10) < 0)
  {
    cout << "listen failed" << endl;
    closesocket(proxysocket);
    WSACleanup();
    return 1;
  }

  cout << "proxy running on port " << port << endl;

  while (true)
  {
    SOCKET clientsocket = accept(proxysocket, nullptr, nullptr);
    if (clientsocket == INVALID_SOCKET)
      continue;

    // lowercase thread
    CreateThread(nullptr, 0, clientthread, (LPVOID)clientsocket, 0, nullptr);
  }

  closesocket(proxysocket);
  WSACleanup();
  return 0;
}
