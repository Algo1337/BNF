#include "init.h"

public fn strip_input(string buffer, int *sz)
{
	if(buffer[*sz - 1] == '\r' || buffer[*sz - 1] == '\n')
		buffer[*sz - 1] = '\0', (*sz)--;

	if(buffer[*sz - 1] == '\r' || buffer[*sz - 1] == '\n')
		buffer[*sz - 1] = '\0', (*sz)--;
}