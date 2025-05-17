/******************************************************************************

Welcome to GDB Online.
  GDB online is an online compiler and debugger tool for C, C++, Python, PHP, Ruby, 
  C#, OCaml, VB, Perl, Swift, Prolog, Javascript, Pascal, COBOL, HTML, CSS, JS
  Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    
    int n;
    printf("insira o numero de execuções:\t \n")
    scanf("%d", &n);
    
    
    
    for(int i=0; i<n; i++){
    pid_t pid = fork();
    if(pid == 0){
        
         printf(" execução do processo filho"; // Corrected printf statement
         //execl("./hello", "hello", NULL);
        
    }
    else if(pidd > 0){
        printf("execução do processo pai"); // Corrected printf statement
    }
   
    }

    return 0;
}