#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/uppercase_socket"

int main()
{
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    // (sun_family) семейство адресов, всегда AF_UNIX для Unix domain sockets
    // (sun_path) путь к файлу сокета

    socklen_t client_len; // длина socket адреса
    char buffer[256];
    ssize_t bytes_read;

    // создание сокета
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    // установка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    unlink(SOCKET_PATH); // на всякий

    // привязка сокета к адресу
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))

    // слушаем соединение
    listen(server_fd, 1);

    // принятие соединения
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

    printf("Клиент подключен.\n");

    // перевод текста в капс
    while ((bytes_read = read(client_fd, buffer, 256)) > 0)
    {
        for (int i = 0; i < bytes_read; i++) { buffer[i] = toupper(buffer[i]); }
        write(STDOUT_FILENO, buffer, bytes_read);
    }

    printf("\nСоединение разорвано.\n");
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}