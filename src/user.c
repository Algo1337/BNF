#include "init.h"

public fn user_destruct(user_t u)
{
	if(u->name)
		_pfree(u->name);

	if(u->ip_addr)
		_pfree(u->ip_addr);

	if(u->passwd)
		_pfree(u->passwd);

	_pfree(u);
}

public user_t init_user(string name, string ip_addr, string passwd)
{
    if(!name || !ip_addr || !passwd)
        return NULL;

    user_t t = allocate(0, sizeof(_user));
    t->name = str_dup(name);
    t->ip_addr = str_dup(ip_addr);
    t->passwd = str_dup(passwd);

    return t;
}

public bool is_passwd_valid(user_t u, string q)
{
    return str_cmp(u->passwd, q);
}