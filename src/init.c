#include "init.h"

boatnet_t _SERVER_ = NULL;
public boatnet_t init_boatnet(string ip, i32 port)
{
    boatnet_t b = allocate(0, sizeof(_boatnet));

    b->ip_addr = str_dup(ip);
    b->port = port;
    b->running = false;
    b->sock = listen_tcp(ip, port, 9999);
    b->users = parse_db();
    b->len = 0;
	b->cogs = init_cogs();
	b->cogs->len = 0;


	for(int i = 0; b->users[i] != NULL; i++)
		b->len++;

	if(b->users == 0)
		fsl_warning("No users found in the database...!");

	_SERVER_ = b;
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

_input parse_cmd(string query)
{
	if(!query)
		return (_input){0};

	if(count_char(query, ' ') <= 0)
		return (_input){query, 0, NULL};

	_input in;
	memzero(&in, sizeof(_input));

	in.args = split_string(query, ' ', &in.argc);
	in.cmd = in.args[0];

	return in;
}

public fn handle_client(client_t u)
{
	u->listening = true;
	while(u->listening != false)
	{
		if(sock_write(u->sock, "> ") <= 0)
			break;

		char data[u->sock->buff_len];
		int bytes = __syscall__(u->sock->fd, (long)data, u->sock->buff_len, -1, -1, -1, _SYS_READ);
		if(bytes == -1)
			break;

		if(bytes == 0)
			break;

		if(bytes < 2) {
			memzero(data, u->sock->buff_len);
			continue;
		}

		strip_input(data, &bytes);
		_input in = parse_cmd(data);

		if(mem_cmp(in.cmd, "q", 1))
			break;
		
		int pos = find_cmd(_SERVER_->cogs, in.cmd);
		if(pos == -1) {
			memzero(data, u->sock->buff_len);
			continue;
		}

		if(_SERVER_->cogs->cogs[pos]->arg_count > 0)
		{
			if(_SERVER_->cogs->cogs[pos]->arg_count > in.argc)
			{
				sock_write(u->sock, _SERVER_->cogs->cogs[pos]->arg_error);
			}
		}

		((void (*)(client_t, _input))_SERVER_->cogs->cogs[pos]->handler)(u, in);
		memzero(data, u->sock->buff_len);
	}

	u->listening = false;
	uninit_mem();
	asm("ret");
}

public fn authorize_connection(/* boatnet_t b, sock_t client */)
{
    register ptr *args asm("r13");

	boatnet_t b = (boatnet_t)args[0];
	sock_t client = (sock_t)args[1];
	init_mem();

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
	_printf("[LOGIN ATTEMPT] UID: #%d | '%s' -> '%s'\n", (ptr)&pos, u->name, u->passwd);
	if(str_cmp(u->name, USERNAME) && str_cmp(u->passwd, PASSWORD))
	{
		sock_write(client, "success\r\n");
	} else
	{
		sock_write(client, "Invalid info\r\n");
		return;
	}

	u->sock = client;
	handle_client(u);
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
        // authorize_connection(b, client);

		void *n[] = {b, client};
		run_thread(authorize_connection, n);
    }

    b->running = false;
}
