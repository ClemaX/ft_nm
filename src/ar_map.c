#include <stdlib.h>
#include <stdint.h>
#include <ar.h>

#include <byteswap.h>
#include <libft.h>

#include <ar_map.h>

#ifdef __BYTE_ORDER
# if __BYTE_ORDER == __LITTLE_ENDIAN
# define btoh32(x) ((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8)	\
   | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24))
# elif __BYTE_ORDER == __BIG_ENDIAN
# define btoh32(x) (x)
# endif
#endif

long		ar_size(const struct ar_hdr *header)
{
	const unsigned	str_len = sizeof(header->ar_size) / sizeof(*header->ar_size);
	long			size;
	unsigned		i;

	if (ft_strncmp(ARFMAG, header->ar_fmag, sizeof(ARFMAG) - 1) == 0)
	{
		size = 0;
		i = 0;
		while (i < str_len && ft_isdigit(header->ar_size[i]))
		{
			size = size * 10 + header->ar_size[i] - '0';
			i++;
		}
		while (i < str_len && header->ar_size[i] == ' ')
			i++;
		if (i != str_len)
		{
			ft_dprintf(2, "ar: Invalid header: ar_size length: %u, offending characters: %.*s\n", i, str_len - i, header->ar_size + i);
			size = -1;
		}
	}
	else
	{
		ft_dprintf(2, "ar: invalid file magic byte: '%.2s'!\n", header->ar_fmag);
		size = -1;
	}
	return size;
}

static const char	*ar_map_name(t_ar_map *map, const struct ar_hdr *header)
{
	unsigned long	name_offset;
	const char		*name;
	unsigned		i;

	name = header->ar_name;
	if (*name == '/')
	{
		i = 1;
		name_offset = 0;
		while (i < sizeof(header->ar_name) && ft_isdigit(name[i]))
			name_offset = name_offset * 10 + name[i++] - '0';
		while (i < sizeof(header->ar_name) && name[i] == ' ')
			i++;
		if (i == sizeof(header->ar_name) && name_offset < map->names_extended_size)
			name = (const char *)(map->names_extended + 1) + name_offset;
		else
			name = NULL;
	}
	return (name);
}

static long	ar_count_members(const struct ar_hdr *header,
	const void *end)
{
	long	size;
	long	count;

	count = 0;
	size = 0;
	while (size >= 0 && (const void *)header < end)
	{
		size = ar_size(header);
		if (size >= 0)
		{
			header = (const void *)(header + 1) + size;
			count++;
		}
	}
	if (header != end)
		count = -1;
	return (count);
}

static int	ar_map_members(t_ar_map *map, const struct ar_hdr *header,
	const void *end)
{
	unsigned long		size;
	unsigned long		i;
	const char			*name;
	int					err;

	map->count = ar_count_members(header, end);
	map->members = NULL;
	err = map->count < 0;
	if (err == 0 && map->count != 0)
	{
		map->members = malloc(sizeof(*map->members) * map->count);
		err = map->members == NULL;
		i = 0;
		while (err == 0 && i < map->count && (const void*)header < end)
		{
			size = ar_size(header);
			err = size < 0 || (const void*)(header + 1) + size > end;

			if (err == 0)
			{
				name = ar_map_name(map, header);
				err = name == NULL;
				if (err == 0)
				{
					map->members[i] = (t_ar_member){name, (const void*)(header + 1), size};
					//ft_dprintf(2, "Mapped symbol[%lu] = {%.16s, %p, %lx}\n", i, map->members[i].name, map->members[i].data, map->members[i].size);
					header = (const void*)(header + 1) + map->members[i].size;
					i++;
				}
				else
					ft_dprintf(2, "Could not map symbol[%lu]: header: %p, end: %p\n", i, header, end);
			}
		}	
	}
	return err;
}

int			ar_map(t_ar_map *map, const void *data, unsigned long size)
{
	const void	*const	end = data + size;
	const struct ar_hdr	*header;
	int					err;
	long				member_size;

	err = size < SARMAG || ft_strncmp(ARMAG, (const char*)data, SARMAG) != 0;
	if (err == 0)
	{
		header = (const struct ar_hdr*)(data + SARMAG);
		member_size = ar_size(header);
		err = member_size < 0;
		if (err == 0)
		{
			map->names_extended = NULL;
			if (*header->ar_name == '/')
			{
				header = (const void *)(header + 1) + member_size;
				err = (const void *)header >= end;
				if (err == 0)
				{
					if (*header->ar_name == '/' && header->ar_name[1] == '/')
					{
						member_size = ar_size(header);
						err = member_size < 0 || (const void *)(header + 1) + member_size > end;
						if (err == 0)
						{
							map->names_extended = header;
							map->names_extended_size = member_size;
							header = (const void *)(header + 1) + member_size;
						}
					}
				}
			}
			if (err == 0)
				err = ar_map_members(map, header, end);
		}
	}

	return	err;
}

void		ar_unmap(t_ar_map *map)
{
	free(map->members);
	map->names_extended = NULL;
	map->members = NULL;
}