#ifndef AR_DUMP_H
# define AR_DUMP_H

# include <elf_def.h>

int ar_ident(const void *data, unsigned long size);

int ar_dump(const void *data, unsigned long size, t_elf_opt options,
    const char *prog);

#endif