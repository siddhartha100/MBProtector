// This program shows how to protect memory intensive 
// part of any program using MBProtector 
#include<stdio.h>

int main(){
	printf("We are in function main now. \n");
	a();
	return 0;
}

void a(){
	// Variable declaration
	double write[1000][1000][1000];
	double read[1000][1000][1000];
	int i,j,k;
	
	printf("We are in function a now. \n");
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
	b();
}

void b(){
	// Variable declaration
	double write[1000][1000][1000];
	double read[1000][1000][1000];
	int i,j,k;
	
	printf("We are in function b now. \n");
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
	c();
}

void c(){
	// Variable declaration
	double write[1000][1000][1000];
	double read[1000][1000][1000];
	int i,j,k;
	
	printf("We are in function c now. \n");
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
	d();
}

void d(){
	printf("We are in function d now. \n");
}