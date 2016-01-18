#include<stdio.h>
/*It seems simple enough to implement memmove; the overlap guarantee apparently requires only an additional test:*/

void *memmove(void *dest, void const *src, size_t n)
{
	register char *dp = dest;
	register char const *sp = src;

	/* copy from front*/
	if(dp < sp) {
		while(n-- > 0)
			*dp++ = *sp++;
	} else {
		/* copy from back */
		dp += n;
		sp += n;
		while(n-- > 0)
			*--dp = *--sp;
	}

	return dest;
}

