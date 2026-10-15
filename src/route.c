#include "init.h"

public Cogs init_cogs()
{
    Cogs c = allocate(0, sizeof(_cogs));

    c->cogs = allocate(0, sizeof(_cog *) * 1);
    c->len = 0;

    return c;
}

public bool append_cog(Cogs c, string name, i32 max_argc, string invalid_arg_err, ptr *handler)
{
    if(!c)
        return false;

    cog_t cog = allocate(0, sizeof(_cog));
    cog->name = str_dup(name);
    cog->arg_count = max_argc;
    cog->arg_error = str_dup(invalid_arg_err);
    cog->handler = handler;

    c->cogs[c->len++] = cog;
    c->cogs = reallocate(c->cogs, sizeof(_cog *) * (c->len + 1));

    return true;
}

public bool remove_cog(Cogs c, string name)
{
    if(!c)
        return false;

    for(int i = 0; i < c->len; i++)
    {
        if(str_cmp(c->cogs[i]->name, name))
        {
            _pfree(c->cogs[i]);
            c->cogs[i] = NULL;

            for(int j = i; j < c->len - 1; j++)
                c->cogs[j] = c->cogs[j + 1];

            c->len--;
            return true;
        }
    }

    return false;
}

public fn cog_Destruct(cog_t c)
{
    if(!c)
        return;

    if(c->name)
        _pfree(c->name);

    if(c->arg_error)
        _pfree(c->arg_error);

    _pfree(c);
}

public fn cogs_Destruct(Cogs c)
{
    if(!c)
        return;

    if(c->len > 0 && c->cogs)
    {
        for(int i = 0; i < c->len; i++)
            cog_Destruct(c->cogs[i]);
    
        _pfree(c->cogs);
    }

    _pfree(c);
}

public i32 find_cmd(Cogs c, string name)
{
    if(!c)
        return -1;

    for(int i = 0; i < c->len; i++)
    {
        if(str_cmp(c->cogs[i]->name, name))
            return i;
    }

    return -1;
}