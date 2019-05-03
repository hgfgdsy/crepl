#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
char buf[4096];
extern char **environ;

int main(int argc, char *argv[]) {
  char name[] = "a-XXXXXX.c";
  char tempname[20];
  int fd = mkstemps(name,2);
  tempname[0] ='.';
  tempname[1] ='/';
  for(int i=0;i<=8;i++) tempname[i+2] = name[i];
  tempname[11] = 's';
  tempname[12]= 'o';
  tempname[13]= '\0';
  close(fd);
  char *my_arg[10] = {"gcc","-fPIC","-shared",name,"-o",tempname};
  while(1){
	  fd = open(name,O_RDWR|O_APPEND);
	  printf(">>");
	  memset(buf,0,sizeof(buf));
	  fgets(buf,4096,stdin);
	  pid_t pid = fork();
	  if(pid==0){
		  if(buf[0]=='i'&&buf[1]=='n'&&buf[2]=='t'&&buf[3]==' '){ 
                          int buflen = strlen(buf);
                          write(fd,buf,buflen);
			  execve("/usr/bin/gcc",my_arg,environ);
		  }
	  }
	  else{
		  wait(NULL);
		  printf("hello\n");
		  void *handle = dlopen(tempname,RTLD_NOW);
		  if(handle==NULL){
			  fprintf(stderr,"%s\n",dlerror());
			  break;
		  }
		  int (*pf)() = (int (*)())dlsym(handle,"g");
		  if(pf!=NULL){
		  int value = pf();
		  printf("value = %d\n",value);}
		  dlclose(handle);
		  close(fd);
	  }
  }
  printf("!!!!\n");
  return 0;
} 
