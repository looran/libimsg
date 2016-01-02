libimsg
=======

imsg IPC library from OpenBSD.

(See http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/ibuf_add.3)

imsglink.{c,h} is an addition, and exposes a high-level API integrated with libevent.

## Examples

Using imsglink wrapper with libevent : [tests/imsglink_sendrecv.c](tests/imsglink_sendrecv.c)

Using imsg directly : [tests/sendmsg.c](tests/sendmsg.c)

## Install

```bash
make
sudo make install
```
