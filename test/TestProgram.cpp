// This program shows how to protect memory intensive 
// part of any program using MBProtector 
#include<stdio.h>

int main(){

// Variable declaration
double write[1000][1000][1000];
double read[1000][1000][1000];
int i,j,k;

while(1){
	for(i=0;i<1000;i++){
		for(j=0;j<1000;j++){
			for(k=0;k<1000;k++){
				write[i][j][k] = (i*0.01)+(j*0.01)+(k*0.01);
			}
		}
	}
	for(i=0;i<1000;i++){
		for(j=0;j<1000;j++){
			for(k=0;k<1000;k++){
				read[i][j][k] = write[i][j][k];
			}
		}
	}
}

return 0;
}