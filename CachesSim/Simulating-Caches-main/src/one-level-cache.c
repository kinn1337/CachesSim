#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// struct for cache block initializing if the slot is open, the oder, the tage, and the usage
struct cacheBlock{
  int open;
  unsigned long long order;
  unsigned long long tag;
  unsigned long long use;
};

// main method that builds the cache based on the project assignment of FIFO
int main(int argc, char* argv[argc+1]){

  if(argc != 6){
    printf("error\n");
    return EXIT_SUCCESS;
  }

  unsigned long long size = atoi(argv[1]);
  unsigned long long assoc;
  char policy;
  unsigned long long block = atoi(argv[4]);
  FILE* fp = fopen(argv[5], "r");

  if(argv[2][0] == 'd'){
    assoc = 1;
  }else{
    if(argv[2][5] == ':'){
      char assoc1[10];
      int f = 0;
      int k = 6;
      while(argv[2][k] != '\0'){
	assoc1[f] = argv[2][k];
	k++;
	f++;
      }
      assoc = atoi(assoc1);
    }else{
      assoc = size/block;
    }
  }

  if(argv[3][0] == 'f'){
    policy = 'f';
  }else{
    policy = 'l';
  }


  if(argc != 6){
    printf("error\n");
    return EXIT_SUCCESS;
  }
  int check1 = log(block)/log(2);
  int check2 = log(size)/log(2);
  if((check1)%1 != 0 || (check2)%1 != 0 ){
    printf("error\n");
    return EXIT_SUCCESS;
  }


  unsigned long long numblock = size/block;
  unsigned long long set = numblock/assoc;

  unsigned long long offsetBits = log(block)/log(2);
  unsigned long long setBits = log(set)/log(2);
  unsigned long long shift = offsetBits+setBits;
  unsigned long long tagBits = 48 - shift;

  struct cacheBlock** cache = calloc(set, sizeof(struct cacheBlock*));
  for(int i = 0; i < set; i++){
    cache[i] = calloc(assoc, sizeof(struct cacheBlock));
    for(int j = 0; j < assoc; j++){
      cache[i][j].open = 0;
    }
  }

  int lru = 0;
  int fifo = 0;

  int hit = 0;
  int miss = 0;
  int read = 0;
  int write = 0;

  char char1;
  unsigned long long num;
  while(fscanf(fp, "%c %llx\n", &char1, &num)!=EOF){
    unsigned long long tagl1 = (num>>shift) & ((int)pow(2,tagBits)-1);
    unsigned long long setl1 = (num>>offsetBits) & ((int)pow(2,setBits)-1);

    if(char1 == 'W'){
      int exists = 0;
      for(int i = 0; i < assoc; i++){
	if(cache[setl1][i].open == 1 && cache[setl1][i].tag == tagl1){
	  exists = 1;
	  write++;
	  hit++;
	   if(policy == 'l'){
	    cache[setl1][i].order = lru;
	    lru++;
	    }
	  break;
	}
      }
      int space = 0;
      if(exists == 0){
	miss++;
	write++;
	read++;
	for(int i = 0; i < assoc; i++){
	  if(cache[setl1][i].open == 0){
	    cache[setl1][i].tag = tagl1;
	    cache[setl1][i].use = fifo;
	    cache[setl1][i].order = lru;
	    cache[setl1][i].open = 1;
	    lru++;
	    fifo++;
	    space = 1;
	    break;
	  }
	}
      }
      if(space == 0 && exists == 0){
	int replace = 0;
	if(policy == 'l'){
	  for(int i = 0; i < assoc-1; i++){
	    if(cache[setl1][replace].order<cache[setl1][i+1].order){

	    }else{
	      replace = i+1;
	    }
	  }
	  cache[setl1][replace].tag = tagl1;
	  cache[setl1][replace].order = lru;
	  lru++;
	}else{
	  for(int i = 0; i < assoc-1; i++){
	    if(cache[setl1][replace].use<cache[setl1][i+1].use){

	    }else{
	      replace = i+1;
	    }
	  }
	  cache[setl1][replace].tag = tagl1;
	  cache[setl1][replace].use = fifo;
	  fifo++;
	}
      }
    }else{
      int exists = 0;
      for(int i = 0; i < assoc; i++){
	if(cache[setl1][i].tag == tagl1){
	  exists = 1;
	  cache[setl1][i].order = lru;
	  lru++;
	  hit++;
	  break;
	}
      }

      int space = 0;
      if(exists == 0){
	read++;
	miss++;
	for(int i = 0; i < assoc; i++){
	  if(cache[setl1][i].open == 0){
	    cache[setl1][i].tag = tagl1;
	    cache[setl1][i].use = fifo;
	    cache[setl1][i].order = lru;
	    cache[setl1][i].open = 1;
	    lru++;
	    fifo++;
	    space = 1;
	    break;
	  }
	}
      }

      if(space == 0 && exists == 0){
	int replace = 0;
	if(policy == 'l'){
	  for(int i = 0; i < assoc-1; i++){
	    if(cache[setl1][replace].order<cache[setl1][i+1].order){

	    }else{
	      replace = i+1;
	    }
	  }
	  cache[setl1][replace].tag = tagl1;
	  cache[setl1][replace].order = lru;
	  lru++;
	}else{
	  for(int i = 0; i < assoc-1; i++){
	    if(cache[setl1][replace].use<cache[setl1][i+1].use){

	    }else{
	      replace = i+1;
	    }
	  }
	  cache[setl1][replace].tag = tagl1;
	  cache[setl1][replace].use = fifo;
	  fifo++;
	}
      }
    }
  }

  printf("memread:%d\n", read);
  printf("memwrite:%d\n", write);
  printf("cachehit:%d\n", hit);
  printf("cachemiss:%d\n", miss);

  for(int i = 0; i < set; i++){
    free(cache[i]);
  }
  free(cache);
  fclose(fp);
  return EXIT_SUCCESS;
}