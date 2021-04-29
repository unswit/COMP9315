#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


//typedef struct intset{
//  int num;
//  int arr[200];
//}Intset;

int compare(const void *p, const void *q)
{
  int l = *(const int *)p;
    int r = *(const int *)q;

    // both odd, put the greater of two first.
    if ((l&1) && (r&1))
        return (r-l);

    // both even, put the smaller of two first
    if ( !(l&1) && !(r&1) )
        return (l-r);

    // l is even, put r first
    if (!(l&1))
        return 1;

    // l is odd, put l first
    return -1;
}

int main ()
{

  /*
  char* str = "{1,2,3,4,5,6,71    }";
  int arr[200];
	int num =0;

	char tempstr[strlen(str)];
	int numchars=0;


  for (int i = 0; str[i] ; i++) {
    if (isdigit(str[i]) || str[i] == ',') {
      tempstr[numchars++] = str[i];
    }
  }

  const char s[2] = ",";
  char *token;
  token = strtok(tempstr, s);
  while( token != NULL ) {
      arr[num++] = atoi(token);
      token = strtok(NULL, s);
   }

   for (int i =0; i < num; i++) {
      printf("arr[%d]: %d\n",i, arr[i] );
   }
   */
/*
  int arr[6]={20,23,45,67,1,121};
  int num = 6;
  char temp[128] = {};
  char numstr[10] = {};
  for ( int i = 0; i < num-1; i++) {

      sprintf(numstr, "%d",arr[i]);
    // numstr[strlen(numstr)]='\0';
 			strcat(temp, numstr);
 			strcat(temp,", ");
      printf("%s\n",temp);

  }
 	//char numstr[10];
  sprintf(numstr, "%d",arr[num-1]);
  strcat(temp, numstr);

  printf("{%s}\n",temp );
  return 0;
}
*/


int str[7] = {4, 2, 6, 1, 5, 12, 11};
//int arr[200];
//int num =0;
//int i=0;
//char tempstr[100];
//int numchars=0;

//Intset result;

//if (!is_valid_input(str))
  //ereport(ERROR,
    //  (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
      // errmsg("invalid input syntax for complex: \"%s\"",
        //  str)));



qsort((void*) str, 7, sizeof(int), compare);
int i=0;

for(i=0; i< 7; i++){
  printf("%d ", str[i]);
}

return 0;
}
