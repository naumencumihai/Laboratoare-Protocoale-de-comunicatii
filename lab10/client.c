#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

#define HOST_IP "34.241.4.235"
#define HOST_PORT 8080
#define HOST_ADDR "ec2-34-241-4-235.eu-west-1.compute.amazonaws.com"
#define API_ADDR "api.openweathermap.org"
#define API_PORT 80
#define DUMMY "/api/v1/dummy"
#define TYPE "application/x-www-form-urlencoded"
#define LOGIN "/api/v1/auth/login"
#define KEY_ADDR "/api/v1/weather/key"
#define WEATHER "/data/2.5/weather"
#define WEATHER_IP "37.139.20.5"
#define COOKIE "connect.sid=s%3AXqwhgVHZ2Omz4d56zZQD8q2JSQrIP0Ku.Z1%2F6Q5bCTHW3bVptWFBSUhb7jL4ZyMcc60dqRJz0h18"
#define KEY "b912dd495585fbf756dc6d8f415a7649"


int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

    sockfd = open_connection(HOST_IP, HOST_PORT, AF_INET, SOCK_STREAM, 0);

    // Ex 1.1: GET dummy from main server
    message = compute_get_request(HOST_ADDR, DUMMY, NULL, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    puts("\n");

    // Ex 1.2: POST dummy and print response from main server
    char **body_data;
    body_data = (char**)malloc(2 * sizeof(char *));
    body_data[0] = strdup("a=2");
    body_data[1] = strdup("b=5");
    int body_data_fields_count = 2;
    message = compute_post_request(HOST_ADDR, DUMMY, TYPE, 
                                                            body_data, 
                                                            body_data_fields_count,
                                                            NULL,
                                                            0);

    puts(message);
    send_to_server(sockfd, message);
    memset(response, 0, LINELEN);
    response = receive_from_server(sockfd);
    puts(response);
    puts("\n");

    // Ex 2: Login into main server
    char **body_data2;
    body_data2 = (char **)calloc(2, sizeof(char *));
    body_data2[0] = strdup("username=student");
    body_data2[1] = strdup("password=student");
    body_data_fields_count = 2;

    message = compute_post_request(HOST_ADDR, LOGIN, TYPE,
                                                            body_data2,
                                                            body_data_fields_count,
                                                            NULL,
                                                            0);

    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    puts("\n");

    // Ex 3: GET weather key from main server
    char **cookies = (char**)calloc(1, sizeof(char*));
    cookies[0] = strdup(COOKIE);
    int no_of_cookies = 1;
    message = compute_get_request(HOST_ADDR, KEY_ADDR, NULL, cookies, no_of_cookies);
    puts(message);
    send_to_server(sockfd, message);
    memset(response, 0, LINELEN);
    response = receive_from_server(sockfd);
    puts(response);
    puts("\n");

    // Ex 4: GET weather data from OpenWeather API
    int sockfd_weather = open_connection(WEATHER_IP, API_PORT, AF_INET, SOCK_STREAM, 0);
    
    char *query_params = (char*)malloc(150);
    char *lat = (char *)malloc(20);
    char *lon = (char *)malloc(20);
    char *appid = (char *)malloc(50);
    strcpy(lat, "lat=44.426765&");
    strcpy(lon, "lon=26.102537&");
    strcpy(appid, "appid=b912dd495585fbf756dc6d8f415a7649");

    // concat pe astea 3


    message = compute_get_request(API_ADDR, WEATHER, query_params, NULL, 0);

    // Ex 5: POST weather data for verification to main server
    // Ex 6: Logout from main server

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    return 0;
}
