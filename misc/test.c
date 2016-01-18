#include<stdio.h>

#ifdef TEST
int flags;

int flag_a = 0x01;
int flag_b = 0x02;
int flag_c = 0x04;

/* I want to flip 'flag_b' without touching 'flag_a' or 'flag_c' */
flags ^= flag_b;

/* I want to set 'flag_b' */
flags |= flag_b;

/* I want to clear (or 'reset') 'flag_b' */
flags &= ~flag_b;

/* I want to test 'flag_b' */
int b_is_set = (flags & flab_b) != 0;
#endif

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))


typedef struct _tt {
	int c;
	int b;
	char h;
} tt;

/*merge c bits from b into a*/
int bit_merge(int a, int b, int c, int d)
{
	int mask = (1<< (c)) -1;
	printf("%x\n", mask);

	int r = (a & mask) | (b & ~mask);
	printf("%x\n", r);
}

/* round down to nearst multiple of power of 2*/
int round_down(int n, int r)
{
	return ((n >> r) <<) ;
}

int main()
{
	int a= 10, b= 20, diff;

	diff = &b - &a;

	printf("diff is %d\n",diff);

	bit_merge(a, b, 10, 16);

	printf("offsetof = %d\n", OFFSETOF(tt, h));

	printf("Round down of %d is %d\n",5125,  round_down(5125, 512));
}
