//============================================================================
// Name        : file_client.cpp
// Author      : Lars Mortensen
// Version     : 1.0
// Description : file_client in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iknlib.h>

using namespace std;

void receiveFile(string fileName, int socketfd);

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
       cout << "Client started" <<endl;

       struct sockaddr_in serv_addr;
       struct hostent *server;

       char buffer[1000];

       if (argc < 3) {
          fprintf(stderr,"usage %s hostname port\n", argv[0]);
          exit(0);
       }
       portno = 9000;
       sockfd = socket(AF_INET, SOCK_STREAM, 0);
       if (sockfd < 0)
           error("ERROR opening socket");
       server = gethostbyname(argv[1]);
       if (server == NULL) {
           fprintf(stderr,"ERROR, no such host\n");
           exit(0);
       }
       bzero((char *) &serv_addr, sizeof(serv_addr));
       serv_addr.sin_family = AF_INET;
       bcopy((char *)server->h_addr,
            (char *)&serv_addr.sin_addr.s_addr,
            server->h_length);
       serv_addr.sin_port = htons(portno);
       if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
           error("ERROR connecting");
       printf("Connection complete \n ");


       // Applikationslag
       writeTextTCP(sockfd,argv[2]); // send user-requested filename to server

       receiveFile(argv[2], sockfd); // enter function to receive file

       close(sockfd);
      // return 0;
}

/**
 * Modtager filstørrelsen og udskriver meddelelsen: "Filen findes ikke" hvis størrelsen = 0
 * ellers
 * Åbnes filen for at skrive de bytes som senere modtages fra serveren (HUSK kun selve filnavn)
 * Modtag og gem filen i blokke af 1000 bytes indtil alle bytes er modtaget.
 * Luk filen, samt input output streams
 *
 * @param fileName Det fulde filnavn incl. evt. stinavn
 * @param sockfd Stream for at skrive til/læse fra serveren
 */
void receiveFile(string fileName, int sockfd)
{
    long filesize = getFileSizeTCP(sockfd); // receive filesize from server


    if (filesize == 0)
    {
        cout << "file could not be found" << endl; // if received filesize = 0 the file does not exist
    }
    else
    {
        FILE *fp = fopen((fileName).c_str(), "a"); // create file-pointer to file with append-permission
        cout<<"Filesize: "<<filesize<<endl;
        char readbuffer[1000] = {0};
        int bytesRecived = 0;
        int totalRecieved = 0;

        while(totalRecieved < filesize)
        {
             bytesRecived = read(sockfd, readbuffer, 1000); // read 1000 bytes from server
             totalRecieved += bytesRecived;
             fwrite(readbuffer, 1, bytesRecived, fp); //write 1000 bytes to file
        }
    }
}

