/*

implementing a version of the "cp" command using standard C Library functions

cp - copies a file

*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){

	if(argc < 3){   //prints usage statement if not enough arguments
		fprintf(stderr,"Usage: %s <src> <dst>\n", argv[0]);
		exit(1); //exits the program 
	}



	FILE *src, *dst;  //declare file pointers
			          // open the source fiile for reading
    src = fopen(argv[1], "r");
    if(src == NULL){
        perror("Error Opening File!\n");
        return 1;
    }
					  // open the destination file for writing
    dst = fopen(argv[2], "w+");
    if(dst == NULL){
        perror("Issue opening or creating dst file!");
        fclose(src);
        return 1;
    }
					  // check return value of fopen for the source file
					  // if it's NULL print usage statement and exit the program

	// if we get here, we then read every character in the source file, 
	// and then write the character to the destination file
	char c;
	while((c=fgetc(src))){

		 //if we reach the end of file, break the loop
        if(c == EOF){
            break;
        }
		 //write the character to the destination file.
        fprintf(dst,"%c",c);
		 //what function would you use to write a character to a file?
	}

	//close the source file
    fclose(src);
	//close the destination file
    fclose(dst);

	return 0;


}
