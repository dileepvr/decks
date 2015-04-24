#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <curand.h>
#include <curand_kernel.h>

__global__ void mykernel() {

}

void get_real_param(char* p_file, char* p_name, float* param, bool trace) {

  FILE *fp;
  char buff[255];
  char symb[3];

  if((fp = fopen(p_file, "r")) == NULL) {
    printf("No such file\n");
    exit(1);
  }
  
  while(42) {
    int ret = fscanf(fp, "%s", buff);
    if(ret == 1) {

      if (buff[0] == '#') {
	fgets(buff, 255, fp);
      } else if (strcmp(buff,p_name)==0) {
	if( fscanf(fp, "%s %f", symb, param) != 2 ) {
	  printf("No match\n"); fclose(fp); exit(1);
	} else if (strcmp(symb,":=") != 0) {
	  printf("No match\n"); fclose(fp); exit(1);	  
	} else {
	  if (trace) {
	    printf("Read value %s = %f\n",p_name,*param);
	  } break;
	}
      }
    }
    else if(errno != 0) {
      perror("scanf:");
      fclose(fp);
      exit(1);
    } else if(ret == EOF) {
      printf("Couldn't find %s in %s.\n",p_name,p_file);
      break;
    }
  }

  fclose(fp);

}

void get_int_param(char* p_file, char* p_name, int* param, bool trace) {

  FILE *fp;
  char buff[255];
  char symb[3];

  if((fp = fopen(p_file, "r")) == NULL) {
    printf("No such file\n");
    exit(1);
  }
  
  while(42) {
    int ret = fscanf(fp, "%s", buff);
    if(ret == 1) {

      if (buff[0] == '#') {
	fgets(buff, 255, fp);
      } else if (strcmp(buff,p_name)==0) {
	if( fscanf(fp, "%s %d", symb, param) != 2 ) {
	  printf("No match\n"); fclose(fp); exit(1);
	} else if (strcmp(symb,":=") != 0) {
	  printf("No match\n"); fclose(fp); exit(1);	  
	} else {
	  if (trace) {
	    printf("Read value %s = %d\n",p_name,*param);
	  } break;
	}
      }
    }
    else if(errno != 0) {
      perror("scanf:");
      fclose(fp);
      exit(1);
    } else if(ret == EOF) {
      printf("Couldn't find %s in %s.\n",p_name,p_file);
      break;
    }
  }

  fclose(fp);

}


void get_str_param(char* p_file, char* p_name, char* param, bool trace) {

  FILE *fp;
  char buff[255];
  char symb[3];

  if((fp = fopen(p_file, "r")) == NULL) {
    printf("No such file\n");
    exit(1);
  }
  
  while(42) {
    int ret = fscanf(fp, "%s", buff);
    if(ret == 1) {

      if (buff[0] == '#') {
	fgets(buff, 255, fp);
      } else if (strcmp(buff,p_name)==0) {
	if( fscanf(fp, "%s %s", symb, param) != 2 ) {
	  printf("No match\n"); fclose(fp); exit(1);
	} else if (strcmp(symb,":=") != 0) {
	  printf("No match\n"); fclose(fp); exit(1);	  
	} else {
	  if (trace) {
	    printf("Read value %s = %s\n",p_name,param);
	  } break;
	}
      }
    }
    else if(errno != 0) {
      perror("scanf:");
      fclose(fp);
      exit(1);
    } else if(ret == EOF) {
      printf("Couldn't find %s in %s.\n",p_name,p_file);
      break;
    }
  }

  fclose(fp);

}

void get_bool_param(char* p_file, char* p_name, bool* param, bool trace) {

  FILE *fp;
  char buff[255];
  char symb[3], val[6];

  if((fp = fopen(p_file, "r")) == NULL) {
    printf("No such file\n");
    exit(1);
  }
  
  while(42) {
    int ret = fscanf(fp, "%s", buff);
    if(ret == 1) {

      if (buff[0] == '#') {
	fgets(buff, 255, fp);
      } else if (strcmp(buff,p_name)==0) {
	if( fscanf(fp, "%s %s", symb, val) != 2 ) {
	  printf("No match\n"); fclose(fp); exit(1);
	} else if (strcmp(symb,":=") != 0) {
	  printf("No match\n"); fclose(fp); exit(1);	  
	} else {
	  if (strcmp(val,"true")==0) {*param = true;}
	  if (strcmp(val,"false")==0) {*param = false;}	  
	  if (trace) {
	    printf("Read value %s = %s\n",p_name,val);
	  } break;
	}
      }
    }
    else if(errno != 0) {
      perror("scanf:");
      fclose(fp);
      exit(1);
    } else if(ret == EOF) {
      printf("Couldn't find %s in %s.\n",p_name,p_file);
      break;
    }
  }

  fclose(fp);

}

int main(int argc, char* argv[]) {

  int seed = time(NULL);
  float blahnum;
  char mystring[64];
  int myint;
  bool debug_trace = true;
  char paramname[]="blah", strname[]="teststr", intname[]="testint";
  char dtrace[] = "debug_trace";
  
  srand(seed);

  blahnum = 0.0;
  get_bool_param(argv[1], dtrace, &debug_trace, true);
  get_real_param(argv[1], paramname, &blahnum, debug_trace);
  get_str_param(argv[1], strname, mystring, debug_trace);
  get_int_param(argv[1], intname, &myint, debug_trace);    
  
}
