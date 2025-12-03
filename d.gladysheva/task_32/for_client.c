#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_PATH "/tmp/uppercase_socket"

// Тексты для разных клиентов
const char* client_texts[] =
{
    "Hello from the first client!\nThis is a string with mixed case.\n",
    "Second client says hello!\nChecking server operation.\n",
    "Third client is online!\nTesting the system.\n",
    "Message from the fourth client!\nEverything works perfectly.\n",
    "Fifth client checking connection!\nEnd of transmission.\n",
    "Sixth client testing!\nMixed CaSE StRinG.\n",
    "Seventh client is working!\nAnother test text.\n",
    "Eighth client connected!\nChecking letter CASE.\n",
    "Ninth client transmitting!\nFinal message.\n",
    "Tenth client finishing!\nTest completed successfully.\n"
};

int main()
{
    int client_fd;
    struct sockaddr_un server_addr;
    
    // генерация id
    srand(time(NULL) ^ getpid());
    int client_id = rand() % 10;
    
    // создание сокета
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    // установка адреса сервера
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // подключение к серверу
    connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))

    printf("Client %d connected with server\n", client_id + 1);
    printf("Text from client %d:\n%s", client_id + 1, client_texts[client_id]);
    
    const char* text = client_texts[client_id];
    size_t text_len = strlen(text);
    
    for (size_t j = 0; j < text_len; j++)
    {
        // посимвольная отправка
        ssize_t bytes_sent = write(client_fd, &text[j], 1);
        // случ задержка между символами (0-100 мс)
        usleep((rand() % 100) * 1000);
    }

    sleep(1);
    close(client_fd);
    return 0;
}