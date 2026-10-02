#include "src/init.h"

public i8 entry()
{
	boatnet_t net = init_boatnet(NULL, 420);
	listener(net);
    return 0;
}
