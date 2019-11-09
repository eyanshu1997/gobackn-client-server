#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h> 
#define BUFLEN 5000
#define PORT 8888 

void die(char *s)
{
	perror(s);
	exit(1);
}

int main(void)
{
	struct sockaddr_in si_me, si_other;
	int s, i, slen = sizeof(si_other) , recv_len;
 
	//socket creation
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}


	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	//server port bind
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
	{
		die("bind");
	}

	char filn[100];
	char size[100];
	//recv file name
	if ((recv_len = recvfrom(s, filn, 100, 0, (struct sockaddr *)&si_other, &slen)) == -1)
	{
			die("recvfrom()");
	}
	if ((recv_len = recvfrom(s, size, 100, 0, (struct sockaddr *)&si_other, &slen)) == -1)
	{
			die("recvfrom()");
	}
	int siz= atoi(size);
	printf("%d\n",siz);
	char filen[110]="destination";
	strcat(filen,filn);
	//file open
	
	FILE *fl = fopen(filen,"w");
	fclose(fl);
	FILE *fp = fopen(filen,"ab");
	if(fp==NULL)
	{
		die("fileopen()");
	} 
	//infinite loop
	while(1)
	{
		char *buf = malloc(siz);
		fflush(stdout);
		//recieve payload
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == -1)
		{
			die("recvfrom()");
		}

		//header seperation 
		char* token = strtok(buf, "\n"); 
		char ack[50];
		strcpy(ack,token);
		token = strtok(NULL, "\n");
		char packettype[100];
		strcpy(packettype,token);
		token = strtok(NULL, "\n");
		char end[100];
		strcpy(end,token);
		//check for end of file
		int res = strcmp(token, "Y");
		token = strtok(NULL, "\n");
		char rcvlen[100];
		strcpy(rcvlen,token);
		char data[BUFLEN]={'\0'};
		token = strtok(NULL, "");
		//data extraction
		while (token != NULL) 
		{ 
			strcat(data,token); 
			token = strtok(NULL, " "); 
		}
		//store to file
		fputs(data,fp);
		printf("%s %s length %s LAST PKT %s \n",packettype,ack,rcvlen,end);
		//send acknoledgement
		if (sendto(s, ack, strlen(ack), 0, (struct sockaddr*) &si_other, slen)== -1)
		{
			die("sendto()");
		}
		else
		{
			printf("SEND ACK %s\n",ack);
		}
		//break condition
		if (res==0)
			break;
	}
	printf("closing connection\n");
	close(s);
	fclose(fp);
	return 0;
}