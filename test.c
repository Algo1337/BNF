#include <fsl.h>

#include "src/init.h"

public string get_db()
{
    fd_t file = open_file("assets/users.db", 0, 0);
    if(!file)
        return NULL;

    int sz = file_content_size(file);
    if(sz <= 0)
    {
        fsl_warning("unable to get file size...!");
        return NULL;
    }

    string data = allocate(0, sz + 1);
    int bytes = file_read(file, data, sz);

    if(bytes <= 0)
    {
        fsl_warning("unable to read file...!");
        return NULL;
    }

    return data;
}

public users_t parse_db()
{

    string rdb = get_db();
    i32 line_count = 0;
    sArr lines = split_lines(rdb, &line_count);
    if(!lines)
        return NULL;

    users_t users = allocate(0, sizeof(_user) * line_count);
    int user_count = 0;
    for(int i = 0; i < line_count; i++)
    {
        if(!lines[i])
            continue;

        if(is_empty(lines[i]))
            continue;

        int len = str_len(lines[i]);
        if(lines[i][0] == '(' && lines[i][len - 2] == ')')
        {
            trim_char_idx(lines[i], 0);
            trim_char_idx(lines[i], len - 3);
            int pos = 0;
            while((pos = find_char(lines[i], '\'')) != -1) trim_char_idx(lines[i], pos);

            int argc = 0;
            sArr args = split_string(lines[i], ',', &argc);
            if(!args)
                continue;

            string username = args[0];
            string ip_addr = args[1];
            string password = args[2];
            users[user_count++] = init_user(username, ip_addr, password);

            pfree_array((array)args);
        }
    }

    if(user_count == 0)
        return NULL;

    return users;
}

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