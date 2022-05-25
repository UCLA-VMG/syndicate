// C++ UDP Receiver

#include "iostream"


#include <winsock2.h>
#include <Ws2tcpip.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <system_error>
#include <string>
#include <iostream>

#pragma once

class WSASession
{
  public:
    WSASession()
    {
      int ret = WSAStartup(MAKEWORD(2, 2), &data);
      if (ret != 0)
        throw std::system_error(WSAGetLastError(), std::system_category(), "WSAStartup Failed");
    }
    ~WSASession()
    {
      WSACleanup();
    }

  private:
    WSAData data;
};

class UDPSocket
{
  public:
    UDPSocket()
    {
      sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (sock == INVALID_SOCKET)
        throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening socket");
    }
    ~UDPSocket()
    {
      closesocket(sock);
    }

    void SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags = 0)
    {
      sockaddr_in add;
      add.sin_family = AF_INET;
      add.sin_addr.s_addr = inet_addr(address.c_str());
      //add.sin_addr.s_addr = inet_pton(AF_INET, "127.0.0.1", address.c_str());
      add.sin_port = htons(port);
      int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));
      if (ret < 0)
        throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
    }
    void SendTo(sockaddr_in& address, const char* buffer, int len, int flags = 0)
    {
      int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR *>(&address), sizeof(address));
      if (ret < 0)
        throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
    }
    sockaddr_in RecvFrom(char* buffer, int len, int flags = 0)
    {
      sockaddr_in from;
      int size = sizeof(from);
      int ret = recvfrom(sock, buffer, 4096, flags, reinterpret_cast<SOCKADDR *>(&from), &size);
    //   int ret = recv(sock, buffer, 4096, 0);
      if (ret < 0)
        throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");

      // make the buffer zero terminated
      buffer[ret] = 0;
      return from;
    }
    void Bind(unsigned short port)
    {
      sockaddr_in add;
      add.sin_family = AF_INET;
      InetPton(AF_INET, __TEXT("192.168.33.30"), &add.sin_addr.s_addr);
      add.sin_port = htons(port);
      int ret = bind(sock, reinterpret_cast<SOCKADDR *>(&add), sizeof(add));
      std::cout << ret << "\n";
      if (ret < 0)
        throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
    }

  private:
    SOCKET sock;
};

int main()
{
  int PORT = 4098;

  int num = 1;

    if (*(char *)&num == 1)
    {
        printf("Little-Endian\n");
    }
    else
    {
        printf("Big-Endian\n");
    }

  try
  {
    WSASession Session;
    UDPSocket Socket;
    char buffer[4096];

    Socket.Bind(PORT);
    while (1)
    {
      std::cout << "hi\n";
      sockaddr_in add = Socket.RecvFrom(buffer, sizeof(buffer));
      std::cout << "hi\n";
      std::string input(buffer);
      std::cout << "Data received: " << input.substr(0,4096) << std::endl;
      break;
    }
  }
  catch (std::system_error& e)
  {
    std::cout << e.what();
  }
}