#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
//include "userprog/syscall.h"


int
main(int argc, char *argv[])
{
	int summation=0,fibonacci=0;
	int num1,num2,num3,num4;
//	printf("%s %s %s %s\n", argv[1], argv[2], argv[3], argv[4]);
	num1 = atoi(argv[1]);
	num2 = atoi(argv[2]);
	num3 = atoi(argv[3]);
	num4 = atoi(argv[4]);

//	printf("——%d %d %d %d ——\n",num1,num2,num3,num4);
	fibonacci = fibo(num1);
	summation = summationFour(num1,num2,num3,num4);

	printf("Input: %s %d %d %d %d\n",argv[0],num1,num2,num3,num4);
	printf("Fibo of num1: %d\nsummation of four elements: %d\n",fibonacci,summation);

	return EXIT_SUCCESS;
}
