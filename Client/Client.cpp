#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>
#include <string>

#define TIME_PORT	27015

void PrintMsg()
{
	cout << "Please type a number for action you want to ask the server:\n" <<
		"1-GetTime\n" <<
		"2-GetTimeWithoutDate\n" <<
		"3-GetTimeSinceEpoch\n" <<
		"4-GetClientToServerDelayEstimation\n" <<
		"5-MeasureRTT\n" <<
		"6-GetTimeWithoutDateOrSeconds\n" <<
		"7-GetYear\n" <<
		"8-GetMonthAndDay\n" <<
		"9-GetSecondsSinceBeginingOfMonth\n" <<
		"10-GetWeekOfYear\n" <<
		"11-GetDaylightSavings \n" <<
		"12-GetTimeWithoutDateInCity \n" <<
		"13-MeasureTimeLap \n" <<
		"14-Exit\n" <<
		"Your choice is: ";
}

void PrintCities()
{
	cout << "The available cities are:\n" <<
		"Doha\n" <<
		"Prague\n" <<
		"New-York\n" <<
		"Berlin\n" <<
		"Choose one of the cities above, by writing the name of the city.\n" <<
		"Your choice is: ";
}

void main()
{
	// Initialize Winsock (Windows Sockets).
	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return;
	}

	// Client side:
	// Create a socket and connect to an internet address.
	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a client to communicate on a network, it must connect to a server.
	// Need to assemble the required data for connection in sockaddr structure.
	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	// My variables.
	bool run = true;
	int ClientInput;
	string city;

	while (run)
	{
		ClientInput = -1;
		while (ClientInput < 1 || ClientInput > 14) // Check for correct input. (between 1-14)
		{
			PrintMsg();
			cin >> ClientInput;

			if (ClientInput == 12)
			{
				cout << "\n";
				PrintCities();
				cin >> city;
			}
			if (ClientInput == 14)
				exit(0);
			cout << "\n";
		}

		// Send and receive data.
		int bytesSent = 0;
		int bytesRecv = 0;
		char sendBuff[255];
		char recvBuff[255];


		string input = to_string(ClientInput);
		if (ClientInput == 12)
			input += "-" + city;

		strcpy(sendBuff, input.c_str());

		// Asks the server for data.
		// The send function sends data on a connected socket.
		// The buffer to be sent and its size are needed.
		// The fourth argument is an idicator specifying the way in which the call is made (0 for default).
		// The two last arguments hold the details of the server to communicate with. 
		// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
		if (ClientInput != 4 && ClientInput != 5)
		{
			bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
			if (SOCKET_ERROR == bytesSent)
			{
				cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
				closesocket(connSocket);
				WSACleanup();
				return;
			}

			cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";

			// Gets the server's answer using simple recieve (no need to hold the server's address).
			bytesRecv = recv(connSocket, recvBuff, 255, 0);
			if (SOCKET_ERROR == bytesRecv)
			{
				cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
				closesocket(connSocket);
				WSACleanup();
				return;
			}
			recvBuff[bytesRecv] = '\0'; // Add the null-terminating to make it a string.
			cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n\n";
		}
		else if (ClientInput == 4) // sendBuff = "4".
		{
			for (int i = 0; i < 100; i++)
			{
				bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
				if (SOCKET_ERROR == bytesSent)
				{
					cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}
			}
			int time = -1, AvgTime = 0;
			for (int i = 0; i < 100; i++)
			{
				// Gets the server's answer using simple recieve (no need to hold the server's address).
				bytesRecv = recv(connSocket, recvBuff, 255, 0);
				if (SOCKET_ERROR == bytesRecv)
				{
					cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}
				recvBuff[bytesRecv] = '\0'; // Add the null-terminating to make it a string.

				if (time == -1) // Firt request.
					time = stoi(recvBuff);
				else
				{
					AvgTime += stoi(recvBuff) - time;
					time = stoi(recvBuff);
				}
			}
			AvgTime /= 100;
			cout << "Time Client: The average of the 100 requests is: " << AvgTime << ". \n\n";
		}
		else if (ClientInput == 5) // sendBuff = "5".
		{
			DWORD sendTime, reciveTime, AvgTime = 0;
			for (int i = 0; i < 100; i++)
			{
				bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
				if (SOCKET_ERROR == bytesSent)
				{
					cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}
				sendTime = GetTickCount(); // Get the time the request sended.

				// Gets the server's answer using simple recieve (no need to hold the server's address).
				bytesRecv = recv(connSocket, recvBuff, 255, 0);
				if (SOCKET_ERROR == bytesRecv)
				{
					cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}
				recvBuff[bytesRecv] = '\0'; // Add the null-terminating to make it a string.
				reciveTime = GetTickCount(); // Get the time the response accepted.

				AvgTime += reciveTime - sendTime;
			}
			AvgTime /= 100;
			cout << "Time Client: The average of the 100 RTT's is: " << AvgTime << ". \n\n";
		}
	}

	// Closing connections and Winsock.
	cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);

	cout << "\n";
}

