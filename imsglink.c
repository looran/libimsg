#include <sys/types.h>
#include <sys/uio.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>

#include <event.h>

#include "queue.h"
#include "imsg.h"
#include "imsglink.h"

void
imsglink_init(struct imsglink *link)
{
	if (socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, link->fds) == -1) 
		err(1, "socketpair"); 
}

int
imsglink_set_send(struct imsglink *link)
{
	if (close(link->fds[0]) < 0)
		return -1;
	imsg_init(&link->imsgs[1], link->fds[1]);
	return 0;
}

int
imsglink_set_recv(struct imsglink *link)
{
	if (close(link->fds[1]) < 0)
		return -1;
	imsg_init(&link->imsgs[0], link->fds[0]);
	return 0;
}

int
imsglink_send(struct imsglink *link, struct event_base *ev_base, int msgtype, void *data, int size)
{
	imsg_compose(&link->imsgs[1], msgtype, 0, 0, -1, data, size); 
	if (msgbuf_write(&link->imsgs[1].w) <= 0 && errno != EAGAIN)
		return -1;
	return 0;
}

void
_recv_cb(evutil_socket_t fd, short what, void *arg)
{
	struct imsglink *link;
	struct imsgbuf  *ibuf;
	struct imsg	imsg; 
	ssize_t         n, datalen; 

	link = arg;
	ibuf = &link->imsgs[0];
	
	if (((n = imsg_read(ibuf)) == -1 && errno != EAGAIN) || n == 0) { 
		/* handle socket error */ 
	} 
 
	for (;;) { 
		if ((n = imsg_get(ibuf, &imsg)) == -1) { 
			/* handle read error */ 
		} 
		if (n == 0)	/* no more messages */ 
			return;
		datalen = imsg.hdr.len - IMSG_HEADER_SIZE; 
		if (link->recv_cb == NULL)
			continue;
		if (link->recv_cb(link, imsg.hdr.type, imsg.data, datalen, link->usrdata) < 0)
			event_del(link->recv_ev);
	} 
}

void
imsglink_recv_setcb(struct imsglink *link, struct event_base *ev_base,
		    int (*recv_cb)(struct imsglink *link, int msgtype, void *data, int len, void *usrdata),
		    void *usrdata)
{
	struct event *ev;

	link->recv_cb = recv_cb;
	link->usrdata = usrdata;

	ev = event_new(ev_base, link->imsgs[0].fd, EV_READ|EV_PERSIST, _recv_cb, link);
	event_add(ev, NULL);
	link->recv_ev = ev;
}
