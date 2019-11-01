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
bool fb;
int B_port=22142;

//call this subfunction to search the link info in the database for the given link ID
//the source code for this subfunction is referred from https://www.cnblogs.com/chenzhefan/p/5314516.html
int search_data(double clientid)
{
	vector<csvdata> incsv;
	csvdata intp;
	FILE *fp=fopen("database_b.csv","r");
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
	int B_socket=socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in B_server;
	socklen_t serverlen=sizeof(B_server);
	bzero(&B_server,serverlen);
	B_server.sin_family=AF_INET;
	B_server.sin_addr.s_addr=inet_addr("127.0.0.1");
	B_server.sin_port=htons(22142);

	struct sockaddr_in AWS_server;
	socklen_t fromlen=sizeof(AWS_server);
	bzero(&AWS_server,fromlen);
	AWS_server.sin_family=AF_INET;
	AWS_server.sin_addr.s_addr=inet_addr("127.0.0.1");
	AWS_server.sin_port=htons(24142);

    //judge if the sockets are created properly
	if (B_socket<0)
	{
		perror("error\n");
		exit(1);
		cout<<"Server B failed to create the socket"<<endl;
	}
	else
	{
		cout<<"Server B created the socket successfully"<<endl;	
	}
	
    //judge if the sockets are bound properly
	if(bind(B_socket,(struct sockaddr *)&B_server,serverlen)>-1)
	{
		cout<<"The Server B is up and running using UDP on port <"<<B_port<<">"<<endl;
	}
	else
	{
		cout<<"The Server B failed to bind the socket"<<endl;
	}

	for(;;)
	{
		////receive the link ID, file size and power from the AWS
		if(recvfrom(B_socket,&clientid, sizeof(clientid),0, (struct sockaddr *)&AWS_server,&fromlen)>-1)
		{		
			cout<<"The server B received input <"<<clientid<<">"<<endl;

			//if there is a match for the link in the database, then send the flag and link info back to the AWS
			if (!search_data(clientid))
			{

				fb=1;
				cout<<"The Server B has found <"<<fb<<"> match"<<endl;
				sendto(B_socket,&fb,sizeof(fb),0,(struct sockaddr *)&AWS_server,fromlen);
				sendto(B_socket,&a[1],sizeof(a[1]),0,(struct sockaddr *)&AWS_server,fromlen);
				sendto(B_socket,&a[2],sizeof(a[2]),0,(struct sockaddr *)&AWS_server,fromlen);
				sendto(B_socket,&a[3],sizeof(a[3]),0,(struct sockaddr *)&AWS_server,fromlen);
				sendto(B_socket,&a[4],sizeof(a[4]),0,(struct sockaddr *)&AWS_server,fromlen);
				cout<<"The Server B finished sending the output to AWS"<<endl;
			}

			//if there is no matches for the link in the database, just send the flag back to the AWS
			else
			{
				fb=0;
				sendto(B_socket,&fb,sizeof(fb),0,(struct sockaddr *)&AWS_server,fromlen);
				cout<<"The Server B has found <"<<fb<<"> match"<<endl;
			}

		}
		
	}

	//close the socket
	close(B_socket);

	return(0);
}




