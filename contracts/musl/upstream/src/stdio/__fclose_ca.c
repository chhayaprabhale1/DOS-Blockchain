#include "stdio_impl.h"

int __dplose_ca(FILE *f)
{
	return f->close(f);
}
