#include <sys/types.h>
#include <event.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//#include "queue.h"

#if defined(__OpenBSD__)
#include <sys/queue.h>
#else
#include <bsd/sys/queue.h>
#endif

#include "imsg.h"
#include "imsglink.h"

#define DATA_VALUE 42

enum imsg_type { 
	IMSG_A_MESSAGE, 
}; 

int
_recv(struct imsglink *link, int msgtype, void *data, int len, void *usrdata)
{
	int idata;

	printf("Received %d bytes: msgtype=%d\n", len, msgtype);

	switch (msgtype) { 
	case IMSG_A_MESSAGE: 
		if (len < sizeof idata)
			exit(-1);
		memcpy(&idata, data, sizeof idata); 
		printf("_recv %d\n", idata);
		if (idata == DATA_VALUE)
			exit(0);
		else
			exit(-1);
		break; /* NOT REACHED */
	default:
		exit(-1);
	}
	return 0; /* NOT REACHED */
}

void
parent(struct imsglink *link)
{
	struct event_base *ev_base;

	ev_base = event_base_new();
	imsglink_set_recv(link);
	imsglink_recv_setcb(link, ev_base, _recv, NULL);
	event_base_dispatch(ev_base);
}

void
child(struct imsglink *link)
{
	struct event_base *ev_base;
	int val = DATA_VALUE;

	ev_base = event_base_new();
	imsglink_set_send(link);
	imsglink_send(link, ev_base, IMSG_A_MESSAGE, &val, sizeof(val));
	event_base_dispatch(ev_base);
}

int main(void)
{
	struct imsglink link;

	imsglink_init(&link);

	if (fork() == 0)
		child(&link);
	else
		parent(&link);
	return -1;
}

