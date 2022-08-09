//pingpong.c
#include<kernel/types.h>
#include<user/user.h>

int main(){
	//pipe1(p1):parent write,child read
	//pipe2(p2):parent read,child write
	int p1[2],p2[2];

	//conveying char array:1 byte
	char buffer[]={'x'};
	long length=sizeof(buffer);

	//create pipe
	pipe(p1);
	pipe(p2);

	//child process
	//system call fork(): create child process
	if(fork()==0){
		close(p1[1]);
		close(p2[0]);

		//child process read from pipe1,p1[0]
		//system call read:
		//int read(int fd,char *buf,int n)
		//read n bytes from buf;returns number read;
		//or 0 if end of file
		if(read(p1[0],buffer,length)!=length){
			printf("a--->b read error!");
			exit(1);
		}
                 
                printf("%d: received ping\n",getpid());

		//child process write into pipe2,p2[1]
		//system call write;
		//int write(int fd,char *buf,int n)
		//write n bytes from buf to file descriptor fd;
		//return n
		if(write(p2[1],buffer,length)!=length){
			printf("a<---b write error!");
			exit(1);
		}
		exit(0);
	}

	//parent process
	close(p1[0]);
	close(p2[1]);

	//parent process write
	if(write(p1[1],buffer,length)!=length){
		printf("a--->b write error!");
		exit(1);
	}

	//parent process read
	if(read(p2[0],buffer,length)!=length){
		printf("a<---b read error!");
		exit(1);
	}

	printf("%d: received pong\n",getpid());
        //wait until child exit
	wait(0);
	exit(0);
}


