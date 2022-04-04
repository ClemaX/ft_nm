#ifndef FILE_MAP_H
# define FILE_MAP_H

# include <sys/types.h>

// System call failed
# define FILE_ESYS -1
// File is a directory
# define FILE_EISDIR -2
// File is irregular
# define FILE_EIRREG -3

off_t	file_map(int fd, void **addr);
int		file_unmap(void **addr, off_t size);

#endif
