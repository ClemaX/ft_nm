#ifndef FILE_MAP_H
# define FILE_MAP_H

# include <sys/stat.h>
# include <sys/mman.h>

# include <stdio.h>
# include <unistd.h>

off_t	file_map(int fd, void**addr);
int		file_unmap(void **addr, off_t size);

#endif
