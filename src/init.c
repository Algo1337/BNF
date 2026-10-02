#include "init.h"

#define SYS_SETSOCKOPT 54

#define SOL_SOCKET    1
#define SO_SNDTIMEO   21

struct timeval {
    long tv_sec;
    long tv_usec;
};

public boatnet_t init_boatnet(string ip, i32 port)
{
    boatnet_t b = allocate(0, sizeof(_boatnet));

    b->ip_addr = str_dup(ip);
    b->port = port;
    b->running = false;
    b->sock = listen_tcp(ip, port, 9999);
    b->users = parse_db();
    b->len = 0;


	for(int i = 0; b->users[i] != NULL; i++)
		b->len++;

	if(b->users == 0)
		fsl_warning("No users found in the database...!");

    return b;
}

public i32 find_user(boatnet_t b, string username)
{
	if(!b) return -1;

	for(int i = 0; i < b->len; i++)
	{
		if(str_cmp(b->users[i]->name, username)) {
			return i;
		}
	}

	return -1;
}

public bool append_user(boatnet_t b, user_t u)
{
	if(!b || !u)
		return false;

	b->users[b->len++] = u;
	b->users = reallocate(b->users, sizeof(_user *) * (b->len + 1));

	return true;
}

int sock_set_read_timeout(sock_t s, int delay)
{
    if (!s || !s->fd)
        return 0;

    struct timeval timeout = {
        delay / 1000000,
        delay % 1000000
    };

    long ret = __syscall__((long)s->fd, SOL_SOCKET, SO_SNDTIMEO, (long)&timeout, sizeof(timeout), -1, _SYS_SETSOCKOPT);

    if (ret < 0)
        return 1;

    return 0;
}

public fn handle_client(boatnet_t b, client_t u)
{
	u->listening = true;
	while(u->listening != false)
	{
		sock_write(u->sock, ">");
		char data[u->sock->buff_len];
		int bytes = __syscall__(u->sock->fd, (long)data, u->sock->buff_len, -1, -1, -1, _SYS_READ);
		if(bytes <= 0)
			continue;

		int sz = _str_len(data);
		if(mem_cmp(data, "help", 4))
		{
			sock_write(u->sock, "worked\r\n");
		}

		memzero(data, u->sock->buff_len);
	}

	u->listening = false;
}

public fn authorize_connection(boatnet_t b, sock_t client)
{
    sock_write(client, "Username: ");
    char USERNAME[client->buff_len];
    memzero(USERNAME, client->buff_len);
    int bytes = __syscall__(client->fd, (long)USERNAME, client->buff_len, -1, -1, -1, _SYS_READ);
    if(bytes <= 0)
    {
		sock_close(client);
		return;
    }

	strip_input(USERNAME, &bytes);
    sock_write(client, "Password: ");
    char PASSWORD[client->buff_len];
    memzero(PASSWORD, client->buff_len);
    bytes = __syscall__(client->fd, (long)PASSWORD, client->buff_len, -1, -1, -1, _SYS_READ);
    if(bytes <= 0)
	{
		sock_close(client);
		return;
	}

	strip_input(PASSWORD, &bytes);
	int pos = find_user(b, USERNAME);
	if(pos == -1)
	{
		// unable to find user...
		sock_write(client, "Invalid info");
		return;
	}

	user_t u = b->users[pos];
	_printf("[LOGIN ATTEMPT] ID: %d | '%s' : '%s'\n", (ptr)&pos, u->name, u->passwd);
	if(str_cmp(u->name, USERNAME) && str_cmp(u->passwd, PASSWORD))
	{
		sock_write(client, "success\r\n");
	} else
	{
		println("HERE 5");
		sock_write(client, "Invalid info\r\n");
		return;
	}

	u->sock = client;
	handle_client(b, u);
}

public fn listener(boatnet_t b)
{
	println("Listening for users....!");
	b->running = true;
    while(b->running != false)
    {
        sock_t client = sock_accept(b->sock, 1024);
        if(!client)
            continue;

		sock_set_read_timeout(client, 0);
        authorize_connection(b, client);
    }

    b->running = false;
}
