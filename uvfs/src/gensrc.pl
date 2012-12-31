#!/usr/bin/perl 

$i = 0;

printf("#include<stdio.h>\n");
printf("#include\"test.h\"\n");

for ($i=0; $i < 5000; $i++)
{
    printf("int test%d\(\)\n", $i);
    printf("{\n");
    printf("    printf(TEST);\n");
    printf("    printf(\"Number is \%d\");\n", $i);
    printf("}\n");
}
