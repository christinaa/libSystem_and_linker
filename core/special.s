/*
 * core: ExtnsShim.c
 * Copyright (c) 2012 Christina Brooks
 *
 * Those weird '$NOCANCEL' symbols.
 */

#define Entry(x) .globl _##x; \
_##x:

#define Invoke(x) b _##x


.align 2
.text

Entry(select$DARWIN_EXTSN)
Invoke(select)

Entry(select$NOCANCEL)
Invoke(select)

Entry(connect$NOCANCEL)
Invoke(connect)

Entry(recvmsg$NOCANCEL)
Invoke(recvmsg)

Entry(sendmsg$NOCANCEL)
Invoke(sendmsg)

Entry(close$NOCANCEL)
Invoke(close)

Entry(fcntl$NOCANCEL)
Invoke(fcntl)