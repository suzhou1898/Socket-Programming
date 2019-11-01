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
double lkid,bdwt,leng,vlct,nspw,size,power,tf,tp,te;
bool fa,fb,fab;
int newsock1,newsock2;
int client_port=25142;
int monitor_port=26142;
int D_port=24142;


int main()
{
	//the part regarding how to establish the TCP and UDP connection is referred from the webpage https://www.youtube.com/watch?v=Emuw71lozdA
    //create sockets and prepare the essential info before using sockets
	int client_socket=socket(AF_INET, SOCK_STREAM,0);
	struct sockaddr_in client_add;
	bzero(&client_add,sizeof(client_add));
	client_add.sin_family=AF_INET;
	client_add.sin_port=htons(25142);
	client_add.sin_addr.s_addr=inet_addr("127.0.0.1");

	int monitor_socket=socket(AF_INET, SOCK_STREAM,0);
	struct sockaddr_in monitor_add;
	bzero(&monitor_add,sizeof(monitor_add));
	monitor_add.sin_family=AF_INET;
	monitor_add.sin_port=htons(26142);
	monitor_add.sin_addr.s_addr=inet_addr("127.0.0.1");

	
	struct sockaddr_in A_server;
	socklen_t fromAlen=sizeof(A_server);
	bzero(&A_server,fromAlen);
	A_server.sin_family=AF_INET;
	A_server.sin_port=htons(21142);
	A_server.sin_addr.s_addr=inet_addr("127.0.0.1");

	
	struct sockaddr_in B_server;
	socklen_t fromBlen=sizeof(B_server);
	bzero(&B_server,fromBlen);
	B_server.sin_family=AF_INET;
	B_server.sin_port=htons(22142);
	B_server.sin_addr.s_addr=inet_addr("127.0.0.1");

	
	struct sockaddr_in C_server;
	socklen_t fromClen=sizeof(C_server);
	bzero(&C_server,fromClen);
	C_server.sin_family=AF_INET;
	C_server.sin_port=htons(23142);
	C_server.sin_addr.s_addr=inet_addr("127.0.0.1");

	int D_socket=socket(AF_INET, SOCK_DGRAM,0);
	struct sockaddr_in D_server;
	socklen_t fromDlen=sizeof(D_server);
	bzero(&D_server,fromDlen);
	D_server.sin_family=AF_INET;
	D_server.sin_port=htons(24142);
	D_server.sin_addr.s_addr=inet_addr("127.0.0.1");

	struct sockaddr_in cli_addr;
	socklen_t clilen=sizeof(cli_addr);

	struct sockaddr_in mon_addr;
	socklen_t monlen=sizeof(mon_addr);

    //judge if the sockets are created properly
	if((client_socket<0)||(monitor_socket<0)||(D_socket<0))
	{
		perror("error\n");
		cout<<"AWS Server failed to create the sockets"<<endl;
	}
	else
	{
		cout<<"AWS Server created the sockets successfully"<<endl; 
	}

    //judge if the sockets are bound properly
	if (bind(client_socket,(struct sockaddr*) &client_add, sizeof(client_add))>-1 && bind(monitor_socket,(struct sockaddr*) &monitor_add, sizeof(monitor_add))>-1 && bind(D_socket,(struct sockaddr*) &D_server, sizeof(D_server))>-1)
	{
		cout<<"The AWS bound the sockets successfully"<<endl;
		cout<<"The AWS is up and running"<<endl;		
	}

	else 
	{	
		cout<<"The AWS failed to bind the sockets"<<endl;
	}

    //keep listening
	for(;;)
	{
		listen(client_socket,10);
		listen(monitor_socket,10);

		newsock1=accept(client_socket, (struct sockaddr *) &cli_addr, &clilen);
		newsock2=accept(monitor_socket, (struct sockaddr *) &mon_addr, &monlen);

	    //judge if both requests from the client and monitor are accepted properly
		if (newsock1>-1 && newsock2>-1)
		{

			cout<<"The AWS established the TCP connection to the client"<<endl;
			cout<<"The AWS established the TCP connection to the monitor"<<endl;
		}


        //receive the link ID, file size and power from the client
		if(recv(newsock1,&lkid, sizeof(lkid),0)>-1)
		{
			recv(newsock1,&size, sizeof(size),0);
			recv(newsock1,&power, sizeof(power),0);
			cout<<"The AWS received link ID=<"<<lkid<<">, size=<"<<size<<">bits, and power=<"<<power<<">db from the client using TCP over port <"<<client_port<<">"<<endl;
		}

        //send the link ID, file size and power to the monitor
		if(send(newsock2,&lkid, sizeof(lkid),0)>-1)
		{
			send(newsock2,&size, sizeof(size),0);
			send(newsock2,&power, sizeof(power),0);
			cout<<"The AWS sent link ID=<"<<lkid<<">, size=<"<<size<<">bits, and power=<"<<power<<">db to the monitor using TCP over port <"<<monitor_port<<">"<<endl;
		}
		
		//send the lind ID to the A Server
		if(sendto(D_socket,&lkid,sizeof(lkid),0, (struct sockaddr*) &A_server, sizeof(A_server))>-1)
		{
			cout<<"The AWS sent link ID=<"<<lkid<<"> to Backend-Server <A> using UDP over port <"<<D_port<<">"<<endl;

            //judge if there is a match for the link based on the flag received from A server
			if(recvfrom(D_socket,&fa, sizeof(fa),0, (struct sockaddr*) &A_server, &fromAlen)>-1)
			{
                //if there is a match, then receive the link info from A Server
				if (fa)
				{	
					cout<<"The AWS received <"<<fa<<"> match from Backend-Server <A> using UDP over port <"<<D_port<<">"<<endl;
					recvfrom(D_socket,&bdwt, sizeof(bdwt),0, (struct sockaddr*) &A_server, &fromAlen);
					recvfrom(D_socket,&leng, sizeof(leng),0, (struct sockaddr*) &A_server, &fromAlen);
					recvfrom(D_socket,&vlct, sizeof(vlct),0, (struct sockaddr*) &A_server, &fromAlen);
					recvfrom(D_socket,&nspw, sizeof(nspw),0, (struct sockaddr*) &A_server, &fromAlen);
				}
				else
				{
					cout<<"The AWS received <"<<fa<<"> match from Backend-Server <A> using UDP over port <"<<D_port<<">"<<endl;	
				}
			}
		}
 
        //send the lind ID to the B Server
		if(sendto(D_socket,&lkid,sizeof(lkid),0, (struct sockaddr*) &B_server, sizeof(B_server))>-1)
		{
			cout<<"The AWS sent link ID=<"<<lkid<<"> to Backend-Server <B> using UDP over port <"<<D_port<<">"<<endl;


            //judge if there is a match for the link based on the flag received from B server
			if(recvfrom(D_socket,&fb, sizeof(fb),0, (struct sockaddr*) &B_server, &fromBlen)>-1)
			{
                //if there is a match, then receive the link info from B Server
				if (fb)
				{
					cout<<"The AWS received <"<<fb<<"> match from Backend-Server <B> using UDP over port <"<<D_port<<">"<<endl;
					recvfrom(D_socket,&bdwt, sizeof(bdwt),0, (struct sockaddr*) &B_server, &fromBlen);
					recvfrom(D_socket,&leng, sizeof(leng),0, (struct sockaddr*) &B_server, &fromBlen);
					recvfrom(D_socket,&vlct, sizeof(vlct),0, (struct sockaddr*) &B_server, &fromBlen);
					recvfrom(D_socket,&nspw, sizeof(nspw),0, (struct sockaddr*) &B_server, &fromBlen);
				}
				else
				{
					cout<<"The AWS received <"<<fb<<"> match from Backend-Server <B> using UDP over port <"<<D_port<<">"<<endl;		
				}
			}
		}

        //send the flag to the client and monitor respectively to tell them if ther is a match for the link
		fab=fa||fb;
		send(newsock1,&fab, sizeof(fab),0);
		send(newsock2,&fab, sizeof(fab),0);

	    //send the link the link ID, file size, power and flag to the C Server
		if(sendto(D_socket,&lkid,sizeof(lkid),0, (struct sockaddr*) &C_server, fromClen)>-1)
		{
			sendto(D_socket,&size,sizeof(size),0, (struct sockaddr*) &C_server, fromClen);
			sendto(D_socket,&power,sizeof(power),0, (struct sockaddr*) &C_server, fromClen);
			sendto(D_socket,&fab,sizeof(fab),0, (struct sockaddr*) &C_server, fromClen);
			cout<<"The AWS sent link ID=<"<<lkid<<">, size=<"<<size<<">bits, and power=<"<<power<<">db and link information to Backend-Server C using UDP over port <24142>"<<endl;	
		}

		
        //if there is a match for the link, then send the link info to the Server C
		if(fab)
		{
			sendto(D_socket,&bdwt,sizeof(bdwt),0, (struct sockaddr*) &C_server, fromClen);
			sendto(D_socket,&leng,sizeof(leng),0, (struct sockaddr*) &C_server, fromClen);
			sendto(D_socket,&vlct,sizeof(vlct),0, (struct sockaddr*) &C_server, fromClen);
			sendto(D_socket,&nspw,sizeof(nspw),0, (struct sockaddr*) &C_server, fromClen);
			

            //receive the final detailed results from the Sever C
			if(recvfrom(D_socket,&tf, sizeof(tf),0, (struct sockaddr*) &C_server, &fromClen)>-1)
			{
				recvfrom(D_socket,&tp, sizeof(tp),0, (struct sockaddr*) &C_server, &fromClen);
				recvfrom(D_socket,&te, sizeof(te),0, (struct sockaddr*) &C_server, &fromClen);
				cout<<"The AWS received outputs from Backend-Server C using UDP over port <"<<D_port<<">"<<endl;	
			}

            //send the end-to-end delay to the client
			if(send(newsock1,&te, sizeof(te),0)>-1)
			{
				printf("The AWS sent delay=<%.2e>ms to the client using TCP over port <%d>\n",te,client_port);
			}

			//send the final detailed results to the monitor
			if(send(newsock2,&tf, sizeof(tf),0)>-1) 
			{   
				send(newsock2,&tp, sizeof(tp),0);
				send(newsock2,&te, sizeof(te),0);
				cout<<"The AWS sent detailed results to the monitor using TCP over port <"<<monitor_port<<">"<<endl;
			}			
		}

		else
		{
			cout<<"The AWS sent No Match to the monitor and the client using TCP over ports <"<<monitor_port<<"> and <"<<client_port<<">, respectively"<<endl;
		}
        //close the TCP sockets
		close(newsock1);
		close(newsock2);
	}
    //close the UDP socket
	close(D_socket);
	return (0);
}
		
	
	

		






			





		

	
	


