#ifndef FT_NM_H
# define FT_NM_H

#include <elf_def.h>

int ft_nm_perror(const char *prog, const char *file, t_elf_err err);
int ft_nm(const char *prog, const char *filename);

#endif
