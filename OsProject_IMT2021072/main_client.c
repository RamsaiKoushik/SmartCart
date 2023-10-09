#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"netinet/in.h"
#include"arpa/inet.h"
#include"unistd.h"
#include"client.h"

int main(void){
	struct sockaddr_in server;
	int socket_descriptor;
	socket_descriptor = socket(AF_INET,SOCK_STREAM,0);
	server.sin_family=AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(6011);
	connect(socket_descriptor,(struct sockaddr*)&server,sizeof(server));
    char reading[18];
    write(socket_descriptor,"Hello from client",18);
	read(socket_descriptor,reading,18);
    printf("%s\n",reading);
    int ch=0,menuRes=0;
    while(1)
    {
        switch(ch)
        {
            case 0:
                ch =  UserMenu(socket_descriptor);
                break;
            case 1:
                menuRes = AdminMenu(socket_descriptor);
                if(menuRes)
                    ch=0;
                break;
            case 2:
                menuRes = CustomerMenu(socket_descriptor);
                if(menuRes)
                    ch=0;
        }
        if(ch==-1)
            break;
    }
	close(socket_descriptor);
	return 0 ;
}