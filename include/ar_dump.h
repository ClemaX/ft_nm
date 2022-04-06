#ifndef AR_DUMP_H
# define AR_DUMP_H

int ar_ident(const void *data, unsigned long size);

int ar_dump(const char *prog, const void *data, unsigned long size);

#endif