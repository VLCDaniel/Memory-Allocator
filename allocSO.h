#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define wordsize 8 

pthread_mutex_t memoryLock;

struct block{
  size_t size;
  unsigned used;
  struct block *next;
};


size_t align(size_t n);
struct block* requestFromOS(size_t size);
struct block* getBlockFromList(size_t size);
void* mallocSO(size_t size);
void freeSO(void *data);
void* reallocSO(void * data, size_t size);


