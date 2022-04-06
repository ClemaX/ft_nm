#ifndef AR_ITER_H
# define AR_ITER_H

#include <ar_map.h>

typedef void (ar_iter_fun(const char* name, unsigned long name_len,
    const void *data, unsigned long size, void *fun_data));

void    ar_iter(const t_ar_map *map, ar_iter_fun fun, void *fun_data);

#endif