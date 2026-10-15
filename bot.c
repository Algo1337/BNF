#include <fsl.h>

#define IP_ADDR ""
#define PORT 420

int sock_set_read_timeout(sock_t s, int delay)
{
    if (!s || !s->fd)
        return 0;

    struct sleep_t t = { delay / 1000000, delay % 1000000 };
    long ret = __syscall__((long)s->fd, 1, 21, (long)&t, sizeof(t), -1, _SYS_SETSOCKOPT);
    if (ret < 0)
        return 1;

    return 0;
}

public sock_t connect_to_host()
{
    sock_t sock = allocate(0, sizeof(sock));
    sock->fd = create_socket(AF_INET, 2, 0);

    memzero(&sock->addr, sizeof(sock->addr));
    sock->addr.sin_family = AF_INET;
    sock->addr.sin_port = _htons(PORT);
    sock->addr.sin_addr.s_addr = 0;

    bool f = __syscall__(sock->fd, (long)&sock->addr, sizeof(sock->addr), 0, 0, 0, _SYS_CONNECT);
    if(f <= false)
        fsl_panic("Unable to connect to host!");

    sock->buff_len = 1024;
    sock_set_read_timeout(sock, 0);
    return sock;
}

public i8 entry()
{
    sock_t sock = connect_to_host();
    char data[sock->buff_len];
    
    while(1)
    {
        int bytes = __syscall__(sock->fd, (long)data, sock->buff_len, -1, -1, -1, _SYS_READ);
        if(bytes == -1)
            break;

        if(bytes == 0)
            continue;

        _printf("Server: %s\n", data);
        memzero(data, sock->buff_len);
    }
    
    return 0;
}