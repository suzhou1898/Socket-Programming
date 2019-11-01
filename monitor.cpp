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
double lkid,size,powerdb;
double tf,tp,te;
bool fab;
bool fc=true;

int main()
{
	//the part regarding how to establish the TCP and UDP connection is referred from the webpage https://www.youtube.com/watch?v=Emuw71lozdA
	//prepare the essential info before using sockets
	struct sockaddr_in AWS_server;
	socklen_t serverlen=sizeof(AWS_server);
	AWS_server.sin_family=AF_INET;
	AWS_server.sin_addr.s_addr=inet_addr("127.0.0.1");
	AWS_server.sin_port=htons(26142);

	for(;;)
	{
		//create the TCP socket to communicate with AWS
		int monitor_socket=socket(AF_INET, SOCK_STREAM, 0);

        //judge if the parameters are input properly 
        if(connect(monitor_socket, (struct sockaddr*) &AWS_server, serverlen)>-1)
		{
			//the boot message only is printed when it is the first time to enter the loop
			if(fc)
			{
				fc=false;
				cout<<"The monitor is up and running"<<endl;
				cout<<"The monitor sent the TCP connection request to the AWS successfully"<<endl;
			}

		}

		else
		{
			cout<<"The monitor failed to establish the TCP connection to the AWS"<<endl;
			break;	
		}


        ////send the link ID, file size and power to the AWS
		if(recv(monitor_socket,&lkid, sizeof(lkid),0)>-1)
		{
			recv(monitor_socket,&size, sizeof(size),0);
			recv(monitor_socket,&powerdb, sizeof(powerdb),0);
			cout<<"The monitor received link ID=<"<<lkid<<">, size=<"<<size<<">bits, and power=<"<<powerdb<<"> from the AWS"<<endl;

		}
		else 
		{
			cout<<"The monitor failed to establish the TCP connection to the AWS"<<endl;
			break;
		}

        //judge if there is a match for the link in Server A or Server B
		recv(monitor_socket,&fab, sizeof(fab),0);
		if (fab)
		{
			if(recv(monitor_socket,&tf, sizeof(tf),0)>-1)
			{
                recv(monitor_socket,&tp, sizeof(tp),0);
                recv(monitor_socket,&te, sizeof(te),0);
				printf("The result for link <%d>:\nTt=<%.2e>ms\nTp=<%.2e>ms\nDelay=<%.2e>ms\n",int(lkid),tf,tp,te);

			}

		}
		else
		{
			cout<<"Found no matches for link <"<<lkid<<">"<<endl;	
		}

        //close the socket
		close(monitor_socket);
	}

return (0);

}











