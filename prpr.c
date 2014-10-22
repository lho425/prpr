//public domain

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

typedef int SOCKET;

enum SOCKET_TYPE { TCP, UDP };

struct sockaddr_in addr;

static SOCKET listen_port(uint16_t port, enum SOCKET_TYPE type ){
  int r;
  int sock;
  switch(type){
  case TCP:
    sock = socket(AF_INET, SOCK_STREAM, 0);
    break;
  case UDP:
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    break;
  default:
    assert(0);
  }

  if(sock == -1){
    return -1;
  }
  
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  r = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  if(r == -1){
    close(sock);
    return -2;
  }
  
  r = listen(sock, 5);//fixme!!! backlog
  if (r == -1){
    close(sock);
    return -3;
  }else {
    return sock;
  }
}

static int accept_connection(SOCKET sock){
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);
  return accept(sock, (struct sockaddr*)&addr, &addrlen);
}

static int spawn_child_process(SOCKET sock_connection, char * const args[]){
  int r;
  r = fork();
  if(r == -1){
    perror("prpr: fork() failed ");
    return -1;
    /*
    while(r == -1){
      perror("prpr: fork() failed ");
      sleep(1);
      fprintf(stderr, "prpr: retry fork()\n");
      r = fork();
      }*/
  }
  
  if(r == 0){
    printf("forked\n");
    dup2(sock_connection, 0);
    dup2(sock_connection, 1);
    close(sock_connection);//fixme!!! is this nessesary?
    execvp(args[0], args);//fixme!!! handle error
  } else {
    return r;
  }
}


/* static int spawn_child_process(SOCKET sock_connection, char * const args[]){ */

/*   dup2(sock_connection, 0); */
/*   //close(sock_connection); */
/*   dup2(sock_connection, 1); */
/*   printf("HELLO. This is prpr.\n"); */
/*   fflush(stdout); */
/*   execvp(args[0], args);//fixme!!! handle error */
/* } */


void print_usage(FILE *out){
  fprintf(out, "Usage:");
  fprintf(out, " prpr port command [args...]\n");
  return;
}

static struct {
  int udp;
  int help;
} options = {0};

// return (count of options)
// return -1: error
static int parse_options(int argc, char** argv){
  int options_count = 0;
  
  int i;
  for(i=1; i<argc; i++){
    if(argv[i][0] == '-'){

      options_count++;
      
      switch(argv[i][1]){
      case 'u':
	options.udp = 1;break;
      case 'h':
	options.help = 1;break;
      default:
	return -1;
      }//switch

    }else /* not '-' */ {
      return options_count;
    }

  }//for

  return options_count;

}//func

static SOCKET sock_close_when_finish;
static void close_when_finish(SOCKET sock){
  sock_close_when_finish = sock;
}
static void finish(){
  close(sock_close_when_finish);
}
  

int main(int argc, char* argv[]){

  int r;
  char ** command_args;
  char *strtol_endptr;
  uint16_t port;
  SOCKET sock;
  SOCKET sock_connection;
  enum SOCKET_TYPE sock_type;

  printf("TODO: SIGINT\n");
  printf("TODO: SIGPIPE\n");
  printf("TODO: -e\n");//redirect stderr
  printf("TODO: -v\n");//verbose mode
  printf("TODO: IP address of client\n");
  printf("TODO: strtol test\n");
  printf("TODO: win32 port\n");
  printf("TODO: IPv6\n");
  printf("TODO: parse -abc option\n");
  printf("TODO: char* invaild_long_option\n");
  
  r = parse_options(argc, argv);
  if(r == -1){
    fprintf(stderr, "invaild option\n");
  }
  if(argc < 3 || r == -1){
    print_usage(stderr);
    return 1;
  }else if(options.help){
    print_usage(stdout);
    return 0;
  }

  command_args = &argv[r + 2];// prpr -u -u -u 8080 command

  //printf("command: %s\n", command_args);
    
  port = strtol(argv[1], &strtol_endptr, 10);
  if( (*strtol_endptr) != '\0' ){
    fprintf(stderr, "invaild port number\n");
    print_usage(stderr);
    return 1;
  }

  if(options.udp){
    sock_type = UDP;
  }else{
    sock_type = TCP;
  }
  
  sock = listen_port(port, sock_type);
  if(sock<0){
    perror("error in listening port");
    //fprintf(stderr, "prpr:error: listen_port()\n");
    return 1;
  }

  /* printf("STUB\n"); */
  /* return 0; */

  for(;;){
    sock_connection = accept_connection(sock);
    printf("connected\n");
    if( sock_connection <0 ){
      perror("prpr:error: ");
      //fprintf(stderr, "prpr:error: accept_connection()\n");
      continue;
    }

    spawn_child_process(sock_connection, command_args);
    close(sock_connection);
    
  }

}
