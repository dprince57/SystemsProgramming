/*
   This program will take a file, and depending on the file type, it's going to do something.
    - regular files:
	- what this does depends on whether or not you have execute permission
	  - if you do, execute the program (but if someone passes in this program as an argument, do NOT let it execute!)
	  - if you don't, print the file's contents to stdout

    - directories:
       	- print the directory's contents in an ordered list (but don't print hidden files) then exit.
   
    - links:
	- Just print that it's a symbolic link, then exit
    - everything else
	- print that this file type is not supported then exit

*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

//function prototypes
int checkFileType(int mode); // checks file type
int checkPermissions(int mode, int perm); // checks file permissions

// writing define statements for file types, so it's clearer what our code is doing
#define OTHER 0   
#define REG   1
#define DIREC 2
#define LINK  3

//define statement for buffer
#define BUF 2000

int main (int argc, char **argv){

	//prints usage statement if file not provided
	if(argc < 2){
		fprintf(stderr, "Usage %s file\n", argv[0]);
		exit(1);
	}

	//getting information on the file we're passing in
	struct stat st; 
	int rt;
	rt = lstat(argv[1], &st);

	//check return value of lstat
	if(rt < 0){
		perror("Error");
		exit(1);
	}
	
	int ftype = checkFileType(st.st_mode); //checking what type of file this is

		//if the filetype is anything other than a regular file, directory, or link
	if(ftype == OTHER){
		// -- print message saying this filetype is not supported and exit
		printf("File Type not supported\n");
		exit(0);
	}

	//if the filetype is a link 
	if(ftype == LINK){
		//just say that it's a symbolic link then exit
		printf("This file is a symbolic link. Closing. Goodbye!\n");
		exit(0);
	}

	//if filetype is a directory, we're going to print the contents in a numbered list
	if(ftype == DIREC){

		DIR *mydir; //defining pointer to directory
		struct dirent *file; //accessing dirent struct, defined in dirent.h

		//open the directory
		mydir = opendir(argv[1]);
		//check return value of opendir
		if(mydir == NULL){
			perror("Error  opening directory");
			exit(1);
		}

		int i = 1; //setting up counter
		printf("%s is a directory, The contents are as follows: \n", argv[1]);
		
		//loop through directory printing contents
		while((file = readdir(mydir)) != NULL){
			//if the current file is a hidden file, skip printing it
			if(file->d_name[0] == '.') continue;
		   //print the list number and the file name
			printf("%d. %s\n" , i++, file->d_name);
		}

		//then close the directory
		closedir(mydir);
		exit(0);

	}

	//if the filetype is a regular file	
			
	if(ftype == REG){

		//check if owner has executable permission 
		// feel free to steal checkPermissions function from perms.c in in-class code
		// but don't forget to fill that function out below
		// store return value in perm;
		int perm = checkPermissions(st.st_mode, S_IXUSR); 


		if(perm == 1){ //it is executable, so we're gonna try to execute it

			// DANGER! used wrongly, fork can do damage if you're not careful!
			// we need to make sure that this program can never execute its own binary, so we will do the following

			struct stat self;     // call stat on this very program, store metadata to self
			if(stat(argv[0], &self) < 0){
				perror("error getting self stat");
				exit(1); //I didnt like the fact that this looked like it could fail
					 //If this failed somehow and it got passed we have endless fork
			}// this program is in argv[0]

			// each file in this filesystem has a unique file inode number
			// so we will check the inode number in self and in st to see if they're the same
							
			if(self.st_ino == st.st_ino) { 
				// someone tried passing this program into this program, so tell the user what they did and not to do that. 
				printf("It looks like you're trying to run this very program through this program.\nPlease don't do that.\n");
				exit(1);

			} else { //it's another program. so we're going to execute it

				printf("Running %s:\n", argv[1]); //inform user what program they're executing
				
				pid_t pid; 

				//call fork, store in pid
				pid = fork();
				//check return value of pid, if fork fails, say so and exit
				if(pid < 0){
					perror("Fork has failed");
					exit(1);
				}
				if(pid){ //we are in the parent process

					//we have a child, so get the exit code
					int status;
					waitpid(pid, &status, 0);
					return 0;

				} else {
					//we are in the child process
					//use a variant of exec to execute the program
					execv(argv[1],&argv[1]);
					return 0;
				}
			}
		} else { // not executable, so we're just printing the contents
			
			char content[BUF]; //buffer for content

			int fd; 
				//open file for reading
			fd = open(argv[1],O_RDONLY);
			//checking return value of open
			if(fd < 0){
				perror("Error opening File");
				exit(1);
			}

			
			// call read, read stuff into the buffer
			ssize_t bytesRead = read(fd, content, BUF - 1);
			if(bytesRead < 0){
				perror("Error opening File");
				exit(1);
			}
			//then write the buffer to stdout
			content[bytesRead] = '\0';
			printf("%s",content);
			//close file
			close(fd);
		}	
	}

	return 0;
	
}


//checking if file has a certain permission
int checkPermissions(int mode, int perm){        
   return(mode & perm) ? 1 : 0;
}

int checkFileType(int mode){

	//check if file is a regular file
	if(S_ISREG(mode))
		return REG;
	
	//check if file is a directory
	if(S_ISDIR(mode))
		return DIREC;

	// check if file is a symbolic link
	if(S_ISLNK(mode))
		return LINK;

   return OTHER;
}


