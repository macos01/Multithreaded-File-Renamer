#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

#define MAX_FILES_IN_QUEUE 5
#define MAX_FILENAME_LENGTH 150

#define READ 0
#define WRITE 1

void *get_file_to_fix(void *arg);

int fix_name ();
int search_metadata();

void *rename_by_metadata (void *arg);
void *rename_by_name (void *arg);
void socket_renaming_client();
void socket_renaming_client2();
void auto_renamer();

int pipe_name[2];
int pipe_metadata[2];

pthread_mutex_t mutex_metadata;
pthread_mutex_t mutex_name;
pthread_cond_t queue_cond_metadata;
pthread_cond_t queue_cond_name;

char *work_directory = "/home/usuarioso/renamer";
char *input_directory = "./input";

char queue_fix_by_name[MAX_FILES_IN_QUEUE][MAX_FILENAME_LENGTH];
char queue_fix_by_metadata[MAX_FILES_IN_QUEUE][MAX_FILENAME_LENGTH];

int nQueue_rename_by_name = 0;
int nQueue_rename_by_metadata = 0;

int main (int argc, char *argv[]) {
  
  if(chdir(work_directory)){
    perror("Chdir main");
    exit(1);
  }
  
 
  if(pipe(pipe_metadata) == -1){
    perror("Pipe metadata error");
    exit(1);
  }
  
  pid_t pid;
  pid = fork();
  
  if (pid > 0) {
      close(pipe_metadata[0]);
      close(pipe_name[0]);
      auto_renamer();
      //close(pipe_metadata[1]);
      //close(pipe_name[1]);
      
  } else { //child
      close(pipe_metadata[1]);
      close(pipe_name[1]);
      socket_renaming_client();
      socket_renaming_client2();
      //close(pipe_name[0]);
      //close(pipe_metadata[0]);
  }
  
  //   if (close(pipe_metadata[WRITE]) != 0){
    //     perror("Error close pipe_metadata WRITE rename_by_metadata");
    //     exit(errno);
    //   }
    // 	
    //   if (close(pipe_metadata[READ]) != 0){
      //     perror("Error close pipe_metadata READ on test_pipe");
      //     exit(errno);
      //   }
      
      
      return 0;
}

void socket_renaming_client(){
  
  int result2 = 0;
  char received[MAX_FILENAME_LENGTH];

  //all time reading messages..
  while(1){
    
    while ((result2 = read(pipe_metadata[READ],received, sizeof(received))) > 0){
      printf("%lu: Received: *** %s ***\n",(unsigned long)time(NULL),received);
    }
   
    if (result2 < 0) {
      perror("read pipes READ on socket");
      exit(1);
    }
  }
  
}

void auto_renamer(){
  
  pthread_t th1, th2, th3;
  
  pthread_mutex_init (&mutex_metadata, NULL);
  pthread_mutex_init (&mutex_name, NULL);
  pthread_cond_init (&queue_cond_name, NULL);
  pthread_cond_init (&queue_cond_metadata, NULL);
  
  pthread_create (&th1, NULL, rename_by_name, NULL);
  pthread_create (&th2, NULL, rename_by_metadata, NULL);
  pthread_create (&th3, NULL, get_file_to_fix, NULL);
  
 pthread_join (th1, NULL);
  pthread_join (th2, NULL);
  pthread_join (th3, NULL);
  
  pthread_mutex_destroy(&mutex_metadata);
  pthread_mutex_destroy(&mutex_name);
  pthread_cond_destroy(&queue_cond_metadata);
  pthread_cond_destroy(&queue_cond_name);
  
}

