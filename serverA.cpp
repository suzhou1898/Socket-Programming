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
class csvdata{
public:
	double lkid;
	double bdwt;
	double leng;
	double vlct;
	double nspw;
};
double a[5];
double clientid;
bool fa;
int A_port=21142;

//call this subfunction to search the link info in the database for the given link ID
//the source code for this subfunction is referred from https://www.cnblogs.com/chenzhefan/p/5314516.html
int search_data(double clientid)
{
	vector<csvdata> incsv;
	csvdata intp;
	FILE *fp=fopen("database_a.csv","r");
	if (fp==NULL)
	{
		perror("error\n");
		exit(1);
	}
	while (1){
		fscanf(fp,"%lf,%lf,%lf,%lf,%lf",&intp.lkid,&intp.bdwt,&intp.leng,&intp.vlct,&intp.nspw);
		incsv.push_back(intp);
		if(feof(fp))
			break;
	}
	fclose(fp);
	for(std::size_t i=0;i<incsv.size();i++)
	{
		if (incsv[i].lkid==clientid)
		{
			a[0]=incsv[i].lkid;
			a[1]=incsv[i].bdwt;
			a[2]=incsv[i].leng;
			a[3]=incsv[i].vlct;
			a[4]=incsv[i].nspw;

			return (0);
		}
	}
	return (1);
}

int main()
{
	//the part regarding how to establish the TCP and UDP connection is referred from the webpage https://www.youtube.com/watch?v=Emuw71lozdA
	//create sockets and prepare the essential info before using sockets
	int A_socket=socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in A_server;
	socklen_t serverlen=sizeof(A_server);
	bzero(&A_server,serverlen);
	A_server.sin_family=AF_INET;
	A_server.sin_addr.s_addr=inet_addr("127.0.0.1");
	A_server.sin_port=htons(21142);

	struct sockaddr_in AWS_server;
	socklen_t fromlen=sizeof(AWS_server);
	bzero(&AWS_server,fromlen);
	AWS_server.sin_family=AF_INET;
	AWS_server.sin_addr.s_addr=inet_addr("127.0.0.1");
	AWS_server.sin_port=htons(24142);

    //judge if the sockets are created properly
	if (A_socket<0)
	{
		perror("error\n");
		exit(1);
		cout<<"Server A failed to create the socket"<<endl;
	}
	else 
	{
		cout<<"Server A created the socket successfully"<<endl;	
	}

    //judge if the sockets are bound properly
	if(bind(A_socket,(struct sockaddr *)&A_server,serverlen)>-1)
	{
		cout<<"The Server A is up and running using UDP on port <"<<A_port<<">"<<endl;
	}
	else
	{
		cout<<"The Server A failed to bind the socket"<<endl;
	}


	for(;;)
	{
		//receive the link ID, file size and power from the AWS
		if(recvfrom(A_socket,&clientid, sizeof(clientid),0, (struct sockaddr *)&AWS_server,&fromlen)>-1)
		{	
			cout<<"The Server A received input <"<<clientid<<">"<<endl;

			//if there is a match for the link in the database, then send the flag and link info back to the AWS
			if (!search_data(clientid))
			{
				fa=1;
				cout<<"The Server A has found <"<<fa<<"> match"<<endl;
				sendto(A_socket,&fa,sizeof(fa),0,(struct sockaddr *)&AWS_server,fromlen);
				sendto(A_socket,&a[1],sizeof(a[1]),0,(struct sockaddr *)&AWS_server,fromlen);
				sendto(A_socket,&a[2],sizeof(a[2]),0,(struct sockaddr *)&AWS_server,fromlen);
				sendto(A_socket,&a[3],sizeof(a[3]),0,(struct sockaddr *)&AWS_server,fromlen);
				sendto(A_socket,&a[4],sizeof(a[4]),0,(struct sockaddr *)&AWS_server,fromlen);
				cout<<"The Server A finished sending the output to AWS"<<endl;
			}

            //if there is no matches for the link in the database, just send the flag back to the AWS
			else
			{
				fa=0;
				sendto(A_socket,&fa,sizeof(fa),0,(struct sockaddr *)&AWS_server,fromlen);
				cout<<"The Server A has found <"<<fa<<"> match"<<endl;
			}
		}		
	}
	
	//close the socket
	close(A_socket);
	
	return(0);
}




















    