#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/time.h>

#define SOCKET_PATH "/tmp/uppercase_socket"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 2
#define SERVER_RUN_TIME 15

int main()
{
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr = {0};
    fd_set readfds, allfds;
    int clients[MAX_CLIENTS] = {0};
    int max_fd = server_fd;
    char buf[BUFFER_SIZE];
    int total_clients = 0, message_count = 0;
    
    struct timeval server_start_time, first_msg_time, last_msg_time;
    gettimeofday(&server_start_time, NULL);
    int first_msg_received = 0;

    unlink(SOCKET_PATH);
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 2);

    FD_ZERO(&allfds);
    FD_SET(server_fd, &allfds);

    printf("Server will run for %d seconds\n", SERVER_RUN_TIME);

    while (total_clients < 2)
    {
        fd_set accept_fds;
        struct timeval timeout;
        struct timeval current_time;
        
        gettimeofday(&current_time, NULL);
        long elapsed = current_time.tv_sec - server_start_time.tv_sec;
        
        // проверка ост времени сервера
        if (elapsed >= SERVER_RUN_TIME)
        {
            close(server_fd);
            unlink(SOCKET_PATH);
            return 0;
        }
        
        FD_ZERO(&accept_fds);
        FD_SET(server_fd, &accept_fds);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int ready = select(server_fd + 1, &accept_fds, NULL, NULL, &timeout);
        
        if (ready > 0 && FD_ISSET(server_fd, &accept_fds))
        {
            int fd = accept(server_fd, NULL, NULL);
            if (fd != -1)
            {
                clients[total_clients] = fd;
                FD_SET(fd, &allfds);
                if (fd > max_fd) max_fd = fd;
                total_clients++;
                printf("Client %d connected (fd = %d)\n", total_clients, fd);
            }
        }
    }


    while (1)
    {
        struct timeval current_time;
        gettimeofday(&current_time, NULL);
        long elapsed = current_time.tv_sec - server_start_time.tv_sec;
        
        if (elapsed >= SERVER_RUN_TIME)
        {
            printf("\nServer time expired (%d seconds)\n", SERVER_RUN_TIME);
            break;
        }
        
        readfds = allfds;
        FD_CLR(server_fd, &readfds);
        
        struct timeval select_timeout;
        select_timeout.tv_sec = 0;
        select_timeout.tv_usec = 100000;
        
        int ready = select(max_fd + 1, &readfds, NULL, NULL, &select_timeout);
        
        if (ready < 0) continue;
        
        // проверка клиентских сокетов
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int fd = clients[i];
            if (fd == 0) continue;
            
            if (ready > 0 && FD_ISSET(fd, &readfds))
            {
                int n = read(fd, buf, BUFFER_SIZE);
                if (n <= 0)
                {
                    printf("Client %d disconnected\n", i + 1);
                    close(fd);
                    FD_CLR(fd, &allfds);
                    clients[i] = 0;
                }
                else if (n > 0)
                {
                    message_count++;
                    struct timeval msg_time;
                    gettimeofday(&msg_time, NULL);
                    
                    // время от старта сервера
                    long seconds = msg_time.tv_sec - server_start_time.tv_sec;
                    long microseconds = msg_time.tv_usec - server_start_time.tv_usec;
                    
                    if (microseconds < 0)
                    {
                        seconds--;
                        microseconds += 1000000L;
                    }

                    if (!first_msg_received)
                    {
                        first_msg_time = msg_time;
                        first_msg_received = 1;
                    }
                    last_msg_time = msg_time;
                    
                    char time_buf[32];
                    int time_len = snprintf(time_buf, sizeof(time_buf), "[%ld.%03ld] Client %d: ", seconds,
                                            microseconds / 1000, i + 1);
                    write(1, time_buf, time_len);
                    
                    for (int j = 0; j < n; j++) { buf[j] = toupper(buf[j]); }
                    write(1, buf, n);
                    
                    if (n > 0 && buf[n-1] != '\n') { write(1, "\n", 1); }
                }
            }
        }
    }

    long seconds = last_msg_time.tv_sec - first_msg_time.tv_sec;
    long microseconds = last_msg_time.tv_usec - first_msg_time.tv_usec;
    
    if (microseconds < 0)
    {
        seconds--;
        microseconds += 1000000L;
    }
    
    printf("Server ran for: %d seconds\n", SERVER_RUN_TIME);
    printf("Time between first and last message: %ld.%06ld seconds\n", seconds, microseconds);
    
    for (int i = 0; i < MAX_CLIENTS; i++) { if (clients[i] > 0) { close(clients[i]); } }
    
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}