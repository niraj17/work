#include<stdio.h>
#include<stdarg.h>


int var_arg(char *first, ...)
{
	va_list list;
	char *p;

	va_start(list, first);

	while ((p = (char *)va_arg(list, char *)) != NULL)
		printf("Got argument: %s\n", p);

	va_end(list);
}

int main()
{

	var_arg("firsta", "second", "third");
}
