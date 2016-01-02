struct imsglink {
	struct imsgbuf imsgs[2];
	int fds[2];

	struct event *recv_ev;
	int (*recv_cb)(struct imsglink *, int, void *, int, void *);
	void *usrdata;
};

void imsglink_init(struct imsglink *);
int imsglink_set_send(struct imsglink *);
int imsglink_set_recv(struct imsglink *);
int imsglink_send(struct imsglink *, struct event_base *, int, void *, int);
void imsglink_recv_setcb(struct imsglink *, struct event_base *,
			 int (*recv_cb)(struct imsglink *, int, void *, int, void *),
			 void *);
