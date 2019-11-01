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
double bdwt;
double leng;
double vlct;
double nspwdb;
double powerdb;
double size;
double powerw;
double nspww;
double cpct;
double tf;
double tp;
double te;
bool fab;
int C_port=23142;

int main() 
{
    //the part regarding how to establish the TCP and UDP connection is referred from the webpage https://www.youtube.com/watch?v=Emuw71lozdA
    //create sockets and prepare the essential info before using sockets
    int C_socket=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in C_server;
    socklen_t serverlen=sizeof(C_server);
    bzero(&C_server,serverlen);
    C_server.sin_family=AF_INET;
    C_server.sin_addr.s_addr=inet_addr("127.0.0.1");
    C_server.sin_port=htons(23142);

    struct sockaddr_in AWS_server;
    socklen_t fromlen=sizeof(AWS_server);
    bzero(&AWS_server,fromlen);
    AWS_server.sin_family=AF_INET;
    AWS_server.sin_addr.s_addr=inet_addr("127.0.0.1");
    AWS_server.sin_port=htons(24142);


    //judge if the sockets are created properly
    if (C_socket<0)
    {
    	perror("error\n");
       exit(1);
       cout<<"Server C failed to create the socket"<<endl;
   }
   else
   {
      cout<<"Server C created the socket successfully"<<endl; 
  }

//judge if the sockets are bound properly
  if(bind(C_socket,(struct sockaddr *)&C_server,serverlen)>-1)
  {
   cout<<"The Server C is up and running using UDP on port <"<<C_port<<">"<<endl;
}
else
{
   cout<<"The Server C failed to bind the socket"<<endl;
}

for(;;)
{
    //receive the link ID, file size, power and flag from the AWS
    if(recvfrom(C_socket,&lkid,sizeof(lkid),0, (struct sockaddr* )&AWS_server,&serverlen)>-1)
    {
     recvfrom(C_socket,&size,sizeof(size),0, (struct sockaddr* )&AWS_server,&serverlen);
     recvfrom(C_socket,&powerdb,sizeof(powerdb),0, (struct sockaddr* )&AWS_server,&serverlen);
     recvfrom(C_socket,&fab,sizeof(fab),0, (struct sockaddr* )&AWS_server,&serverlen);
     cout<<"The Server C received link information of link <"<<lkid<<">, file size <"<<size<<">bits, and signal power <"<<powerdb<<">db"<<endl;
 }

 //if there a match for the link, then receive the link info from the AWS
 if(fab)
 {
   recvfrom(C_socket,&bdwt,sizeof(bdwt),0, (struct sockaddr* )&AWS_server,&serverlen);
   recvfrom(C_socket,&leng,sizeof(leng),0, (struct sockaddr* )&AWS_server,&serverlen);
   recvfrom(C_socket,&vlct,sizeof(vlct),0, (struct sockaddr* )&AWS_server,&serverlen);
   recvfrom(C_socket,&nspwdb,sizeof(nspwdb),0, (struct sockaddr* )&AWS_server,&serverlen);

   //calculate the transmission delay, propagation delay and end-to-end delay based on the link info
   powerw=pow(10,powerdb/10-3);
   nspww=pow(10,nspwdb/10-3);
   cpct=(bdwt*pow(10,6))*(log(1+powerw/nspww)/log(2));
   tp=leng/(vlct*pow(10,1));
   tf=(size/cpct)*pow(10,3);
   te=tp+tf;
   cout<<"The Server C finished the calculation for link <"<<lkid<<">"<<endl;

   //send the final results to the AWS
   sendto(C_socket,&tf,sizeof(tf),0,(struct sockaddr* )&AWS_server,fromlen);
   sendto(C_socket,&tp,sizeof(tp),0,(struct sockaddr* )&AWS_server,fromlen);
   sendto(C_socket,&te,sizeof(te),0,(struct sockaddr* )&AWS_server,fromlen);
   cout<<"The Server C finished sending the output to AWS"<<endl;
}
else
{
    cout<<"No link information was found in both the Server A and Server B"<<endl;
}

}

//close the socket
close(C_socket);
return (0);
}

