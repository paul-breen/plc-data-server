/******************************************************************************
* PROJECT:  Network comms library                                             *
* MODULE:   nw_comms.c                                                        *
* PURPOSE:  Functions for performing network comms                            * 
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-06-28                                                        *
******************************************************************************/

#include "nw_comms.h"

/******************************************************************************
* Function to open a TCP/IP client socket connection                          *
*                                                                             *
* Pre-condition:  The server host name (or IP address) and port number are    *
*                 passed to the function                                      *
* Post-condition: Socket connection is established with server, socket file   *
*                 descriptor is returned or -1 on error                       *
******************************************************************************/
int open_client_socket(char *host, unsigned short int port)
{
  int sockfd = -1;
  struct hostent *hostinfo = NULL;

  /* Create and connect to socket */
  if((hostinfo = (struct hostent *) gethostbyname(host)))
  {
    sockfd = create_client_network_socket(hostinfo, port);
  }

  return sockfd;
}



/******************************************************************************
* Function to open a TCP/IP server socket connection                          *
*                                                                             *
* Pre-condition:  The server host name (or IP address) and port number are    *
*                 passed to the function                                      *
* Post-condition: Socket is bound to the server address, socket file          *
*                 descriptor is returned or -1 on error                       *
******************************************************************************/
int open_server_socket(char *host, unsigned short int port)
{
  int sockfd = -1;
  struct hostent *hostinfo = NULL;

  /* Create and bind the socket */
  if((hostinfo = (struct hostent *) gethostbyname(host)))
  {
    sockfd = create_server_network_socket(hostinfo, port);
  }

  return sockfd;
}



/******************************************************************************
* Function to connect client to network server socket                         *
*                                                                             *
* Pre-condition:  A valid server host info struct and server port are passed  *
*                 to the function                                             *
* Post-condition: A connected socket file descriptor to the server or a -1 on *
*                 error is returned                                           *
******************************************************************************/
int create_client_network_socket(struct hostent *hostinfo,
                                 unsigned short int port)
{
  struct sockaddr_in address;
  int sockfd = -1, len = 0, result = 0;

  /* Create a socket for the client */
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) != -1)
  {
    /* Name the socket, as agreed with the server */
    address.sin_family = AF_INET;
    address.sin_addr = *(struct in_addr*)*hostinfo->h_addr_list;
    address.sin_port = htons(port);
    len = sizeof(address);

    /* Connect client socket to the server socket */
    if((result = connect(sockfd, (struct sockaddr *)&address, len)) == -1)
    {
      close(sockfd);
      sockfd = result;
    }
  }

  return sockfd;
}



/******************************************************************************
* Function to create and name a network server socket                         *
*                                                                             *
* Pre-condition:  A valid server host info struct and server port are passed  *
*                 to the function                                             *
* Post-condition: A named socket file descriptor for the server or a -1 on    *
*                 error is returned                                           *
******************************************************************************/
int create_server_network_socket(struct hostent *hostinfo,
                                 unsigned short int port)
{
  struct sockaddr_in address;
  int sockfd = -1, len = 0, result = 0, sopt = 0;

  /* Create an unnamed socket for the server */
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) != -1)
  {
    /* Setup the server socket address struct */
    address.sin_family = AF_INET;
    address.sin_addr = *(struct in_addr*)*hostinfo->h_addr_list;
    address.sin_port = htons(port);
    len = sizeof(address);

    /* Set the reuse address socket option.  This allows the server to be
       restarted and bound to this IP address/port even if there are previous
       socket comms current on this IP address/port.  It is recommended
       (W. Richard Stevens, UNIX Network Programming 2nd ed., pp. 194-197)
       that all TCP servers set this option */
    sopt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sopt, sizeof(int));
   
    /* Name the socket (bind it to this address) */
    if((result = bind(sockfd, (struct sockaddr *)&address, len)) == -1)
    {
      close(sockfd);
      sockfd = result;
    }
  }

  return sockfd;
}



/******************************************************************************
* Function to display host information on stdout                              *
*                                                                             *
* Pre-condition:  Host info struct is passed to the function                  *
* Post-condition: Contents of the host info struct are displayed on stdout    *
******************************************************************************/
void display_hostinfo(struct hostent *hostinfo)
{
  char **names = NULL, **addrs = NULL;

  printf("Results for host %s:\n", hostinfo->h_name);
  printf("Name: %s\n", hostinfo->h_name);
  printf("Aliases:");

  names = hostinfo->h_aliases;

  while(*names)
  {
    printf(" %s", *names);
    names++;
  }
  puts("");

  /* Check if the host's address type is an IP address */  
  if(hostinfo->h_addrtype != AF_INET)
  {
    puts("not an IP host!");
  }

  printf("IP address(es):"); 

  addrs = hostinfo->h_addr_list;

  while(*addrs)
  {
    printf(" %s", inet_ntoa(*(struct in_addr *) *addrs));
    addrs++;
  }
  puts("");
}



/******************************************************************************
* Function to read data on a particular socket fd                             *
*                                                                             *
* Pre-condition:  Socket fd, a valid buffer, and the length to read are       *
*                 passed to the function                                      *
* Post-condition: Data is stored in buffer, and the number of bytes read is   *
*                 returned or -1 on error                                     *
******************************************************************************/
int socket_read(int fd, unsigned char *buf, long int len)
{
  fd_set fds;
  struct timeval tv;

  FD_ZERO(&fds);
  tv.tv_sec = NW_COMMS_TMO_SECS;
  tv.tv_usec = NW_COMMS_TMO_USECS;
  FD_SET(fd, &fds);

  /* Check that the socket is ready for reading and receive the data */
  if((select((fd + 1), &fds, NULL, NULL, &tv)) < 1)
    return -1;
  else
    return recv(fd, buf, len, 0);
}



/******************************************************************************
* Function to write data on a particular socket fd                            *
*                                                                             *
* Pre-condition:  Socket fd, a valid buffer, and the length to write are      *
*                 passed to the function                                      *
* Post-condition: Data from buffer is written on the socket, and the number   *
*                 of bytes written is returned or -1 on error                 *
******************************************************************************/
int socket_write(int fd, unsigned char *buf, long int len)
{
  fd_set fds;
  struct timeval tv;

  FD_ZERO(&fds);
  tv.tv_sec = NW_COMMS_TMO_SECS;
  tv.tv_usec = NW_COMMS_TMO_USECS;
  FD_SET(fd, &fds);

  /* Check that the socket is ready for writing and send the data */
  if((select((fd + 1), NULL, &fds, NULL, &tv)) < 1)
    return -1;
  else
    return send(fd, buf, len, 0);
}