void *get_file_to_fix(void *arg){
  
  DIR *dir;
  struct dirent *dp;
  
  dir = opendir(input_directory);
  if (dir == NULL) {
    perror("opendir get_file_to_fix()");
    exit(1);
  }
  else {
    
    while ((dp = readdir(dir)) != NULL) {
      if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) {
	
	sleep(1);
	
	if (!strncmp(dp->d_name,"3",1)) {
	  pthread_mutex_lock (&mutex_name);
	  
	  //fprintf(stdout,"%lu: %s => FIX NAME \n",(unsigned long)time(NULL),dp->d_name);
	  
	  strcpy(queue_fix_by_name[nQueue_rename_by_name],dp->d_name);
	  nQueue_rename_by_name++;
	  
	  pthread_cond_signal (&queue_cond_name);
	  pthread_mutex_unlock (&mutex_name);
	}
	else {
	  pthread_mutex_lock (&mutex_metadata);
	  
	  //fprintf(stdout,"%lu: %s => RENAME BY METADATA \n",(unsigned long)time(NULL),dp->d_name);
	  
	  strcpy(queue_fix_by_metadata[nQueue_rename_by_metadata],dp->d_name);
	  nQueue_rename_by_metadata++;
	  
	  pthread_cond_signal (&queue_cond_metadata);
	  pthread_mutex_unlock (&mutex_metadata);
	}
      }
    }
    
    //fprintf(stdout,"%lu: Fin asignar trabajo \n",(unsigned long)time(NULL));
    closedir(dir);
    
  }
  
  return NULL; 
}

void *rename_by_name (void *arg) {
  
  char filename[MAX_FILENAME_LENGTH];
  int result;
  
  while(1){
    
    pthread_mutex_lock (&mutex_name);
    
    while (nQueue_rename_by_name == 0) {
      pthread_cond_wait (&queue_cond_name, &mutex_name);
    }
    
    strcpy(filename, queue_fix_by_name[nQueue_rename_by_name - 1]);
    nQueue_rename_by_name--;
    
    pthread_mutex_unlock (&mutex_name);
    
    if(fix_name()){
      //ok
    }
    else {
      
      //printf("%lu: Envio datos rename_by_name %s\n",(unsigned long)time(NULL),filename);
      
      result = write(pipe_name[WRITE], filename, strlen(filename) + 1);
      
      if(result != strlen(filename) + 1){
	perror("Error write pipe_name WRITE rename_by_name");
	exit(1);
      }
      else {
	//printf("%lu: Enviado pipe_name %s\n",(unsigned long)time(NULL),filename);
      }
    }
  }
  
  return NULL;
}

void *rename_by_metadata (void *arg) {
  
  char filename[MAX_FILENAME_LENGTH];
  int result = 0;
  
  while(1){
    
    pthread_mutex_lock (&mutex_metadata);
    
    while (nQueue_rename_by_metadata == 0) {
      pthread_cond_wait (&queue_cond_metadata, &mutex_metadata);
    }
    
    strcpy(filename, queue_fix_by_metadata[nQueue_rename_by_metadata - 1]);
    nQueue_rename_by_metadata--;
    
    pthread_mutex_unlock (&mutex_metadata);
    
    if(search_metadata()){
      //printf("ok\n");
    }
    else {
      
      //printf("%lu: Envio datos rename_by_metadata %s\n",(unsigned long)time(NULL),filename);
      
      result = write(pipe_metadata[WRITE], filename, strlen(filename) + 1);
      
      if(result != strlen(filename) + 1){
	perror("Error write pipe_metadata WRITE rename_by_metadata");
	exit(1);
      }
      else {
	//printf("%lu: Enviado pipe_metadata %s\n",(unsigned long)time(NULL),filename);
	// 	  if (close(pipe_metadata[WRITE]) != 0){
	  //     perror("Error close pipe_metadata WRITE rename_by_metadata");
	//     exit(errno);
	//       }
      }
    }
  }
  return NULL;
}

int search_metadata() {
  
  int ret = 0;
  
  //   fprintf(stdout, "%lu: search_metadata 2 SEC \n",(unsigned long)time(NULL));
  sleep (1); 
  int dato = rand () % 99;
  if (dato > 49) {
    ret = 0;
  }
  else {
    ret = 1;
  }
  sleep (1); 
  //   fprintf(stdout, "%lu: search_metadata FIN \n", (unsigned long)time(NULL));
  
  return ret;
}

int fix_name() {
  
  int ret = 0;
  //   fprintf(stdout, "%lu: search_name 2 SEC \n",(unsigned long)time(NULL));
  sleep (1); 
  int dato = rand () % 99;
  if (dato > 49) {
    ret = 0;
  }
  else {
    ret = 1;
  }
  sleep (1); 
  //   fprintf(stdout, "%lu: search_name FIN \n", (unsigned long)time(NULL));
  
  return ret;
}


