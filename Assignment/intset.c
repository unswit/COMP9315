/*
 * src/tutorial/complex.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */
#include <string.h>
#include <ctype.h>

#define MAX 500

 int is_valid_input(char *str);

PG_MODULE_MAGIC;

typedef struct intset
{
	int num;
	int arr[MAX];
}Intset;


/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

 int is_valid_input(char *str)
 {
	int numLeftBrace = 0;
	int numRightBrace =0;
	int numComma = 0;
	int digitSeen =0;
	int spaceSeen =0;
	int commaSeen = 0;

 	int i;
 	for(i = 0; i < strlen(str); i++)
 	{
 		if(!isdigit(str[i]) && str[i]!='{' && str[i] != '}' && str[i]!=',' && str[i]!=' ' && str[i]!='-'){
 			return 0;
 		}
		if(str[i]=='{'){
 			numLeftBrace++;
 		}
		if(str[i]=='}'){
 			numRightBrace++;
 		}
		if(str[i]==','){
 			numComma++;
 		}
		if(isdigit(str[i])){
			digitSeen =1;
		}
		if(digitSeen && str[i]==' '){
			spaceSeen =1;
		}
		if(spaceSeen && str[i]==','){
			commaSeen =1;
		}
		if(spaceSeen && !commaSeen && isdigit(str[i])){
			return 0;
		}
 	}

	if (numLeftBrace != 1 || numRightBrace != 1) {
		return 0;
	}


 	return 1;
 }


PG_FUNCTION_INFO_V1(intset_in);

Datum
intset_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);
	int arr[MAX];
	int num =0;
	int i=0;
	char tempstr[strlen(str)];
	int numchars=0;

	Intset    *result;

	if (!is_valid_input(str))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for intset: \"%s\"",
						str)));

	for (i = 0; i< strlen(str); i++) {
		if (isdigit(str[i]) || str[i] == ',') {
			tempstr[numchars++] = str[i];
			//printf("%c\n", str[i]);
		}
	}

	 if(numchars > 0){
		 tempstr[numchars]='\0';
	   //const char s[2] = ",";
	   char *token;
	   token = strtok(tempstr, ",");
	   //printf("%s ", tempstr);
	   //printf("token %s ", token);
	 while( token != NULL ) {
	     //  printf("%s\n", token);
	       arr[num++] = atoi(token);
	       //printf("num %d\n", num);
	       token = strtok(NULL,",");
	       //printf("%s\n", token);
	    }
	  }

	result = (Intset *) palloc(sizeof(Intset));
	result->num =0;
	//result->arr = arr;

	for(i = 0; i < num; i++){
		int j = 0;
		int found = 0;
		for(j =0; j < result->num; j++){
			if(arr[i] == result->arr[j]){
				found = 1;
			}
		}
		if(!found){
			result->arr[result->num++] = arr[i];
		}
	}

	PG_RETURN_POINTER(result);
}




PG_FUNCTION_INFO_V1(intset_out);

Datum
intset_out(PG_FUNCTION_ARGS)
{
	Intset    *intset = (Intset *) PG_GETARG_POINTER(0);
	char	   *result;
	char temp[128] = {};
	int i;

	for (i = 0; i < intset->num-1; i++) {
			char numstr[10] = {};
			sprintf(numstr, "%d",intset->arr[i]);
			numstr[strlen(numstr)]='\0';
 			strcat(temp, numstr);
 			strcat(temp,", ");
	}
	if(intset->num > 0){
		char numstr[10] = {};
		sprintf(numstr, "%d",intset->arr[intset->num-1]);
		numstr[strlen(numstr)]='\0';
	  strcat(temp, numstr);
		temp[strlen(temp)]='\0';
	}
	result = psprintf("{%s}", temp);
	PG_RETURN_CSTRING(result);
}

