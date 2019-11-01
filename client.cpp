#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <math.h>

using namespace std;
double lkid;
double size;
double powerdb;
double te;
bool fab;

int main (int argc, char *argv[])
{
    //the part regarding how to establish the TCP and UDP connection is referred from the webpage https://www.youtube.com/watch?v=Emuw71lozdA
    //create the TCP socket to communicate with AWS
	int client_socket=socket(AF_INET, SOCK_STREAM, 0);

    //prepare the essential info before using sockets
	struct sockaddr_in AWS_server;
	socklen_t serverlen=sizeof(AWS_server);
	bzero(&AWS_server,serverlen);
	AWS_server.sin_family=AF_INET;
	AWS_server.sin_addr.s_addr=inet_addr("127.0.0.1");
	AWS_server.sin_port=htons(25142);

    //input parameters, which includes the link ID, file size and power, from the command line
	lkid=atof(argv[1]);
	size=atof(argv[2]);
	powerdb=atof(argv[3]);

	cout<<"The client is up and running"<<endl;

    //judge if the parameters are input properly 
	if (argc<3)
	{
		cout<<"Input arguments expected"<<endl;
		exit(1);
	}

    //judge if the TCP connection request is sent correctly
	if(connect(client_socket, (struct sockaddr*) &AWS_server, serverlen)>-1)
	{

		cout<<"The client sent the TCP connection request to the AWS successfully"<<endl;

	}
	else
	{
		cout<<"The client failed to send the TCP connection request to the AWS"<<endl;	
	}


    //send the link ID, file size and power to the AWS
	if(send(client_socket, &lkid, sizeof(lkid),0)>-1)
	{
		send(client_socket, &size, sizeof(size),0);
		send(client_socket, &powerdb, sizeof(powerdb),0);
		cout<<"The client sent ID=<"<<lkid<<">, size=<"<<size<<">bits, and power=<"<<powerdb<<">dbm to AWS"<<endl;
	}
	else
	{
		cout<<"The client failed to send the information"<<endl;
	}
    
    //receive the flag from the AWS to judge if there is a match for the link in Server A or Server B
	if(recv(client_socket,&fab, sizeof(fab),0)>-1)
	{
        //if there is a match for the link in Server A or Server B, then receive the final results from the AWS 
		if(fab)
		{
			if(recv(client_socket,&te, sizeof(te),0)>-1)
			{
				printf("The delay for link <%d> is <%.2e>ms\n",int(lkid),te);
			}
		}
		else
		{
			cout<<"Found no matches for link <"<<lkid<<">" <<endl;		
		}
	}

    //close the socket
	close(client_socket);

	return (0);
}

