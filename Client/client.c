#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include "protocol.h"

void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

void errorhandler(char *errorMessage) {
    printf("%s", errorMessage);
}

int main(int argc, char *argv[]) {
#if defined WIN32
    // Initialize Winsock
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (result != NO_ERROR) {
        printf("Error at WSAStartup()\n");
        return 0;
    }
#endif

    // create client socket
    int c_socket;
    c_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (c_socket < 0) {
        errorhandler("socket creation failed.\n");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }

    // set connection settings
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP del server
    sad.sin_port = htons(PROTO_PORT); // Server port

    // connection
    if (connect(c_socket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
        errorhandler("Failed to connect.\n");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }

    // receive from server
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    if ((recv(c_socket, buffer, BUFFER_SIZE - 1, 0)) <= 0) {
        errorhandler("recv() failed or connection closed prematurely");
        closesocket(c_socket);
        clearwinsock();
        return -1;
    }
    printf("%s\n", buffer); // Print the echo buffer

    while (1) {
        // get password type and length from user
        msg request;
        printf("Enter the type of password to generate (n: numeric, a: alphabetic, m: mixed, s: secure) followed by the length, or 'q' to quit: ");
        scanf(" %c", &request.type);

        if (request.type == 'q') {
            printf("Closing connection and exiting...\n");
            closesocket(c_socket);
            clearwinsock();
            return 0;
        }

        scanf("%d", &request.length);

        // send request to server
        if (send(c_socket, (char*)&request, sizeof(request), 0) != sizeof(request)) {
            errorhandler("send() sent a different number of bytes than expected");
            closesocket(c_socket);
            clearwinsock();
            return -1;
        }

        // receive generated password from server
        msg response;
        memset(&response, 0, sizeof(response));
        if ((recv(c_socket, (char*)&response, sizeof(response), 0)) <= 0) {
            errorhandler("recv() failed or connection closed prematurely");
            closesocket(c_socket);
            clearwinsock();
            return -1;
        }
        printf("Generated password: %s\n", response.password);
    }

    closesocket(c_socket);
    clearwinsock();
    return 0;
} // main end
