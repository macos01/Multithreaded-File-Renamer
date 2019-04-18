#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

#define MAX_FILES_IN_LIST 5
#define MAX_FILENAME_LENGTH 150

void *get_file_to_fix(void *arg);
int search_name ();
int search_date ();
void *fix_date (void *arg);
void *fix_name (void *arg);

pthread_mutex_t mutex_date;
pthread_mutex_t mutex_name;
pthread_cond_t queue_date;
pthread_cond_t queue_name;

//IDEAS:
//Modificar para que el propio programa cree las carpetas
	
char *work_directory = "/home/usuarioso/renamer";
char *input_directory = "./input";
char *output_directory = "./output";
char *manual_directory = "./manual";

char list_fix_name[MAX_FILES_IN_LIST][MAX_FILENAME_LENGTH];
char *list_fix_date[MAX_FILES_IN_LIST];

int nFix_name = 0;
int nFix_date = 0;
int *ptr_list_name = NULL;
int *ptr_list_date = NULL;

int main (int argc, char *argv[]){
  
  //ptr_list_name = (int *) calloc(MAX_FILES_IN_LIST*MAX_FILENAME_LENGTH*sizeof(char));
  //ptr_list_date = (int *) calloc(MAX_FILES_IN_LIST*MAX_FILENAME_LENGTH*sizeof(char));
  
  printf("Renamer\n");
  
  pthread_t th1, th2, th3;
   if(chdir(work_directory)){
    perror("Chdir main");
    exit(errno);
  }
  
  pthread_mutex_init (&mutex_date, NULL);
  pthread_mutex_init (&mutex_name, NULL);
  pthread_cond_init (&queue_name, NULL);
  pthread_cond_init (&queue_date, NULL);
  
  pthread_create (&th1, NULL, fix_name, NULL);
   //pthread_create (&th4, NULL, fix_name, NULL);
  pthread_create (&th2, NULL, fix_date, NULL);
  pthread_create (&th3, NULL, get_file_to_fix, NULL);
  
  pthread_join (th1, NULL);
  pthread_join (th3, NULL);
  pthread_join (th2, NULL);
   //pthread_join (th4, NULL);
 
  pthread_mutex_destroy(&mutex_date);
  pthread_mutex_destroy(&mutex_name);
  pthread_cond_destroy(&queue_date);
  pthread_cond_destroy(&queue_name);
  
  return 0;
}

void *get_file_to_fix(void *arg){
  
  DIR *dir;
  struct dirent *dp;
  
  int j = 0;
  
  dir = opendir(input_directory);
  if (dir == NULL) {
    perror("opendir get_file_to_fix()");
    exit(errno);
  }
  else {
    
    while ((dp = readdir(dir)) != NULL) {
      if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) {
	
	//printf("Archivo a analizar: %s\n",dp->d_name);
	
	//usleep (500000);
	
	if (!strncmp(dp->d_name,"3",1)) {
	  pthread_mutex_lock (&mutex_name);
	  
	  fprintf(stdout, "%lu -> ", (unsigned long)time(NULL)); 
	  fprintf(stdout,"%s => ORDEN BUSCAR NOMBRE: ",dp->d_name);
	  
	  strcpy(list_fix_name[nFix_name],dp->d_name);
	  nFix_name++;
	  
	  printf("%lu -> total de archivos en la lista %u\n", (unsigned long)time(NULL),nFix_name); 
	  
	  
	  pthread_cond_signal (&queue_name);
	  pthread_mutex_unlock (&mutex_name);
	}
	else {
	  pthread_mutex_lock (&mutex_date);
	  
	  fprintf(stdout, "%lu -> ", (unsigned long)time(NULL)); 
	  fprintf(stdout,"%s => ORDEN BUSCAR FECHA \n",dp->d_name);

	  list_fix_date[nFix_date] = dp->d_name;
	  nFix_date++;
	  
	  //printf("total en Fix date: %u \n",nFix_date);
	  
	  pthread_cond_signal (&queue_date);
	  pthread_mutex_unlock (&mutex_date);
	}
	
	
	 for(j = 0; j < nFix_name; j++){
	    printf("%lu -> List name pos %u: %s \n",(unsigned long)time(NULL),j,list_fix_name[j]);
	  }
	
	
      }
    }
   fprintf(stdout, "%lu -> ", (unsigned long)time(NULL)); 
    printf("He terminado de mandar los archivos y los hilos estan a su bola\n");
    closedir(dir);
  }
  
  return NULL;
  
}


void *fix_name (void *arg) {
  
  char filename[MAX_FILENAME_LENGTH];
  
  
  while(1){
    
    pthread_mutex_lock (&mutex_name);
    
    while (nFix_name == 0) {
       //fprintf(stdout, "%lu -> duermo nombre \n", (unsigned long)time(NULL)); 
      pthread_cond_wait (&queue_name, &mutex_name);
    }
   
    strcpy(filename, list_fix_name[nFix_name - 1]);
    nFix_name--;
    
    printf("el archivo a trabajar es %s\n",filename);
    
    pthread_mutex_unlock (&mutex_name);
    
    if(search_name()){
      //lo movemos con el nombre cambiado a output
      
    
      //printf("OK, PASAMOS AL SIGUIENTE\n");
    }
    else {
      //movemos el fichero a manual
      //printf("Le tenemos que pasar el nombre al socket\n");
    }
    
   
    
  }
  
   return NULL;
}


void *fix_date (void *arg) {
  
  while(1){
    
    
    pthread_mutex_lock (&mutex_date);
    
    while (nFix_date == 0) {
      //fprintf(stdout, "%lu -> duermo fecha\n", (unsigned long)time(NULL)); 
      pthread_cond_wait (&queue_date, &mutex_date);
    }
    //tengo el mutex..
    //fprintf(stdout, "%lu -> hago fecha \n", (unsigned long)time(NULL));
    
    //get the filename
    nFix_date--;
    //suelto el tema para hacer mi trabajo
    pthread_mutex_unlock (&mutex_date);
    
    
    //cuando salgo hago lo que tengo que hacer
    search_date();
    
    //si la encuentro....
    
    // si no la encuentro
    
  }
  
   return NULL;
}



int search_date() {
  
  int found = 0;
  fprintf(stdout, "%lu ->", (unsigned long)time(NULL));
  printf
  ("VOY A ARREGLAR LA FECHA. Tardo 2 segundos. \n");
  sleep (1);
  
  int dato = rand () % 99;

  if (dato > 49)
  {
    found = 1;
  }
  else {
    found = 0;
  }
    
  sleep (1);
  fprintf(stdout, "%lu ->", (unsigned long)time(NULL));
  printf ("ARREGLADO LA FECHA\n");
  
  return found;
  
}

int search_name() {
  int found = 0;
  
   fprintf(stdout, "%lu ->", (unsigned long)time(NULL));
  printf
  ("VOY A ARREGLAR EL NOMBRE. Tardo 2 segundos. \n");
  sleep (1);
  
  int dato = rand () % 99;

  if (dato > 49)
  {
    found = 1;
  }
  else {
    found = 0;
  }
  
  sleep(1);
  
   fprintf(stdout, "%lu ->", (unsigned long)time(NULL));
  printf ("ARREGLADO EL NOMBRE\n");
  
  return found;
}



