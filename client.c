#include<stdio.h> 
#include<string.h>
#include<stdlib.h> 
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/time.h>
#include <unistd.h> 
#define BUFLEN 5000
 
void die(char *s)
{
	perror(s);
	exit(1);
}

int main(int argc,char *argv[])
{
	//check for arguments
	if(argc<6)
	{
		printf("wrong arguments correct format: server IP address, Server port number, file_name,pkt_size, and WS");
		return 0;
	}
	struct sockaddr_in si_other;
	int s, i, slen=sizeof(si_other);
	int size;
	int ws;
	int port;
	char ip[100];
	char filen[100];
	char buf[100];
	//variable initialization
	strcpy(ip,argv[1]);
	port=atoi(argv[2]);
	strcpy(filen,argv[3]);
	size=atoi(argv[4]);
	ws=atoi(argv[5]);
	//socket creation
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);
	si_other.sin_addr.s_addr = inet_addr(ip);
	//file open
	FILE *fp = fopen(filen,"rb");
	if(fp==NULL)
	{
		die("fileopen()");
	} 
	
	//file name send
	if (sendto(s, filen, strlen(filen) , 0 , (struct sockaddr *)&si_other, slen)==-1)
	{
		die("sendto()");
	}
	if (sendto(s,argv[4], strlen(filen) , 0 , (struct sockaddr *)&si_other, slen)==-1)
	{
		die("sendto()");
	}
	//variable initialization
	int lastack=0;
	int done=1;
	int count=0;
	int fcount=9999;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	//socket timeout set
	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) 
	{
		printf("server not started");
		perror("Error");
	}
	//while loop till end of file
	while( lastack!=fcount+1)
	{
		int j=0;
		//while loop for window size
		while(j<ws && done==1)
		{
			unsigned char *buff	 =malloc(size);
			char off[100];
			//acknoledgement
			int ackno=lastack+j;
			sprintf(off, "%d", ackno);
			
			printf("SEND PACKET ");
			puts(off);
			//packet header creation
			char *newstr1 = malloc(strlen(off) + 2);
			strcpy(newstr1, off);
			strcat(newstr1, "\n");
			char newstr3[]="recieve packet \n";
			char *newstr4 = malloc(strlen(newstr1)+strlen(newstr3) + 6);
			strcat(newstr4,newstr1);
			strcat(newstr4,newstr3);
			
			//file data read
			fseek(fp, (lastack+j)*size, SEEK_SET);
			int nread = fread(buff,1,size,fp);
			
			if (nread < size)
			{
				if (feof(fp))
				{
					//header and packet creation of last packet
					strcat(newstr4,"Y");
					strcat(newstr4,"\n");
					char siz[2];
					sprintf(siz, "%d", nread);
					char *newstr2 = malloc(strlen(siz) + 2);
					strcpy(newstr2, siz);
					strcat(newstr2, "\n");
					char *newstr = malloc(strlen(newstr4)+strlen(newstr2) + 2);
					strcat(newstr,newstr4);
					strcat(newstr,newstr2);
					//exit condition
					done=0;
					fcount=count;
					char *newst = malloc(strlen(newstr)+strlen(buff) + 2);
					strcat(newst,newstr);
					strcat(newst,buff);
					//sending
					if (sendto(s, newst, strlen(newst) , 0 , (struct sockaddr *)&si_other, slen)==-1)
					{
						die("sendto()");
					}
					else
					{
						count++;
						sleep(1);
					}
				}
				if (ferror(fp))
				printf("Error reading\n");
			}
			else
			{
				//header and packet creation of packets
					strcat(newstr4,"N");
					strcat(newstr4,"\n");
					char siz[2];
					sprintf(siz, "%d", size);
					char *newstr2 = malloc(strlen(siz) + 2);
					strcpy(newstr2, siz);
					strcat(newstr2, "\n");
					char *newstr = malloc(strlen(newstr4)+strlen(newstr2) + 2);
					strcat(newstr,newstr4);
					strcat(newstr,newstr2);
					char *newst = malloc(strlen(newstr)+strlen(buff) + 2);
					strcat(newst,newstr);
					strcat(newst,buff);
					//sending
					if (sendto(s, newst, strlen(newst) , 0 , (struct sockaddr *)&si_other, slen)==-1)
					{
						die("sendto()");
					}
					else
					{
						count++;
						sleep(1);
					}
			}

			j++;
		}

		int k=0;
		for(k=0;k<ws;k++)
		{
			//recieve packets
			if(lastack==fcount+1)
				break;
			memset(buf,'\0', BUFLEN);
			if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)== -1)
			{
				die("recvfrom() there is no server running at the specified ip and port");
			}
			printf("RECIEVE ACK ");
			puts(buf);
			int offset= atoi(buf);
			if(offset==lastack)
				lastack++;
		}
		if(lastack==fcount+1)
				break;
	}
	//tear off 
	printf("closing connection\n");
	exit(0);
	return 0;
}
