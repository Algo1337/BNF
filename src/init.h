#pragma once

#include <fsl.h>

#define SOL_SOCKET    1
#define SO_SNDTIMEO   21

struct timeval {
    long tv_sec;
    long tv_usec;
};

typedef struct 
{
    string  cmd;
    int     argc;
    sArr    args;
} _input;

typedef struct
{
    string  name;
    int     arg_count;
    string  arg_error;
    ptr     handler;
} _cog;

typedef _cog *cog_t;
typedef _cog **cogs_t;

typedef struct 
{
    cogs_t  cogs;
    i32     len;    
} _cogs;

typedef _cogs *Cogs;

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
    /* Socket Info */
    string		ip_addr;
    int			port;
    bool		running;
    sock_t		sock;
    
    /* Database */
    users_t		users;
    len_t		len;

    /* Cogs */
    Cogs        cogs;
} _boatnet;

typedef _boatnet *boatnet_t;

extern boatnet_t _SERVER_;

// init.c
public boatnet_t    init_boatnet(string ip, i32 port);
public i32          find_user(boatnet_t b, string username);
public bool         append_user(boatnet_t b, user_t u);
public fn           handle_client(client_t u);
public fn           authorize_connection(/* boatnet_t b, sock_t client */);
public fn           listener(boatnet_t b);

// db.c
public string       get_db();
public users_t      parse_db();

// user.c
public user_t       init_user(string name, string ip_addr, string passwd);
public bool         is_passwd_valid(user_t u, string q);
public fn           user_destruct(user_t u);

// utils.c
public fn           strip_input(string buffer, int *sz);

// route.c
public Cogs         init_cogs();
public bool         append_cog(Cogs c, string name, i32 max_argc, string invalid_arg_err, ptr *handler);
public bool         remove_cog(Cogs c, string name);
public fn           cog_Destruct(cog_t c);
public fn           cogs_Destruct(Cogs c);
public i32          find_cmd(Cogs c, string name);