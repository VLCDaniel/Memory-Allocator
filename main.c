#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "allocSO.h"

int i = 1;
void *functie(void *arg) //Ca sa pot folosi thread-uri
{
  int *thr = (int *)mallocSO(10);
  *thr = i * 10;
  i++;
  printf("valoare %d la adresa %p\n", *thr, thr);
  freeSO(thr);
  printf("am eliberat adresa %p\n", thr);
  return NULL;
}

int main(void)
{
  int* aux = (int*) mallocSO(10);
  *aux = 2312;
  printf("valoare %d la adresa %p\n",*aux,aux);
  freeSO(aux);

  int* aux2 = (int*)mallocSO(15);
  *aux2 = 1123;// un bloc de marime ca cel de mai sus o sa refoloseasca spatiul pe care l-a folosit acela
  printf("valoare %d la adresa %p\n",*aux2,aux2);
  freeSO(aux2);

  int* aux3 = (int*)mallocSO(20);//daca iau mai multa memorie, o sa folosesc alta lungime la blocul alocat
  *aux3 = 22;
  printf("valoare %d la adresa %p\n",*aux3,aux3);
  freeSO(aux3);

 pthread_t tid[7];
  int i, count;
  for (i = 0; i < 7; i++)
  {
    if (pthread_create(&tid[i], NULL, functie, NULL))
    {
      perror("Nu s-a putut crea thread-ul.\n");
      return errno;
    }
  }

  for (i = 0; i < 7; i++)
  {
    if (pthread_join(tid[i], NULL))
    {
      perror("Eroare.\n");
      return errno;
    }
  }

  return 0;
}

// gcc -Wall -c allocSOLib.c -- compile to .o files
// ar -cvq libso.a *.o  Creating a Library File
// gcc -o a main.c -L. -lso  compile with library