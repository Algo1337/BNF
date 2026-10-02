#pragma once

#include <fsl.h>

typedef struct
{
    string		name;
    string		ip_addr;
    string		passwd;

    sock_t		sock;
    bool		listening;
} _user;

typedef _user *client_t;
typedef _user *user_t;
typedef _user **users_t;

typedef struct
{
    string		ip_addr;
    int			port;
    bool		running;
    sock_t		sock;
    users_t		users;
    len_t		len;
} _boatnet;

typedef _boatnet *boatnet_t;

// init.c
public boatnet_t    init_boatnet(string ip, i32 port);
public i32          find_user(boatnet_t b, string username);
public bool         append_user(boatnet_t b, user_t u);
public fn           handle_client(boatnet_t b, client_t u);
public fn           authorize_connection(boatnet_t b, sock_t client);
public fn           listener(boatnet_t b);

// db.c
public string get_db();
public users_t parse_db();

// user.c
public user_t       init_user(string name, string ip_addr, string passwd);
public bool         is_passwd_valid(user_t u, string q);
public fn           user_destruct(user_t u);

// utils.c
public fn strip_input(string buffer, int *sz);