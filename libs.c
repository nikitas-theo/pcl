#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void writeInteger(int n){
    printf("%d",n);
}

void writeBoolean(int b){
    printf("%s", b == 0 ? "false" : "true");
}

void writeChar(int c){
  printf("%c",(char)c);
}

void writeReal(double r){
    printf("%lf",r);
}

void writeString(char * s){
    printf("%s",s);
}

int readInteger(){
    int x; 
    scanf("%d",&x);
    return x; 
}

int readBoolean(){
    int b; 
    scanf("%d",&b);
    return b; 
}

char readChar(){
    char c; 
    scanf("%c",&c);
    return c; 
}

double readReal(){
    double r;
    scanf("%lf",&r);
    return r; 
}

void readString(int size, char * s){
    int curr_size = 0; 
    size --; // acount for '\0'
    char c;
    while (curr_size < size && (c = getchar() != '\n') ){
        if (c == EOF) break;
        s[curr_size++] = c; 
    }
    s[curr_size] = '\0';
}

double ln(double x) {
  return log(x);
}

double arctan(double x) {
  return (double) atan(x);
}

double pi() {
  return (double) M_PI;
}

int truncFunc(double x) {
  return (int) trunc(x);
}

int roundFunc(double x){
  return (int) round(x);
}

char chr(int x){
  return (char) x;
}

int ord(char c){
  return (int) c;
}
