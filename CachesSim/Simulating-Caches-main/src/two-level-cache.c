#include<stdio.h>
#include<stdlib.h>
#include<math.h>

// struct for cache block initializing if the slot is open, the oder, the tage, and the usage
struct cacheBlock{
  int open;
  unsigned long long order;
  unsigned long long tag;
  unsigned long long use;
  unsigned long long num;
};

// main method that builds the cache based on the project assignment of FIFO and LRU
int main(int argc, char* argv[argc+1]){

  if(argc != 9){
    printf("error\n");
    return EXIT_SUCCESS;
  }

  unsigned long long sizel1 = atoi(argv[1]);
  unsigned long long assocl1;
  char policyl1;
  unsigned long long blockl1 = atoi(argv[4]);

  if(argv[2][0] == 'd'){
    assocl1 = 1;
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
      assocl1 = atoi(assoc1);
    }else{
      assocl1 = sizel1/blockl1;
    }
  }

  if(argv[3][0] == 'f'){
    policyl1 = 'f';
  }else{
    policyl1 = 'l';
  }

  unsigned long long sizel2 = atoi(argv[5]);
  unsigned long long assocl2;
  char policyl2;
  FILE* fp = fopen(argv[8], "r");

  if(argv[6][0] == 'd'){
    assocl2 = 1;
  }else{
    if(argv[6][5] == ':'){
      char assoc2[10];
      int f = 0;
      int k = 6;
      while(argv[6][k] != '\0'){
	assoc2[f] = argv[6][k];
	k++;
	f++;
      }
      assocl2 = atoi(assoc2);
    }else{
      assocl2 = sizel2/blockl1;
    }
  }

  if(argv[7][0] == 'f'){
    policyl2 = 'f';
  }else{
    policyl2 = 'l';
  }

  int check1 = log(blockl1)/log(2);
  int check2 = log(sizel1)/log(2);
  int check3 = log(sizel2)/log(2);
  if((check1)%1 != 0 || (check2)%1 != 0 || (check3)%1 != 0){
    printf("error\n");
    return EXIT_SUCCESS;
  }


  unsigned long long numblock1 = sizel1/blockl1;
  unsigned long long numblock2 = sizel2/blockl1;
  unsigned long long set1 = numblock1/assocl1;
  unsigned long long set2 = numblock2/assocl2;

  unsigned long long offsetBits = log(blockl1)/log(2);
  unsigned long long setBits1 = log(set1)/log(2);
  unsigned long long setBits2 = log(set2)/log(2);
  unsigned long long shift1 = offsetBits+setBits1;
  unsigned long long shift2 = offsetBits+setBits2;
  unsigned long long tagBits1 = 48 - shift1;
  unsigned long long tagBits2 = 48 - shift2;

  struct cacheBlock** cache1 = calloc(set1, sizeof(struct cacheBlock*));
  for(int i = 0; i < set1; i++){
    cache1[i] = calloc(assocl1, sizeof(struct cacheBlock));
    for(int j = 0; j < assocl1; j++){
      cache1[i][j].open = 0;
    }
  }

  struct cacheBlock** cache2 = calloc(set2, sizeof(struct cacheBlock*));
  for(int i = 0; i < set2; i++){
    cache2[i] = calloc(assocl2, sizeof(struct cacheBlock));
    for(int j = 0; j < assocl2; j++){
      cache2[i][j].open = 0;
    }
  }

  int fifo1 = 0;
  int fifo2 = 0;
  int lru1 = 0;
  int lru2 = 0;

  int read = 0;
  int write = 0;
  int l1hit = 0;
  int l1miss = 0;
  int l2hit = 0;
  int l2miss = 0;

  char char1;
  unsigned long long num;
  while(fscanf(fp, "%c %llx\n", &char1, &num)!=EOF){
    unsigned long long tagl1 = (num>>shift1) & ((int)pow(2,tagBits1)-1);
    unsigned long long setl1 = (num>>offsetBits) & ((int)pow(2,setBits1)-1);
    unsigned long long tagl2 = (num>>shift2) & ((int)pow(2,tagBits2)-1);
    unsigned long long setl2 = (num>>offsetBits) & ((int)pow(2,setBits2)-1);
    if(char1 == 'W'){
      unsigned long long temp = 0;
      int exists = 0;
      for(int i = 0; i < assocl1; i++){
	if(cache1[setl1][i].open == 1 && cache1[setl1][i].tag == tagl1){
	  exists = 1;
	  write++;
	  l1hit++;
	  if(policyl1 == 'l'){
	    cache1[setl1][i].order = lru1;
	    lru1++;
	  }
	  break;
	}
      }

      int exists2 = 0;
      if(exists == 0){
	for(int i = 0; i < assocl2; i++){
	  if(cache2[setl2][i].open == 1 && cache2[setl2][i].tag == tagl2){
	    exists2 = 1;
	    l2hit++;
	    cache2[setl2][i].open = 0;
	  }
	}
      }

      if(exists2 == 0 && exists == 0){
	l2miss++;
	read++;
      }


      int space = 0;
      if(exists == 0){
	l1miss++;
	write++;
	for(int i = 0; i < assocl1; i++){
	  if(cache1[setl1][i].open == 0){
	    cache1[setl1][i].tag = tagl1;
	    cache1[setl1][i].num = num;
	    cache1[setl1][i].use = fifo1;
	    cache1[setl1][i].order = lru1;
	    cache1[setl1][i].open = 1;
	    lru1++;
	    fifo1++;
	    space = 1;
	    break;
	  }
	}
      }

      if(space == 0 && exists == 0){
	int replace = 0;
	if(policyl1 == 'l'){
	  for(int i = 0; i < assocl1-1; i++){
	    if(cache1[setl1][replace].order<cache1[setl1][i+1].order){

	    }else{
	      replace = i+1;
	    }
	  }
	  cache1[setl1][replace].order = lru1;
	  lru1++;
	}else{
	  for(int i = 0; i < assocl1-1; i++){
	    if(cache1[setl1][replace].use<cache1[setl1][i+1].use){

	    }else{
	      replace = i+1;
	    }
	  }
	  cache1[setl1][replace].use = fifo1;
	  fifo1++;
	}

	  temp = cache1[setl1][replace].num;
	  cache1[setl1][replace].tag = tagl1;
	  cache1[setl1][replace].num = num;

	  tagl2 = (temp>>shift2) & ((int)pow(2,tagBits2)-1);
	  setl2 = (temp>>offsetBits) & ((int)pow(2,setBits2)-1);

	  int exists2 = 0;
	  for(int i = 0; i < assocl2; i++){
	    if(cache2[setl2][i].open == 1 && cache2[setl2][i].tag == tagl2){
	      exists2 = 1;
	      l2hit++;
	      if(policyl2 == 'l'){
		cache2[setl2][i].order = lru2;
		lru2++;
	      }
	      break;
	    }
	  }
	  int space2 = 0;
	  if(exists2 == 0){
	    l2miss++;
	    for(int i = 0; i < assocl2; i++){
	      if(cache2[setl2][i].open == 0){
		cache2[setl2][i].tag = tagl2;
		cache2[setl2][i].use = fifo2;
		cache2[setl2][i].order = lru2;
		cache2[setl2][i].num = temp;
		cache2[setl2][i].open = 1;
		lru2++;
		fifo2++;
		space2 = 1;
		break;
	      }
	    }
	  }
	  if(space2 == 0 && exists2 == 0){
	    int replace2 = 0;
	    if(policyl2 == 'l'){
	      for(int i = 0; i < assocl2-1; i++){
		if(cache2[setl2][replace2].order<cache2[setl2][i+1].order){

		}else{
		  replace2 = i+1;
		}
	      }
	      cache2[setl2][replace2].tag = tagl2;
	      cache2[setl2][replace2].order = lru2;
	      lru2++;
	    }else{
	      for(int i = 0; i < assocl2-1; i++){
		if(cache2[setl2][replace2].use<cache2[setl2][i+1].use){

		}else{
		  replace2 = i+1;
		}
	      }
	      cache2[setl2][replace2].tag = tagl2;
	      cache2[setl2][replace2].use = fifo2;
	      fifo2++;
	    }
	  }
      }
    }else{
      int exists = 0;
      for(int i = 0; i < assocl1; i++){
	if(cache1[setl1][i].tag == tagl1){
	  exists = 1;
	  cache1[setl1][i].order = lru1;
	  lru1++;
	  l1hit++;
	  break;
	}
      }
      int space = 0;
      int existsl2 = 0;
      unsigned long long temp1 = num;
      unsigned long long temp = 0;
      if(exists == 0){
	l1miss++;
        for(int i = 0; i < assocl2; i++){
	  if(cache2[setl2][i].tag == tagl2){
	    existsl2 = 1;
	    l2hit++;
	    cache2[setl2][i].open = 0;
	    //temp1 = cache2[setl2][i].num;
	    //tagl1 = (temp1>>shift1) & ((int)pow(2,tagBits1)-1);
	    //setl1 = (temp1>>offsetBits) & ((int)pow(2,setBits1)-1);
	    break;
	  }
	}
	if(existsl2 == 0){
	  read++;
	  //write++;
	  l2miss++;
	}
	for(int i = 0; i < assocl1; i++){
	  if(cache1[setl1][i].open == 0){
	    cache1[setl1][i].tag = tagl1;
	    cache1[setl1][i].use = fifo1;
	    cache1[setl1][i].order = lru1;
	    cache1[setl1][i].open = 1;
	    cache1[setl1][i].num = temp1;
	    lru1++;
	    fifo1++;
	    space = 1;
	    break;
	  }
	}
      }

      if(space == 0 && exists == 0){
        int replace = 0;
	if(policyl1 == 'l'){
	  for(int i = 0; i < assocl1-1; i++){
	    if(cache1[setl1][replace].order<cache1[setl1][i+1].order){

	    }else{
	      replace = i+1;
	    }
	  }
	  cache1[setl1][replace].order = lru1;
	  lru1++;
	}else{
	  for(int i = 0; i < assocl1 - 1; i++){
	    if(cache1[setl1][replace].use<cache1[setl1][i+1].use){

	    }else{
	      replace = i+1;
	    }
	  }
	  cache1[setl1][replace].use = fifo1;
	  fifo1++;
	}

	  temp = cache1[setl1][replace].num;
	  cache1[setl1][replace].tag = tagl1;
	  cache1[setl1][replace].num = num;

	  tagl2 = (temp>>shift2) & ((int)pow(2,tagBits2)-1);
	  setl2 = (temp>>offsetBits) & ((int)pow(2,setBits2)-1);

	  /*
	  int exists2 = 0;
	  for(int i = 0; i < assocl2; i++){
	    if(cache2[setl2][i].open == 1 && cache2[setl2][i].tag == tagl2){
	      exists2 = 1;
	      l2hit++;
	      if(policyl2 == 'l'){
		cache2[setl2][i].order = lru2;
		lru2++;
	      }
	      break;
	    }
	  }
	  */

	  int space2 = 0;
	  // if(exists2 == 0){
	  //  l2miss++;
	    for(int i = 0; i < assocl2; i++){
	      if(cache2[setl2][i].open == 0){
		cache2[setl2][i].tag = tagl2;
		cache2[setl2][i].use = fifo2;
		cache2[setl2][i].order = lru2;
		cache2[setl2][i].num = temp;
		cache2[setl2][i].open = 1;
		lru2++;
		fifo2++;
		space2 = 1;
		break;
	      }
	    }
	    // }
	    if(space2 == 0) {
	    int replace2 = 0;
	    if(policyl2 == 'l'){
	      for(int i = 0; i < assocl2-1; i++){
		if(cache2[setl2][replace2].order<cache2[setl2][i+1].order){

		}else{
		  replace2 = i+1;
		}
	      }
	      cache2[setl2][replace2].tag = tagl2;
	      cache2[setl2][replace2].order = lru2;
	      lru2++;
	    }else{
	      for(int i = 0; i < assocl2-1; i++){
		if(cache2[setl2][replace2].use<cache2[setl2][i+1].use){

		}else{
		  replace2 = i+1;
		}
	      }
	      cache2[setl2][replace2].tag = tagl2;
	      cache2[setl2][replace2].use = fifo2;
	      fifo2++;
	    }
	  }
      }
    }
  }

  printf("memread:%d\n", read);
  printf("memwrite:%d\n", write);
  printf("l1cachehit:%d\n", l1hit);
  printf("l1cachemiss:%d\n", l1miss);
  printf("l2cachehit:%d\n", l2hit);
  printf("l2cachemiss:%d\n", read);

  for(int i = 0; i < set1; i++){
    free(cache1[i]);
  }
  free(cache1);

    for(int i = 0; i < set2; i++){
    free(cache2[i]);
  }
  free(cache2);

  fclose(fp);
  return EXIT_SUCCESS;
}