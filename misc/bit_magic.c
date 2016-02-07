
/*
int i = 0xabcdef12; 32bit integer
char c = 'A';
int p = ; 0-3


char *cc = &i;
cc = cc + p;
*cc = c;

cal you call back ?. when I call, it seems, I can't hear you properly.
Let me call you then. I will call now.



unsigned int k = (unsigned int ) c;
k = k << p * 8;
unsigned int j = ~ (0xFF << p * 8);
            
i = i & j;
i = i & k; 

-------------------------
*/

/*
 REMEMBER - you have four bitwise operator + you can do bitshift  !!!!!!
*/

#include<stdio.h>

int print_bits(unsigned int a)
{
	unsigned int i = 0, t = 1;

	t = (1 << 31);
	for(i = 0; i < 32 ; i++) {
		if (a & t) printf("1");
		else printf("0");

		t = t >> 1;
	}
}


int merge_bits(unsigned int a, unsigned int b, unsigned int mask)
{
	return ( (a & ~mask) | (b & mask) );

}

int main()
{
	unsigned int z = 0x12FF;
	unsigned int q = 0x00FF;

	print_bits(8);
	printf("\n");

	unsigned r = merge_bits(z, q, 0);
	print_bits(r);
	printf("\n");
}
