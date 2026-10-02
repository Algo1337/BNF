#include <fsl.h>

#include "src/init.h"

public i8 entry()
{
    users_t users = parse_db();
    if(!users)
        fsl_panic("unable to get users");

    for(int i = 0; users[i] != NULL; i++)
    {
        _printf("Username: %s | IP Addr: %s | Password: %s\n", users[i]->name, users[i]->ip_addr, users[i]->passwd);
    }
    return 0;
}