#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include "pack109.hpp"

int main(int argc, char **argv)
{
    bool hostFlag = false;
    std::string hostString;
    std::string portNoString;
    char *hostChars = NULL;
    int portNo;
    bool sendFlag = false;
    char *sendChars;
    bool requestFlag = false;
    char *requestChars;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && argv[i][1] == '-')
        { // flag
            if (argv[i][2] == 'h' && argv[i][3] == 'o' && argv[i][4] == 's' && argv[i][5] == 't' && argv[i][6] == 'n' && argv[i][7] == 'a' && argv[i][8] == 'm' && argv[i][9] == 'e')
            {
                // hostname flag
                hostFlag = true;
                i++;
                int z = 0;
                while (argv[i][z] != ':')
                {
                    // printf("argv[%d][%d] is:%c\n",i,z,argv[i][z]);
                    hostString += argv[i][z];
                    z++;
                }
                z++;
                while (argv[i][z] != NULL)
                {
                    // printf("argv[%d][%d] is:%c\n",i,z,argv[i][z]);
                    portNoString += argv[i][z];
                    z++;
                }
                hostChars = (char *)hostString.c_str();
                portNo = stoi(portNoString);
                // printf("hostChars is: %s\nport is: %s\n",hostChars, portNo);
            }
            else if (argv[i][2] == 's' && argv[i][3] == 'e' && argv[i][4] == 'n' && argv[i][5] == 'd')
            {
            // send flag
                sendFlag = true;
                i++;
                sendChars = argv[i];
                // printf("sendChars is: %s\n",sendChars);
            }
            else if (argv[i][2] == 'r' && argv[i][3] == 'e' && argv[i][4] == 'q' && argv[i][5] == 'u' && argv[i][6] == 'e' && argv[i][7] == 's' && argv[i][8] == 't')
            {
            // request flag
                requestFlag = true;
                i++;
                requestChars = argv[i];
                // printf("requestChars is: %s\n",requestChars);
            }
            else
            {
                printf("ERROR: Unknown Flag\n");
                return 1;
            }
        }
    }
    if (!hostFlag)
    { // no host flag - return error
        printf("ERROR: No host flag\n");
        return 1;
    }
    if (sendFlag && requestFlag)
    { // both send and request flag - return error
        printf("ERROR: Both send and request entered\n");
        return 1;
    }
    // unix socket and port
    printf("Connecting to %s:%d.\n", hostChars, portNo);
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket\nDone.\n");
        exit(1);
    }
    server = gethostbyname(hostChars);
    if (server == NULL)
    {
        printf("Failed to connect: failed to lookup address information: nodename nor servename provided, or not known\nDone.\n");
        exit(1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portNo);

    // connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Failed to connect");
        printf("Done.\n");
        exit(1);
    }
    printf("Successfully connected to server.\n");
    int count = 0;
    while (true)
    {
        //printf("count is %d\n",count);
        if (count != 0)
        { // check for input
            printf("Enter a new flag and file name.\n");
            string input;
            std::cin >> input;
            size_t sendPos = input.find("--send");
            size_t requestPos = input.find("--request");
            //printf("sendPos is: %d\n", sendPos);
            if (sendPos != string::npos)
            {
                printf("send flag caught\n");
                std::cin >> input;
                sendChars = (char *)input.c_str();
                sendFlag = true;
            }
            else if (requestPos != string::npos)
            {
                printf("request flag caught\n");
                std::cin >> input;
                requestChars = (char *)input.c_str();
                requestFlag = true;
            }
        }
        if (sendFlag)
        { // send a file
            // open file and read to memory
            FILE *fileP;
            long lSize;
            char *buffer;
            size_t result;
            printf("Sending File \"%s\".\n", sendChars);
            fileP = fopen(sendChars, "rb");
            if (fileP == NULL)
            {
                printf("Failed to open file: \"%s\"\nDone.\n", sendChars);
                exit(1);
            }
            fseek(fileP, 0, SEEK_END);
            lSize = ftell(fileP);
            printf("Read File: %d bytes.\n", lSize);
            // printf("lSize is %ld\n",lSize);
            rewind(fileP);
            buffer = (char *)malloc(sizeof(char) * lSize);
            if (buffer == NULL)
            {
                printf("Memory error\n");
                exit(2);
            }
            result = fread(buffer, 1, lSize, fileP);
            if (result != lSize)
            {
                printf("Reading error\n");
                exit(3);
            }
            int counter = 0;
            std::vector<unsigned char> file_bytes;
            while (buffer[counter] != NULL)
            {
                // printf("buffer[%d]: %c\n",counter,buffer[counter]);
                file_bytes.push_back(buffer[counter]);
                counter++;
            }
                // printf("counter is %d\n",counter);
                // printf("file_bytes size is: %d\n",file_bytes.size());
            fclose(fileP);
            free(buffer);
            // store file in File struct
            std::string sendString = std::string(sendChars);
            std::string fileName = sendString.substr(sendString.find_last_of("/") + 1);
                // printf("fileName is: %s\n",fileName.c_str());
            struct File myFile = {.name = fileName, .bytes = file_bytes};
            // serialie using PACK109 protocol
            printf("original file btye size is %d\n", myFile.bytes.size());
            std::vector<unsigned char> serialized_file = pack109::serialize(myFile);
                //printf("serialized file bytes: \n");
                //pack109::printVec(serialized_file);
                    // printf("serialized_file size is: %d\n",serialized_file.size());
                //  pack109::printVec(serialized_file);
                //  File deserialized_file = pack109::deserialize_file(serialized_file);
                //  printf("deserialized...\nname is %s\n bytes is: ",deserialized_file.name.c_str());
                //  pack109::printVec(deserialized_file.bytes);
            //  encrypt
            std::vector<unsigned char> encryptedFile = pack109::encrypt(serialized_file);
                // printf("Sending %d bytes.\n", encryptedFile.size());
                // printf("Sending File \"%s\".\n", sendChars);

            // send byte vector over socket
                // pack109::printVec(encryptedFile);
            unsigned char sendBuffer[encryptedFile.size()];
            for (int i = 0; i < encryptedFile.size(); i++)
            {
                sendBuffer[i] = encryptedFile[i];
                // printf("encryptedFile[%d] is: %d\n",i,encryptedFile[i]);
                // printf("sendBuffer[%d] is: %d\n",i,sendBuffer[i]);
            }
            printf("Sending %d bytes.\n", encryptedFile.size());
                // pack109::printVec(encryptedFile);
                // printf("size is %d\n",encryptedFile.size());
            n = write(sockfd, sendBuffer, encryptedFile.size());
            if (n < 0)
            {
                perror("ERROR writing to socket\n");
                printf("Done.\n");
                exit(1);
            }

            printf("Message sent\nDone.\n");
        }
        else if (requestFlag)
        { // request a file
            // store into request struct
            std::string requestString = std::string(requestChars);
            std::string requestName = requestString.substr(requestString.find_last_of("/") + 1);
            struct Request myRequest = {.name = requestName};
            // serialize
            std::vector<unsigned char> serializedRequest = pack109::serialize(myRequest);
            // encrypt
            std::vector<unsigned char> encryptedRequest = pack109::encrypt(serializedRequest);
            // send encypted byte vector over socket
            printf("Requesting file \"%s\".\n", requestChars);
            printf("Sending %d bytes.\n", encryptedRequest.size());
            unsigned char requestBuffer[encryptedRequest.size()];
            for (int i = 0; i < encryptedRequest.size(); i++)
            {
                requestBuffer[i] = encryptedRequest[i];
            }
            n = write(sockfd, requestBuffer, encryptedRequest.size());
            if (n < 0)
            {
                perror("ERROR writing to socket\n");
                printf("Done.\n");
                exit(1);
            }
            printf("Message Sent.\n");
            // Now read server response
            std::vector<unsigned char> encryptedResponse;
            unsigned char responseBuffer[1000];
            bzero(responseBuffer, 1001);
            ssize_t readBytes = 1;
            while (true)
            {
                readBytes = recv(sockfd, responseBuffer, sizeof(responseBuffer), 0);
                if (readBytes > 0)
                {
                    for (int i = 0; i < readBytes; i++)
                    {
                        encryptedResponse.push_back(responseBuffer[i]);
                    }
                    bzero(responseBuffer, sizeof(responseBuffer));
                    break;
                }
            }
                // pack109::printVec(encryptedResponse);
                // printf("size is: %d\n",encryptedResponse.size());
            if (n < 0)
            {
                perror("ERROR reading from socket\n");
                printf("Done.\n");
                exit(1);
            }
            if (encryptedResponse.empty())
            {
                printf("No file received, not stored in server.\n");
                return 0;
            }
            else
            {
                printf("Received %d bytes.\n", encryptedResponse.size());
                // decrypt
                std::vector<unsigned char> decryptedResponse = pack109::encrypt(encryptedResponse);
                    //printf("serialized file requested:");
                    //pack109::printVec(decryptedResponse);
                File responseFile = pack109::deserialize_file(decryptedResponse);
                    // std::vector<unsigned char> testVec{174, 1, 170, 4, 70, 105, 108, 101, 174, 2, 170, 4, 110, 97, 109, 101, 170, 8, 102, 105, 108, 101, 46, 116, 120, 116, 170, 5, 98, 121, 116, 101, 115, 172, 5, 162, 72, 162, 101, 162, 108, 162, 108, 162, 111 };
                    // File testFile = pack109::deserialize_file(testVec);
                // save file in folder called received
                FILE *newFileP;
                string newName = "received/" + responseFile.name;
                newFileP = fopen(newName.c_str(), "wb");
                if (newFileP == NULL)
                {
                    printf("Failed to open file: \"%s\"\nDone.\n", responseFile.name.c_str());
                    exit(1);
                }
                for (int i = 0; i < responseFile.bytes.size(); i++)
                {
                    fputc(responseFile.bytes[i], newFileP);
                }
                fclose(newFileP);
                printf("Saved file in \"%s\"\n", newName.c_str());
            }
            
        }
        requestFlag = false;
        sendFlag = false;
        count++;
       
    }
     return 0;
}