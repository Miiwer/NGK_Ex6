//============================================================================
// Name        : file_server.cpp
// Author      : Lars Mortensen
// Version     : 1.0
// Description : file_server in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iknlib.h>
#include <math.h>

using namespace std;

void sendFile(char *fileName, long fileSize, int outToClient);

/**
 * main starter serveren og venter på en forbindelse fra en klient
 * Læser filnavn som kommer fra klienten.
 * Undersøger om filens findes på serveren.
 * Sender filstørrelsen tilbage til klienten (0 = Filens findes ikke)
 * Hvis filen findes sendes den nu til klienten
 *
 * HUSK at lukke forbindelsen til klienten og filen nÃ¥r denne er sendt til klienten
 *
 * @throws IOException
 *
 */
int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
        cout << "server started" << endl;
         socklen_t clilen;
         char buffer[1000];
         struct sockaddr_in serv_addr, cli_addr;
         int n;
         sockfd = socket(AF_INET, SOCK_STREAM, 0);
         if (sockfd < 0)
            error("ERROR opening socket");
         bzero((char *) &serv_addr, sizeof(serv_addr));
         portno = 9000;
         serv_addr.sin_family = AF_INET;
         serv_addr.sin_addr.s_addr = INADDR_ANY;
         serv_addr.sin_port = htons(portno);
         if (bind(sockfd, (struct sockaddr *) &serv_addr,
                  sizeof(serv_addr)) < 0)
                  error("ERROR on binding");
         while(1)
         {
             listen(sockfd,5);
             clilen = sizeof(cli_addr);
             newsockfd = accept(sockfd,
                         (struct sockaddr *) &cli_addr,
                         &clilen);
             if (newsockfd < 0)
                  error("ERROR on accept");
             cout << "Connected to client" << endl;


             // Applikationslag
             char filename[100] = {0};
             bzero(buffer,1000);
             readTextTCP(filename,100,newsockfd); // reads from filename from client

             printf("the client wants this file: %s\n",filename);

             long filesize = check_File_Exists(filename); // checks if file exists and returns size
             sprintf(buffer, "%ld", filesize);

             writeTextTCP(newsockfd, buffer); // writes filesize to client
             printf("the file is this big: %s\n",buffer);

             if (filesize > 0) //if file exists send it
             {
                sendFile(filename, filesize, newsockfd);
             }
         else
        cout << "file could not be found" << endl;

             close(newsockfd);
         }
         close(sockfd);
         return 0;

}

/**
 * Sender filen som har navnet fileName til klienten
 *
 * @param fileName Filnavn som s�9
kal sendes til klienten
 * @param fileSize Størrelsen på filen, 0 hvis den ikke findes
 * @param outToClient Stream som der skrives til socket
     */
void sendFile(char *fileName, long fileSize, int outToClient)
{
    char readbuffer[1000]; //
    FILE *fp = fopen(fileName,"rb"); // file-pointer created with read-permission

    int written = 0;
    int writtenTotal = 0;


       while(writtenTotal < (fileSize - 1000))
       {
           fread(readbuffer, 1, 1000, fp); // read 1000 bytes from file-pointer to buffer
           written = write(outToClient, readbuffer, 1000); // write from buffer to client
           writtenTotal += written;
       }

    fread(readbuffer, 1, fileSize % 1000, fp); // read last bytes to buffer
    write(outToClient, readbuffer, (fileSize % 1000)); // send last bytes to client

    fclose(fp);
}

