#include <stringManipulation.h>

namespace stringManipulation
{


uint32_t strlen(const char *s)
{
	uint32_t i= 0;

	while (s[i++] != 0);

	return i-1;
}

}
;