#include "src/init.h"

public i8 entry()
{
	uninit_mem();
	set_heap_sz(_HEAP_PAGE_ * 5);
	init_mem();

	boatnet_t net = init_boatnet(NULL, 420);
	println("Starting server @ port: 420!");
	listener(net);
    return 0;
}
