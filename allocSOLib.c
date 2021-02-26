#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


#define wordsize 8 

pthread_mutex_t memoryLock = PTHREAD_MUTEX_INITIALIZER;

struct block{
  size_t size;
  unsigned used;
  struct block *next;
};


struct block* heapStart[101];
struct block* top[101];

unsigned getBucket(size_t size){
  unsigned index = size/wordsize;

  index = (index > 100) ? 100 : index;

  return index;
}

size_t align(size_t n){
  while(n%wordsize){
    n++;
  }
  return n;
}

struct block* requestFromOS(size_t size){
  struct block* requestedBlock = (struct block*)sbrk(0);
  if(sbrk(size + sizeof(struct block))==(void*) -1){
    return NULL;
  }
  return requestedBlock;
}

struct block* getBlockFromList(size_t size){
  unsigned bucket = getBucket(size);

  struct block* aux = heapStart[bucket];
  while(aux){
    if(aux->used == 0 && aux->size >= size){
      return aux;
    }
    aux=aux->next;
  }
  return 0;
}

void* mallocSO(size_t size){
  if((int)size<=0){
    perror("valoare negativa in malloc");
    return (void*)0;
  }
  size = align(size);
  pthread_mutex_lock(&memoryLock);
  
  struct block* fromList = getBlockFromList(size);
  if(fromList){
    fromList->used = 1;   
    pthread_mutex_unlock(&memoryLock);
    return (void*)(fromList + 1);
  }
  struct block* block = requestFromOS(size);
  block->size = size;
  block->used = 1;
  unsigned bucket = getBucket(size);

  if (heapStart[bucket] == NULL)
  {
    heapStart[bucket] = block;
  }
  
  if(top[bucket]!= 0){
    top[bucket]->next = block;
  }

  top[bucket] = block;
  pthread_mutex_unlock(&memoryLock);
  return (void*)(block + 1);
}
void freeSO(void *data){
  if(!data)
    return;
  pthread_mutex_lock(&memoryLock);
  struct block* header = (struct block*)data -  1;
  header->used=0;

  pthread_mutex_unlock(&memoryLock);
}

void* reallocSO(void * data, size_t size){
  if((int)size<=0){
    perror("valoare negativa in malloc");
    return (void*)0;
  }

  struct block* blockNou = mallocSO(size);

  if(((struct block*)data-1)->size >= size)
    return data;

  if(blockNou){
    pthread_mutex_lock(&memoryLock);
    memcpy(blockNou,data,((struct block*)data-1)->size);
    pthread_mutex_unlock(&memoryLock);
    freeSO(data);
  }
  return blockNou;
}
