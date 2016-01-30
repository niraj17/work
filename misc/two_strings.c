/*
 * http://www.geeksforgeeks.org/find-the-smallest-window-in-a-string-containing-all-characters-of-another-string/
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_WIN  8192

// Returns false if no valid window is found. Else returns 
// true and updates minWindowBegin and minWindowEnd with the 
// starting and ending position of the minimum window.
int minWindow(const char* S, const char *T, 
               int *minWindowBegin, int *minWindowEnd) {
  int sLen = strlen(S);
  int tLen = strlen(T);
  int needToFind[256] = {0};
  int i = 0;	
 
  for (i = 0; i < tLen; i++)
    needToFind[T[i]]++;
 
  int hasFound[256] = {0};
  int minWindowLen = MAX_WIN;
  int count = 0;
  int begin = 0, end = 0;
  for (begin = 0, end = 0; end < sLen; end++) {
    // skip characters not in T
    if (needToFind[S[end]] == 0) continue;
    hasFound[S[end]]++;
    if (hasFound[S[end]] <= needToFind[S[end]])
      count++;
 
    // if window constraint is satisfied
    if (count == tLen) {
      // advance begin index as far right as possible,
      // stop when advancing breaks window constraint.
      while (needToFind[S[begin]] == 0 ||
            hasFound[S[begin]] > needToFind[S[begin]]) {
        if (hasFound[S[begin]] > needToFind[S[begin]])
          hasFound[S[begin]]--;
        begin++;
      }
 
      // update minWindow if a minimum length is met
      int windowLen = end - begin + 1;
      if (windowLen < minWindowLen) {
        *minWindowBegin = begin;
        *minWindowEnd = end;
        minWindowLen = windowLen;
      } // end if
    } // end if
  } // end for
  return (count == tLen) ? 1 : 0;
}
 
int main()

{
	int start, end;
	char *s1 = "This oe is ll a hello";
	char *s2 = "oell";

	minWindow(s1, s2, &start, &end);
	printf("window is %d %d\n", start, end);
}
