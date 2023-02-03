#define _GNU_SOURCE

#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit, atoi, malloc, free */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
// #include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <string.h> /* memmem */
#include <math.h>

#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#define TAG "LOG"

#define DEBUG_LOG(format, ...) (esp_log_write(2, TAG, format, ##__VA_ARGS__))

void error(const char *msg) { perror(msg); exit(0); }

int str2int(char* str){
  int out = 0;
  int base = 1;
  int str_len = strlen(str);
  /* For each char in string, subtract 0 from an ASCII 
  representation of an integer, which gets it its integer value*/
  for(int i = (str_len-1); i>=0; i--){
    out += (str[i] - '0') * base;
    /* get the value in the 1s place then the 10s place*/
    base *= 10;
  }
  return out;
}

int get_humidity(int zip_code)
{
    /* Default http port number */
    int portno = 80;
    char *host = "api.weatherapi.com";

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char* response = malloc(4096);

    char* format = "GET /v1/current.json?key=a950a65fae6049bfbef223651231202&q=%d HTTP/1.1\r\nHost: api.weatherapi.com\r\ncache-control: no-cache\r\n\r\n";
    size_t message_size = snprintf(NULL, 0, format, zip_code) + 1;
    char* request = malloc(message_size);
    snprintf(request, message_size, format, zip_code);

    DEBUG_LOG("\n%s\n", request);

    /* Create socket to recieve data */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // TODO: DEBUG_LOG("%d", sockfd);
    if (sockfd < 0) error("ERROR opening socket");

    /* Get server ip */
    server = gethostbyname(host);
    if (server == NULL) error("ERROR, no such host");
 
    /* Zero out structure */
    memset(&serv_addr,0,sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    /* host to network short, converts port number to big-endian or "network" byte order */
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    total = strlen(request);
    sent = 0;
    /* Write GET request to socketfd */
    do {
        bytes = write(sockfd,request+sent,total-sent);
        if (bytes < 0){
          error("ERROR writing message to socket");
        }
        if (bytes == 0){
          break;
        }
        sent+=bytes;
    } while (sent < total);

    /* Zero out response array as it will store strings */
    memset(response,0,4096);
    /* Total bytes recieved */
    total = sizeof(response)-1; 

    received = read(sockfd, response, 4096);
    if (received == total)
        error("ERROR storing complete response from socket");

    close(sockfd);

    /* Parse the response, strip the header.
    Http headers end with two CRLFs */
    char* header_end = (char*)((uint64_t)memmem(response, received, "\r\n\r\n", 4) + 9);
    /* Get length of response without the header */
    int header_len = (uint64_t)header_end - (uint64_t)response; 
    char* response_data = malloc(received-header_len);
    /* Copy the response without headers into a new buffer*/
    memcpy(response_data, header_end, strlen(header_end));

    /* Search for humidity substring- offset by 11 bytes to get just the value */
    char* humidity_begin = (char*)((uint64_t)memmem(response_data, strlen(response_data), "\"humidity\":", 11) + 11);
    if(humidity_begin == NULL){
      error("Couldn't find humidity");
    }
    /* Search for the comma that indicates the end of the humidity value */
    char* humidity_end = memmem(humidity_begin, 8, ",", 1);
    /* Get length of the humidity value */
    int humidity_len = (uint64_t)humidity_end - (uint64_t)humidity_begin;
    char* humidity_str = malloc(humidity_len+1);
    memset(humidity_str, 0, humidity_len+1);

    memcpy(humidity_str, humidity_begin, humidity_len);

    /* Convert string to int with simple conversion algorithm */
    int humidity = str2int(humidity_str);

    //TODO: DEBUG_LOG("\n%s\n",response);

    return humidity;
}