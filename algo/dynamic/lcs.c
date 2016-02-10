
/*
 http://www.geeksforgeeks.org/dynamic-programming-set-4-longest-common-subsequence/
*/
#include<stdio.h>
#include<string.h>

#define MAX(x,y) (x>y?x:y)

int lcs(char *s1, char *s2)
{
	int i, j;
	int m = strlen(s1);
	int n = strlen(s2);

	int l[m][n];


	for(i=0; i < m; i++) {
		for(j=0; j < n; j++) {
			// initilization
			if (i==0 || j == 0)	l[i][j] = 0;
			else if (s1[i] == s2[j]) {
				l[i][j] = l[i-1][j-1] + 1;
			} else {
				l[i][j] = MAX(l[i][j-1], l[i-1][j]);
			}
		}
	}

	return l[m-1][n-1];
}


int main()
{
	char *s1 = "I am niraj 3";
	char *s2 = "n a i a r a a j 3";

	int r = lcs(s1, s2);

	printf("LCS is %d\n", r);
}