/*****************************************************************************
 * Binary Input/Output functions
 *
 * These are optional.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(intset_recv);

Datum
intset_recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);
	Intset    *result;
	int i;
	result = (Intset *) palloc(sizeof(Intset));
	result->num = pq_getmsgint(buf,4);
	for(i = 0; i < result->num; i++){
		result->arr[i] = pq_getmsgint(buf,4);
	}

	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(intset_send);

Datum
intset_send(PG_FUNCTION_ARGS)
{
	Intset    *intset = (Intset *) PG_GETARG_POINTER(0);
	StringInfoData buf;
	int i;
	pq_begintypsend(&buf);
	pq_sendint(&buf, intset->num,4);
	for(i = 0; i < intset->num; i++){
		pq_sendint(&buf, intset->arr[i],4);
	}

	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 * New Operators
 *
 * A practical Complex datatype would provide much more than this, of course.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(intset_minus);

Datum
intset_minus(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);
	Intset    *result;
	int i;

	result = (Intset *) palloc(sizeof(Intset));
	result->num = 0;


	for(i =0; i < a->num; i++){
		int found = 0;
		int j = 0;
		for(j = 0; j < b->num; j++){
			if(a->arr[i] == b->arr[j]){
				found = 1;
			}
		}
		if(!found){
			result->arr[result->num++] = a->arr[i];
		}
	}
	PG_RETURN_POINTER(result);
}


PG_FUNCTION_INFO_V1(intset_disjunction);

Datum
intset_disjunction(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);
	Intset    *result;
	int i;

	result = (Intset *) palloc(sizeof(Intset));
	result->num = 0;


	for(i =0; i < a->num; i++){
		int found = 0;
		int j = 0;
		for(j = 0; j < b->num; j++){
			if(a->arr[i] == b->arr[j]){
				found = 1;
			}
		}
		if(!found){
			result->arr[result->num++] = a->arr[i];
		}
	}

	for(i =0; i < b->num; i++){
		int found = 0;
		int j = 0;
		for(j = 0; j < a->num; j++){
			if(b->arr[i] == a->arr[j]){
				found = 1;
			}
		}
		if(!found){
			result->arr[result->num++] = b->arr[i];
		}
	}

	PG_RETURN_POINTER(result);
}


PG_FUNCTION_INFO_V1(intset_union);

Datum
intset_union(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);
	Intset    *result;
	int i;

	result = (Intset *) palloc(sizeof(Intset));
	result->num = 0;


	for(i =0; i < a->num; i++){
		result->arr[result->num++] = a->arr[i];
	}

	for(i =0; i < b->num; i++){
		int found = 0;
		int j = 0;
		for(j = 0; j < a->num; j++){
			if(b->arr[i] == a->arr[j]){
				found = 1;
			}
		}
		if(!found){
			result->arr[result->num++] = b->arr[i];
		}
	}

	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(intset_intersection);

Datum
intset_intersection(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);
	Intset    *result;
	int i;

	result = (Intset *) palloc(sizeof(Intset));
	result->num = 0;


	for(i =0; i < a->num; i++){
		int found = 0;
		int j = 0;
		for(j = 0; j < b->num; j++){
			if(a->arr[i] == b->arr[j]){
				found = 1;
			}
		}
		if(found){
			result->arr[result->num++] = a->arr[i];
		}
	}
	PG_RETURN_POINTER(result);
}


PG_FUNCTION_INFO_V1(intset_equal);

Datum
intset_equal(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);
	int numFoundinB = 0;
	int numFoundinA = 0;

	int i = 0;
	for(i =0; i < a->num; i++){
		int j = 0;
		for(j = 0; j < b->num; j++){
			if(a->arr[i] == b->arr[j]){
				numFoundinB++;
			}
		}
	}

	for(i =0; i < b->num; i++){
		int j = 0;
		for(j = 0; j < a->num; j++){
			if(b->arr[i] == a->arr[j]){
				numFoundinA++;
			}
		}
	}

	PG_RETURN_BOOL(numFoundinA == b->num && numFoundinB == a->num);
}


PG_FUNCTION_INFO_V1(intset_subset);

Datum
intset_subset(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);
	int numFoundinB = 0;

	int i = 0;
	for(i =0; i < a->num; i++){
		int j = 0;
		for(j = 0; j < b->num; j++){
			if(a->arr[i] == b->arr[j]){
				numFoundinB++;
			}
		}
	}

	PG_RETURN_BOOL(numFoundinB == a->num);
}


PG_FUNCTION_INFO_V1(intset_contains);

Datum
intset_contains(PG_FUNCTION_ARGS)
{
	int   a = (int) PG_GETARG_INT32(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);

	int found = 0;
	int i = 0;
	for(i =0; i < b->num; i++){
		if(a == b->arr[i]){
			found = 1;
		}
	}

	PG_RETURN_BOOL(found == 1);
}


PG_FUNCTION_INFO_V1(intset_cardinality);

Datum
intset_cardinality(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);

	PG_RETURN_INT32(a->num);
}
/*****************************************************************************
 * Operator class for defining B-tree index
 *
 * It's essential that the comparison operators and support function for a
 * B-tree index opclass always agree on the relative ordering of any two
 * data values.  Experience has shown that it's depressingly easy to write
 * unintentionally inconsistent functions.  One way to reduce the odds of
 * making a mistake is to make all the functions simple wrappers around
 * an internal three-way-comparison function, as we do here.
 *****************************************************************************/

/*
#define Mag(c)	((c)->x*(c)->x + (c)->y*(c)->y)

static int
complex_abs_cmp_internal(Complex * a, Complex * b)
{
	double		amag = Mag(a),
				bmag = Mag(b);

	if (amag < bmag)
		return -1;
	if (amag > bmag)
		return 1;
	return 0;
}


PG_FUNCTION_INFO_V1(complex_abs_lt);

Datum
complex_abs_lt(PG_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(complex_abs_cmp_internal(a, b) < 0);
}

PG_FUNCTION_INFO_V1(complex_abs_le);

Datum
complex_abs_le(PG_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(complex_abs_cmp_internal(a, b) <= 0);
}

PG_FUNCTION_INFO_V1(complex_abs_eq);

Datum
complex_abs_eq(PG_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(complex_abs_cmp_internal(a, b) == 0);
}

PG_FUNCTION_INFO_V1(complex_abs_ge);

Datum
complex_abs_ge(PG_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(complex_abs_cmp_internal(a, b) >= 0);
}

PG_FUNCTION_INFO_V1(complex_abs_gt);

Datum
complex_abs_gt(PG_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(complex_abs_cmp_internal(a, b) > 0);
}

PG_FUNCTION_INFO_V1(complex_abs_cmp);

Datum
complex_abs_cmp(PG_FUNCTION_ARGS)
{
	Complex    *a = (Complex *) PG_GETARG_POINTER(0);
	Complex    *b = (Complex *) PG_GETARG_POINTER(1);

	PG_RETURN_INT32(complex_abs_cmp_internal(a, b));
}
*/
