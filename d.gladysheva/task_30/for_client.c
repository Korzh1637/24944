#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/uppercase_socket"

int main()
{
    int client_fd;
    struct sockaddr_un server_addr;
    // (sun_family) семейство адресов, всегда AF_UNIX для Unix domain sockets
    // (sun_path) путь к файлу сокета
    
    // создание сокета
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    // установка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // подключение к серверу
    connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    const char *text = "Hello World!\nThis is a Test String with Mixed CASE.\nEnd of transmission.\n";
    
    // отправка текста
    ssize_t bytes_sent = write(client_fd, text, strlen(text));

    printf("Клиент разрывает соединение.\n");
    close(client_fd);
    return 0;
}