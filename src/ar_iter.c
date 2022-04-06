#include <stdint.h>
#include <ar.h>

#include <libft.h>

#include <ar_iter.h>

static unsigned	ar_name_len(const char *name)
{
	unsigned	i;

	i = 0;
	while (name[i] != ' ' && name[i] != '\n' && name[i] != '/')
		i++;
	
	return i;
}

void		ar_iter(const t_ar_map *map, ar_iter_fun fun, void *fun_data)
{
	unsigned long	i;

	i = 0;
	while (i < map->count)
	{
		fun(map->members[i].name, ar_name_len(map->members[i].name),
			map->members[i].data, map->members[i].size, fun_data);
		i++;
	}
}