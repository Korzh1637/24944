#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_PATH "/tmp/uppercase_socket"
#define CLIENT_RUN_TIME 20

int main(int argc, char* argv[])
{
    int sock_fd;
    struct sockaddr_un addr;
    int client_id = 1;
    time_t start_time, current_time;
    
    if (argc > 1)
    {
        client_id = atoi(argv[1]);
        if (client_id < 1 || client_id > 2) { client_id = 1; }
    }

    sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr));

    printf("Client %d connected to server\n", client_id);
    
    char* messages[2][10] = {
        {
            "Hello from Client 1!\n",
            "This is second message from client 1\n",
            "Third message: testing 1 2 3\n",
            "Client 1 is still here!\n",
            "Almost done with client 1 messages\n",
            "Client 1 continues sending\n",
            "More data from client 1\n",
            "Network programming is fun!\n",
            "Unix sockets are powerful\n",
            "Final message from client 1\n"
        },
        {
            "Greetings from Client 2\n",
            "Second message from the second client\n",
            "Another test message here\n",
            "Client 2 checking in\n",
            "Penultimate message from client 2\n",
            "Client 2 keeps sending\n",
            "More messages from client 2\n",
            "Inter-process communication\n",
            "Socket programming example\n",
            "Last message from client 2!\n"
        }
    };
    
    // иниц случайных чисел
    srand(time(NULL) ^ getpid() ^ (client_id * 1000));
    
    start_time = time(NULL);
    int message_index = 0;
    
    while (1)
    {
        current_time = time(NULL);
        if (current_time - start_time >= CLIENT_RUN_TIME) { break; }
        
        char* msg = messages[client_id - 1][message_index % 10];
        
        write(sock_fd, msg, strlen(msg));
        
        message_index++;
        
        // случ задержка
        usleep(50000 + (rand() % 150000));
        
        // задержка дольше
        if (rand() % 10 == 0) { usleep(300000 + (rand() % 500000)); }
    }
    
    sleep(1);
    close(sock_fd);
    return 0;
}