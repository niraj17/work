/*
http://www.geeksforgeeks.org/count-possible-paths-top-left-bottom-right-nxm-matrix/
*/

#include<stdio.h>


int count_no_of_path(int r, int c)
{
	int i,j;
	int  paths[r][c];

	for(i=0; i < r; i++)
		paths[i][0] = 1;
	for(i=0; i < r; i++)
		paths[0][i] = 1;


	for(i = 1; i < r; i++) {
		for(j=1; j < c;j++) {
			paths[i][j] = paths[i][j-1] + paths[j][i-1];
		}
	}

	return paths[r-1][c-1];
}
int main()
{
	int c = count_no_of_path(3,3);
	printf("No of path is %d\n", c);
}
