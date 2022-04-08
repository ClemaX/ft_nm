#ifndef AR_ITER_H
# define AR_ITER_H

#include <elf_def.h>

#include <ar_map.h>

typedef void (ar_iter_fun(const char* name, unsigned long name_len,
    const void *data, unsigned long size, t_elf_opt options, const char *prog));

void    ar_iter(const t_ar_map *map, ar_iter_fun fun, t_elf_opt options,
    const char *prog);

#endif