#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"netinet/in.h"
#include"arpa/inet.h"
#include"unistd.h"
#include"server.h"
#include"fcntl.h"
int main()
{
	struct sockaddr_in server,client;
	int sd,nsd,clientLen;
	sd=socket(AF_INET,SOCK_STREAM,0);

	int optval = 1;
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	// fcntl(sd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
	if(sd==-1)
		perror("socket"); 
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=inet_addr("127.0.0.1");
	server.sin_port=htons(6011);

	int x = bind(sd,(struct sockaddr *)&server,sizeof(server));
	if(x==-1)
		perror("bind");
	int y = listen(sd,5);	
	if(y==-1)
		perror("listen");
		clientLen=sizeof(client);
	sleep(1);
	printf("server is accepting from socket - %d\n",sd);
	printf("server process id- %d\n",getpid());
	while(1)
	{
		nsd=accept(sd,(struct sockaddr *)&client,&clientLen);
		if(nsd==-1)
		{
			perror("accept");
			printf("pid - %d accept error",getpid());
			return 0;
		}
		printf("sd- %d\n",sd);
		if(!fork())
		{
			printf("child process id- %d. nsd=%d\n",getpid(),nsd);
			printf("closing socket in child process\n");
			close(sd);
			char reading[18];
			read(nsd,reading,18);
			write(nsd,"Hello from server",18);
			int choice=1,id,menuResult;
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
			close(nsd);
			exit(0);
		}
		else
			close(nsd);
	}
	return 0;
}