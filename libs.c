#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

char* readString(int size, char *s) {
    if (size <= 0) {
      return NULL;
    }

    // Create safeguard buffer
    char buf[size];

    // Sets the sequence to 0
    memset(buf, 0, size);

    if (size == 1) {
      return NULL;
    }
    size--; // Nead to read size - 1 chars

    // Counter
    int i = 0;

    char c = getchar();
    buf[i] = c;
    // Read the string until EOF or newline
    for (i = 0; i < size; i++) {
      c = getchar();
      buf[i] = c;
      if (c == EOF || c == '\n') break;
    }
    // Copy from safeguard buffer
    strcpy((char *)s, (char *)buf);
    return NULL;
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
