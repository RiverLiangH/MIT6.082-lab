//primes.c
#include"kernel/types.h"
#include"user/user.h"

void func(int *input,int num);

int main(){
	int input[34];//end by 35
	int i=0;
	for(;i<34;i++){
		input[i]=i+2;
	}
	//input number from 2 to 25
	
	//using func to chose primes
	//using pipe
	func(input,34);
	exit(0);

}

void func(int *input,int num){
	if(num==1){
		printf("prime %d\n",*input);
		return;
	}//when num=1

        //verable define
	int p[2],i;
	int prime=*input;
	int temp;

	printf("prime %d\n",prime);

	pipe(p);//create pipe

	//child process
	if(fork()==0){
		for(i=0;i<num;i++){
			temp=*(input+i);
			write(p[1],(char*)(&temp),4);
		}
		exit(0);
	}

	
	close(p[1]);
	if(fork()==0){
		int counter=0;
		char buffer[4];
		while(read(p[0],buffer,4)!=0){
			temp=*((int *)buffer);
			if(temp%prime!=0){
				*input=temp;
				input++;
				counter++;
			}
		}
		func(input-counter,counter);
		exit(0);
	}

	wait(0);
	wait(0);
}

