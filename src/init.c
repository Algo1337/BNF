#include "init.h"


public boatnet_t init_boatnet(string ip, i32 port)
{
    boatnet_t b = allocate(0, sizeof(_boatnet));

    b->ip_addr = str_dup(ip);
    b->port = port;
    b->running = false;
    b->sock = listen_tcp(ip, port, 9999);
    b->users = allocate(sizeof(_user *), 1);
    b->len = 0;

    return b;
}

public i32 find_user(boatnet_t b, string username)
{
	if(!b || !username || b->len == 0) return -1;

	for(int i = 0; i < b->len; i++)
	{
		if(str_cmp(b->users[i]->name, username))
			return i;
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

public fn handle_client(boatnet_t b, client_t u)
{
	u->listening = true;
	while(u->listening != false)
	{
		sock_write(u->sock, ">");
		string data = sock_read(u->sock);
		if(!data)
			continue;

		int sz = __get_size__(data);
		if(mem_cmp(data, "help", 4))
		{
			sock_write(u->sock, "worked");
		}

		_pfree(data);
	}

	u->listening = false;
}

public fn authorize_connection(boatnet_t b, sock_t client)
{
    if(!b || !client)
        return;

    sock_write(client, "Username: ");
    string username = sock_read(client);
    if(!username)
    {
		sock_close(client);
		return;
    }

    sock_write(client, "Password ");
    string password = sock_read(client);
    if(!password)
    {
		sock_close(client);
		_pfree(username);
		return;
    }

	int pos = 0;
	if((pos = find_user(b, username)) == -1)
	{
		// unable to find user...
		sock_write(client, "Invalid info");
		_pfree(username);
		_pfree(password);
		return;
	}

	user_t u = b->users[pos];
	if(!str_cmp(u->name, username) || !str_cmp(u->passwd, password))
	{
		sock_write(client, "Invalid info");
		_pfree(username);
		_pfree(password);
		return;
	}

	user_destruct(u);
	_pfree(username);
	_pfree(password);
	u->sock = client;

	handle_client(b, u);
}

public fn listener(boatnet_t b)
{
	b->running = true;
    while(b->running != false)
    {
        sock_t client = sock_accept(b->sock, 1024);
        if(!client)
            continue;

        authorize_connection(b, client);
    }

    b->running = false;
}
