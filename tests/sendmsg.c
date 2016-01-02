/* based on http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/ibuf_add.3 */

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

#define DATA_VALUE 42

enum imsg_type { 
	IMSG_A_MESSAGE, 
	IMSG_MESSAGE2 
}; 

int
dispatch_imsg(struct imsgbuf *ibuf) 
{ 
	struct imsg	imsg; 
	ssize_t         n, datalen; 
	int		idata; 
 
	if (((n = imsg_read(ibuf)) == -1 && errno != EAGAIN) || n == 0) { 
		/* handle socket error */ 
	} 
 
	for (;;) { 
		if ((n = imsg_get(ibuf, &imsg)) == -1) { 
			/* handle read error */ 
		} 
		if (n == 0)	/* no more messages */ 
			return -1;
		datalen = imsg.hdr.len - IMSG_HEADER_SIZE; 
 
		switch (imsg.hdr.type) { 
		case IMSG_A_MESSAGE: 
			if (datalen < sizeof idata) { 
				/* handle corrupt message */ 
			} 
			memcpy(&idata, imsg.data, sizeof idata); 
			/* handle message received */ 
			printf("dispatch_imsg %d\n", idata);
			if (idata == DATA_VALUE)
				return 0;
			else
				return -1;
			break; 
		} 
 
		imsg_free(&imsg); 
	} 
	return -1;
}


int
parent_main(struct imsgbuf *ibuf)
{
	return dispatch_imsg(ibuf);
}
 
int 
child_main(struct imsgbuf *ibuf) 
{ 
	int	idata; 
	idata = DATA_VALUE; 
	imsg_compose(ibuf, IMSG_A_MESSAGE, 
		0, 0, -1, &idata, sizeof idata); 
	if (msgbuf_write(&ibuf->w) <= 0 && errno != EAGAIN) { 
		return -1;
	}
	return 0;
}

int main(void)
{
	struct imsgbuf	parent_ibuf, child_ibuf; 
	int		imsg_fds[2]; 

	if (socketpair(AF_UNIX, SOCK_STREAM, PF_UNSPEC, imsg_fds) == -1) 
		err(1, "socketpair"); 
	 
	switch (fork()) { 
	case -1: 
		err(1, "fork"); 
	case 0: 
		/* child */ 
		close(imsg_fds[0]); 
		imsg_init(&child_ibuf, imsg_fds[1]); 
		exit(child_main(&child_ibuf)); 
	} 
	 
	/* parent */ 
	close(imsg_fds[1]); 
	imsg_init(&parent_ibuf, imsg_fds[0]); 
	exit(parent_main(&parent_ibuf));
}

