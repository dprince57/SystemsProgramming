/*

implementing a version of the "head" command using standard C Library functions

head - prints the first 10 lines of a file, or a specified number of lines if a 
	   number is given

*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
int main(int argc, char **argv){

	//if not enough arguments provided, print usage statement and exit the program
	if(argc < 2 || argc > 3){
		fprintf(stderr,"Usage: %s <file> <number of lines>\n", argv[0]);
		exit(1);
	}
    if(!isdigit(atoi(argv[2]))){
        printf("Number of lines is required to be a interger!\n");
        exit(1);
    }

	FILE *fd;  //declare file pointer
			  // open the file specified by the user
    fd = fopen(argv[1], "r");
	// check return value of fopen
	// if it's NULL print usage statement and exit the program


	int lines;  //declare variable for the number of lines we will print

    if(argc < 3){
        lines = 10;
    }else{
        lines = atoi(argv[2]);
    }
	// if only the filename was specified, we will print 10 lines
		//hint: argc will be less than what?
	// else: we print however many lines specified

	char c; 				//declaring a char
	int linecount = 0; 		//counting the number of lines so far
	while((c=fgetc(fd))){

		//if we see a newline character, increment linecount
        if(c == '\n'){
            printf("%c",c);
            linecount++;
            if(linecount == lines){
                break;
            }
            continue;
        }
		//check if we've reached the end of file, if so break the loop
        if(c == EOF) break;
		//print a single character to the screen
        printf("%c",c);

	}

	fclose(fd); //close the file

	return 0;

}
