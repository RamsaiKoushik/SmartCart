// Program Number: 34 a
// Name:P.Ramsai Koushik
// Register Number: IMT2021072
// Date: May 11,2023
// Description: a program to create a concurrent server (using fork()).
#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"netinet/in.h"
#include"arpa/inet.h"
#include"unistd.h"
#include"server.h"
int main()
{
	struct sockaddr_in server,client;
	int sd,nsd,clientLen;
	sd=socket(AF_INET,SOCK_STREAM,0);
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=inet_addr("127.0.0.1");
	server.sin_port=htons(6031);
	int x = bind(sd,(struct sockaddr *)&server,sizeof(server));
	if(x==-1)
		perror("bind");
	int y = listen(sd,5);
	if(y==-1)
		perror("listen");	
	write(1,"Waiting for the client.....\n",29);
	while(1)
    {
		clientLen=sizeof(client);
		nsd=accept(sd,(struct sockaddr *)&client,&clientLen);
		if(nsd==-1)
			perror("accept");
        int choice=1,id,menuResult;
		write(1,"Connected to the client.....\n",30);
		if(!fork()){
			close(sd);
            char reading[18];
			read(nsd,reading,18);
            write(nsd,"Hello from server",18);
            while(1)
			{
				switch(choice)
				{
					case 1:
						id = UserMenu(nsd);
						choice=2;
						break;
					case 2:
						menuResult = Menu(nsd,id);
						if(menuResult)
							choice=1;
						break;
				}
				if(id==-1)
					break;
			}
            exit(0);
		}
		else{
			close(nsd);
		}
	}
	close(sd);
	return 0;
}