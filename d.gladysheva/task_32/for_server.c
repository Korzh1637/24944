#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/select.h>

#define SOCKET_PATH "/tmp/uppercase_socket"
int client_counter = 0;

int main()
{
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    // (sun_family) семейство адресов, всегда AF_UNIX для Unix domain sockets
    // (sun_path) путь к файлу сокета
    socklen_t client_len;
    char buffer[256];
    ssize_t bytes_read;
    
    fd_set read_fds, master_fds;
    int max_fd;
    int client_fds[10] = {0};
    int i;

    // создание сокета
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    // установка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    // привязка сокета к адресу
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // cлушаем соединение
    listen(server_fd, 10);

    printf("Server is working\n");

    // иниц множеств файловых дескрипторов
    FD_ZERO(&master_fds);
    FD_SET(server_fd, &master_fds);
    max_fd = server_fd;

    while (1)
    {
        read_fds = master_fds;
        
        // ожидание действий клиентов
        select(max_fd + 1, &read_fds, NULL, NULL, NULL)

        // проверка файловых дескрипторов
        for (i = 0; i <= max_fd; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == server_fd) // новый клиент
                {
                    // принятие соединения
                    client_len = sizeof(server_addr);
                    client_fd = accept(server_fd, (struct sockaddr*)&server_addr, &client_len);

                    // добавление клиента
                    FD_SET(client_fd, &master_fds);
                    if (client_fd > max_fd) { max_fd = client_fd; }
                    
                    // сохр дескриптор клиента
                    for (int j = 0; j < 10; j++)
                    {
                        if (client_fds[j] == 0)
                        {
                            client_fds[j] = client_fd;
                            printf("New client (fd = %d, number = %d)\n", client_fd, client_counter + 1);
                            client_counter++;
                            break;
                        }
                    }
                }
                else // работа с подключенным клиентом
                {
                    bytes_read = read(i, buffer, sizeof(buffer) - 1);

                    if (bytes_read <= 0) // удаление
                    {
                        printf("Client disconnected (fd = %d)\n", i);
                        close(i);
                        FD_CLR(i, &master_fds);
                        
                        for (int j = 0; j < 10; j++)
                        {
                            if (client_fds[j] == i) { client_fds[j] = 0; break; }
                        }
                    }
                    else // перевод в верх регистр и печать
                    {
                        buffer[bytes_read] = '\0';
                        for (int j = 0; j < bytes_read; j++) { buffer[j] = toupper(buffer[j]); }       
                        write(STDOUT_FILENO, buffer, bytes_read);
                    }
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}