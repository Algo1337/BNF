#if 0 
fsl main.c -o server src/user.c src/utils.c src/route.c src/db.c src/init.c
exit  
#endif 
 
#include "src/init.h" 
 
public bool validate_ip(string q) 
{ 
	if(!q) 
		return false; 
 
	int len = _str_len(q); 
	if(len == 0) 
		return false; 
 
	int min = _str_len("0.0.0.0"); 
	int max = _str_len("255.255.255.255"); 
 
	if(len < min || len > max) 
		return false; 
 
	int dots = count_char(q, '.'); 
	if(dots != 3) 
		return false; 
 
	int argc = 0; 
	sArr args = split_string(q, '.', &argc); 
	if(!args) 
		return false; 
 
	for(int i = 0; i < argc; i++) 
	{ 
		u8 num = str_to_int(args[i]); 
		if(num <= 0 || num > 255) 
			return false; 
	}
 
	pfree_array((array)args); 
	return true; 
} 
 
ptr _help_cmd(client_t client, _input cmd) 
{ 
	sock_write(client->sock, "working\n"); 
} 
 
ptr _geo_cmd(client_t client, _input cmd) 
{ 
	string ip = cmd.args[1]; 
 
	if(!validate_ip(ip)) 
	{ 
		sock_write(client->sock, "Invalid IPv4 argument provided...!\n"); 
		return NULL; 
	} 
 
	char rr[255]; 
	_sprintf(rr, "Looking up: %s\n", (ptr []){ip, NULL}); 
	sock_write(client->sock, rr); 
	string buffer = allocate(0, 255); 
	str_append_array(buffer, (string []){ 
		"/bin/curl ", 
		"-s ", 
		"https://ip-info.com/json?ip=", 
		ip, 
		" -o ", 
		"t.txt", 
		NULL 
	}); 
 
	_printf("Payload -> %s\n", buffer); 
	int argc = 0; 
	sArr r = split_string(buffer, ' ', &argc); 
 
	execute(r[0], (sArr)r); 
	_sleep(5); 
 
	fd_t file = open_file("t.txt", 0, 0); 
	if(!file) 
	{ 
		sock_write(client->sock, "Unable to geo locate the IP Address...!\n"); 
		return NULL; 
	} 
 
	int sz = file_content_size(file); 
	string buff = allocate(0, sz + 1); 
	int bytes = file_read(file, buff, sz); 
 
	int pos; 
	while((pos = find_char(buff, '"')) != -1) trim_char_idx(buff, pos);
	while((pos = find_char(buff, '\'')) != -1) trim_char_idx(buff, pos);
	while((pos = find_char(buff, ',')) != -1) trim_char_idx(buff, pos);
	while((pos = find_char(buff, '{')) != -1) trim_char_idx(buff, pos);
	while((pos = find_char(buff, '}')) != -1) trim_char_idx(buff, pos);
	trim_char_idx(buff, _str_len(buff));
	trim_char_idx(buff, 0);
	sock_write(client->sock, buff); 
	_pfree(buffer); 
	_pfree(buff); 
	_pfree(r); 
} 
 
public i8 entry() 
{ 
	uninit_mem(); 
	set_heap_sz(_HEAP_PAGE_ * 5); 
	init_mem(); 
 
	boatnet_t net = init_boatnet(NULL, 420); 
	if(!net) 
	{ 
		fsl_panic("Unable to start socket....!"); 
	} 
 
	append_cog(net->cogs, "help", 0, NULL, (ptr)_help_cmd); 
	append_cog(net->cogs, "geo", 2, "invalid arguments!\n", (ptr)_geo_cmd); 
	println("Starting server @ port: 420!"); 
 
	listener(net); 
    return 0; 
} 
