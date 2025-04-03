#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

class Connection {
public:

	Connection(std::string ip, int port, short buf) {
		this->IP_SERV = new char[sizeof(ip)];
		strcpy(this->IP_SERV, ip.c_str());
		this->PORT_NUM = port;
		this->BUFF_SIZE = buf;

	}

	void CloseConnection() {
		Close();
	}

private:

	void WSAStart() {

		// WinSock initialization
		WSADATA wsData;

		int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);

		if (erStat != 0) {
			throw "Division by zero!" + WSAGetLastError();
		}
		else
			std::cout << "WinSock initialization is OK" << std::endl;

	}

	void SocketInic() {

		// Server socket initialization
		this->ServSock = socket(AF_INET, SOCK_STREAM, 0);

		if (this->ServSock == INVALID_SOCKET) {
			closesocket(this->ServSock);
			WSACleanup();
			throw "Error initialization socket # " + WSAGetLastError();
		}
		else
			std::cout << "Server socket initialization is OK" << std::endl;

	}

	void SocketAndIPPort() {

		//IP in string format to numeric format for socket functions. Data is in "ip_to_num"
		in_addr ip_to_num;
		int erStat = inet_pton(AF_INET, this->IP_SERV, &ip_to_num);

		if (erStat < 0) {
			throw "Error in IP translation to special numeric format";
		}

		// Server socket binding
		sockaddr_in servInfo;
		ZeroMemory(&servInfo, sizeof(servInfo));	// Initializing servInfo structure

		servInfo.sin_family = AF_INET;
		servInfo.sin_addr = ip_to_num;
		servInfo.sin_port = htons(this->PORT_NUM);

		erStat = bind(this->ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

		if (erStat != 0) {
			closesocket(this->ServSock);
			WSACleanup();
			throw "Error Socket binding to server info. Error # " + WSAGetLastError();
		}
		else
			std::cout << "Binding socket to Server info is OK" << std::endl;

	}

	void Listen() {

		//Starting to listen to any Clients
		int erStat = listen(this->ServSock, SOMAXCONN);

		if (erStat != 0) {
			closesocket(this->ServSock);
			WSACleanup();
			throw "Can't start to listen to. Error # " + WSAGetLastError();
		}
		else {
			std::cout << "Listening..." << std::endl;
		}

	}

	void ConnectionWithClient() {

		//Client socket creation and acception in case of connection
		sockaddr_in clientInfo;
		ZeroMemory(&clientInfo, sizeof(clientInfo));	// Initializing clientInfo structure

		int clientInfo_size = sizeof(clientInfo);

		this->ClientConn = accept(this->ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

		if (this->ClientConn == INVALID_SOCKET) {
			closesocket(this->ServSock);
			closesocket(this->ClientConn);
			WSACleanup();
			throw "Client detected, but can't connect to a client. Error # " + WSAGetLastError();
		}
		else {
			std::cout << "Connection to a client established successfully" << std::endl;
			char clientIP[22];

			inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);	// Convert connected client's IP to standard string format

			std::cout << "Client connected with IP address " << clientIP << std::endl;

		}

	}

	void Talck() {

		//Exchange text data between Server and Client. Disconnection if a client send "xxx"

		std::vector <char> servBuff(this->BUFF_SIZE), clientBuff(this->BUFF_SIZE);							// Creation of buffers for sending and receiving data
		short packet_size = 0;												// The size of sending / receiving packet in bytes

		bool run = true;
		while (run) {
			packet_size = recv(this->ClientConn, servBuff.data(), servBuff.size(), 0);					// Receiving packet from client. Program is waiting (system pause) until receive
			std::cout << "Client's message: " << servBuff.data() << std::endl;

			std::cout << "Your (host) message: ";
			fgets(clientBuff.data(), clientBuff.size(), stdin);

			// Check whether server would like to stop chatting 
			if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {

				shutdown(this->ClientConn, SD_BOTH);
				closesocket(this->ServSock);
				closesocket(this->ClientConn);
				WSACleanup();
				run = false;

			}

			if (run) {

				packet_size = send(this->ClientConn, clientBuff.data(), clientBuff.size(), 0);

				if (packet_size == SOCKET_ERROR) {
					closesocket(this->ServSock);
					closesocket(this->ClientConn);
					WSACleanup();
					run = false;
					throw "Can't send message to Client. Error # " + WSAGetLastError();
				}

			}

		}
	}

	void Close() {
		closesocket(this->ServSock);
		closesocket(this->ClientConn);
		WSACleanup();
	}

	void run() {

		try {
			WSAStart();
			SocketInic();
			SocketAndIPPort();
			Listen();
			ConnectionWithClient();
			Talck();
		}
		catch (const char* error_message) {
			std::cout << error_message << std::endl;
		}

	}

	//Key constants
	char *IP_SERV;			// Enter local Server IP address
	int PORT_NUM;				// Enter Open working server port
	short BUFF_SIZE;			// Maximum size of buffer for exchange info between server and client

	SOCKET ServSock;
	SOCKET ClientConn;

};