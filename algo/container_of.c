#include<stdio.h>

#define offset_of(type, member) (&(((type *)0)->member))

#define container_of (ptr, type, member) {\
	typeof (type) *tmp = ptr;
	tmp = tmp - offsetof(type, member) }
	
