#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

void main()
{
    string ipAddress = "127.0.0.1";            // IP Address of the server
    int port = 54000;                        // Listening port # on the server

    // Initialize WinSock
    WSAData data;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);
    if (wsResult != 0)
    {
        cerr << "Can't start Winsock..." << endl;
        exit(3);
    }

    // Create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        cerr << "Can't create socket..." << endl;
        WSACleanup();
        exit(4);
    }

    // Fill in a hint structure
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    // Connect to server
    int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR)
    {
        cerr << "Can't connect to server..." << endl;
        closesocket(sock);
        WSACleanup();
        exit(5);
    }

    cout << "Connecting...\n";

    while (true) {
        char buf[4096];
        string userInput;
        string chosedOper;

        cout << "Subsciribe to:\n\n";
        cout << "1. Hourly weather prediction\n2. Minutely stock exchange rate\n3. Daily exchange rate\n\n--> ";
        cin >> chosedOper;


        int sendResult = send(sock, chosedOper.c_str(), chosedOper.size() + 1, 0);
        if (sendResult != SOCKET_ERROR)
        {
            ZeroMemory(buf, 4096);
            int bytesReceived = recv(sock, buf, 4096, 0);
            if (bytesReceived > 0)
            {
                cout << "[SERVER]> " << string(buf, 0, bytesReceived) << endl;
            }
        }

        int k = 1;
        string isContinue;

        while (true) {

            if (k % 5 == 0) {
                cout << "[SERVER]> " << "Do you want to continue subsciption: Y | N: ";
                cin >> isContinue;
                int continueResult = send(sock, isContinue.c_str(), isContinue.size() + 1, 0);

                if (continueResult == SOCKET_ERROR) exit(5);

                ZeroMemory(buf, 4096);

                int bytesReceived = recv(sock, buf, 4096, 0);
                if (bytesReceived > 0)
                {
                    cout << "[SERVER]> " << string(buf, 0, bytesReceived) << endl;
                }

                if (!isContinue.compare("N")) {
                    break;
                }
            }
            else {
                ZeroMemory(buf, 4096);

                int bytesReceived = recv(sock, buf, 4096, 0);
                if (bytesReceived > 0)
                {
                    cout << "[SERVER]> " << string(buf, 0, bytesReceived) << endl;
                }
            }

            k++;
        }
    }



    // Gracefully close down everything
    closesocket(sock);
    WSACleanup();
}