#ifndef FT_NM_H
# define FT_NM_H

# include <elf_def.h>
// Default filename
# define FT_NM_DEFAULT "a.out"
// Default filters
# define FT_NM_OPTIONS 0

int ft_nm_perror(const char *prog, const char *file, t_elf_err err);
int ft_nm(const char *filename, t_elf_opt options, const char *prog);

#endif
