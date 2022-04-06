#ifndef AR_MAP_H
# define AR_MAP_H

typedef struct  s_ar_member
{
	const char		*name;
	const void		*data;
	unsigned long	size;
}               t_ar_member;

typedef struct  s_ar_map
{
	const struct ar_hdr	*names_extended;
	unsigned long		names_extended_size;
	t_ar_member			*members;
	unsigned long		count;
}               t_ar_map;

int 	ar_ident(const void *data, unsigned long size);

int		ar_map(t_ar_map *map, const void *data, unsigned long size);
void	ar_unmap(t_ar_map *map);

#endif