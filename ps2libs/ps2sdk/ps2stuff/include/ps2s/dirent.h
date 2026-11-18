#ifndef ps2_dirent_h
#define ps2_dirent_h

#include "sifdev.h"

#ifndef NAME_MAX
#   define NAME_MAX	256
#endif

struct dirent {
      struct	sce_stat d_stat;
      char	d_name[256];
      void	*d_private;
};

int readdir(int fd, struct dirent *dirp, unsigned int count);

#endif // ps2_dirent_h
