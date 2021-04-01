/* f20180991@hyderabad.bits-pilani.ac.in Golla Sai Venkat Gowtham */




#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <string.h>
#include<unistd.h>	//usleep
#include<fcntl.h>	//fcntl


#define CHUNK_SIZE 512


char* base64encode(char input[], int length) {  
     char *result = (char *) malloc(2500 * sizeof(char)); 
     char poss[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 
    
      
    
    int i, j, k = 0; 
    int nb = 0,ind, cnt = 0, pd = 0, value = 0, tmp; 
       
    for (i = 0; i < length; i += 3) 
        { 
            value = 0;
            cnt = 0;
            nb = 0; 
  
            for (j = i; j < length && j <= i + 2; j++) 
            {  
                value = value << 8;  
                value = value | input[j];  
                cnt++;   
            } 
  
            nb = cnt * 8;   
            pd = nb % 3;  
            while (nb != 0)  
            {  
                if (nb >= 6) 
                { 
                    tmp = nb - 6; 
                    ind = (value >> tmp) & 63;  
                    nb -= 6;          
                } 
                else
                { 
                    tmp = 6 - nb;  
                    ind = (value << tmp) & 63;  
                    nb = 0; 
                } 
                result[k++] = poss[ind]; 
            } 
    }  
    for (i = 1; i <= pd; i++)  
    { 
        result[k++] = '='; 
    } 
  
    result[k] = '\0'; 
  
    return result; 
  
}


int main(int argc,char* argv[])
{
	
	char *address;
	printf("hii\n");
	//address = argv[1];
	char* head_url = (char*)malloc(2000*sizeof(char)) ;
	strcpy( head_url ,"http://"); // we need to chnage it to arg
	strcat(head_url,argv[1]);
	while(1)
	{
		printf("1st while started\n");
		int client_socket;
		client_socket = socket(AF_INET,SOCK_STREAM,0);
		
		if(client_socket < 0)
		{
		printf("Error in creating socket");
		}
		int proxyport;
		sscanf(argv[3], "%d", &proxyport);
		
		struct sockaddr_in rem_addr;
		rem_addr.sin_family = AF_INET;
		rem_addr.sin_port = htons(proxyport);
		inet_aton(argv[2], &rem_addr.sin_addr.s_addr);
		
		int c_num = connect(client_socket, (struct sockaddr *) &rem_addr,sizeof(rem_addr));
		
		if(c_num < 0)
		{
		printf("Error in creating connection");
		}
		char* id_pass = (char*)malloc(500*sizeof(char));
		strcpy(id_pass,"");
		strcat(id_pass,argv[4]);
		strcat(id_pass,":");
		strcat(id_pass,argv[5]);
		
		
		//char* website= "info.in2p3.fr";
		//char request[] = "HEAD http://go.com HTTP/1.1\nProxy-Authorization: Basic Y3NmMzAzOmNzZjMwMw==\nConnection: close\r\n\r\n";
		char* request = (char*)malloc(3000*sizeof(char));
		// modify it for general case afterwards
		char* auth = (char*)malloc(600*sizeof(char));
		auth = base64encode(id_pass, strlen(id_pass));
		
		// start building request string
		//strcpy(request,"");
		strcpy(request,"HEAD ");
		strcat(request,head_url);
		strcat(request," HTTP/1.1\n");
		strcat(request,"Proxy-Authorization: Basic ");
		strcat(request,auth);
		strcat(request,"\nConnection: close\n\n");
		//end
		//printf("%s\n",request);
		char response[1024];
		long long nb;//no of bytes recieved
		
		if(send(client_socket,request,strlen(request),0)<0)
		{
			printf("Error while sending during head");
		}
		//FILE* fp = fopen("temp_file.html","w");
		char* tot_res=(char*)malloc(545454545*sizeof(char));
		strcpy(tot_res,"");
		int tot_size =0;
		while(1)
		{
			//printf("infinite loop\n");
			nb = recv(client_socket,response,1024,0);
			if(nb==0)
			{
				printf("broke\n");
				break;
			}
			//strcat(tot_res,response);
			//printf("nb -- %lld",nb);
			for(int i=0;i<nb;i++)
			{
				tot_res[tot_size++] = response[i];
			}
			//printf("hello\n");
			//fwrite((void* )response,1,nb,fp); //
		}
		printf("hii again\n");
		printf("%d---",tot_size);
		printf("%d ----- %d",strlen(tot_res),sizeof(tot_res));
		printf("%s",tot_res);
		int flg = 0;
		int red_flag = 0;
		/*for(int i=0;i<tot_size ;i++)
		{
			if(flg==0 && i+3<tot_size && tot_res[i]=='\r' && tot_res[i+1]=='\n' && tot_res[i+2]=='\r' && tot_res[i+3]=='\n' )
			{
				flg = 1;
				i=i+4;
				printf("File Reading started");
			} 
			if(flg == 1)
			{
				
				fputc(tot_res[i],fp);	
			}
		}*/
		for(int i=0;tot_res[i]!='\n';i++)
		{
			if(tot_res[i]=='3' && tot_res[i+1]=='0')
			{
				red_flag = 1;
				printf("redirect flag enabled\n");
			}
		}
		if(red_flag == 1)
		{
			printf("redirection started");
			char* ptr_loc = strstr(tot_res,"Location:");
			int itr=0,j=10;
			char ans[500];
			while(*(ptr_loc+j)!=13){
				ans[itr] = ptr_loc[j]; // ans = new_url
				j++;
				itr++;
				
				//new_url[url_length++] = loc_loc[url_index];
				//url_index++;
			}
			
			//new_url[url_length]='\0';
			ans[itr] = '\0' ;
			strcpy(head_url, ans);
			//strcat(new_url," HTTP/1.1\n");
			//url_content[url_length]='\0';
			
		}
		else
		{
			break;
		}
		
		
		close(client_socket);
		free(request);
		free(tot_res);
		free(auth);
        	
	}
	
	char* id_pass = (char*)malloc(500*sizeof(char));
	strcpy(id_pass,"");
	strcat(id_pass,argv[4]);
	strcat(id_pass,":");
	strcat(id_pass,argv[5]);
	char* request = (char*)malloc(3000*sizeof(char));
	// modify it for general case afterwards
	char* auth = (char*)malloc(600*sizeof(char));
	
	auth = base64encode(id_pass, strlen(id_pass));
	// start building request string
	strcpy(request,"");
	strcpy(request,"GET ");
	strcat(request,head_url);
	strcat(request," HTTP/1.1\n");
	strcat(request,"Proxy-Authorization: Basic ");
	strcat(request,auth);
	strcat(request,"\nConnection: close\r\n\r\n");
	//end
	
	int client_socket;
		client_socket = socket(AF_INET,SOCK_STREAM,0);
		
		if(client_socket < 0)
		{
		printf("Error in creating socket");
		}
		
		int proxyport;
		sscanf(argv[3], "%d", &proxyport);
		
		struct sockaddr_in rem_addr;
		rem_addr.sin_family = AF_INET;
		rem_addr.sin_port = htons(proxyport);
		inet_aton(argv[2], &rem_addr.sin_addr.s_addr);
		
		int c_num = connect(client_socket, (struct sockaddr *) &rem_addr,sizeof(rem_addr));
		
		if(c_num < 0)
		{
		printf("Error in creating connection");
		}
	char response[1024];
		size_t nb;//no of bytes recieved
		
		if(send(client_socket,request,strlen(request),0)<0)
		{
			printf("Error while sending");
		}
		FILE* fp = fopen(argv[6],"w");
		char* tot_res=(char*)malloc(545454545 * sizeof(char));
		strcpy(tot_res,"");
		int tot_size =0;
		while(1)
		{
			if((nb = recv(client_socket,response,1024,0))==0)
			{
				break;
			}
			//strcat(tot_res,response);
			for(int i=0;i<nb;i++)
			{
				tot_res[tot_size++] = response[i];
			}
			//fwrite((void* )response,1,nb,fp); //
		}
		//printf("%d ----- %d",strlen(tot_res),sizeof(tot_res));
		//printf("%s",tot_res);
		int flg = 0;
		int red_flag = 0;
		//printf("Tot size %d\n", tot_size);
		for(int i=0;i<tot_size ;i++)
		{
			if((flg==0) && (i+3<tot_size) && tot_res[i]=='\r' && tot_res[i+1]=='\n' && tot_res[i+2]=='\r' && tot_res[i+3]=='\n' )
			{
				flg = 1;
				i=i+4;
				printf("File Reading started");
			} 
			if(flg == 1)
			{
				
				fputc(tot_res[i],fp);	
			}
		}
		printf("Done file reading\n");
	close(client_socket);
	free(request);
	free(tot_res);
	free(auth);
        	
	puts("1 Done file reading\n");
	
	
	
	
	
	
	
	
	
	
	
	//char* website = "info.in2p3.fr";
	printf("Head url:- %s", head_url);
	int va = strcmp(head_url,"http://info.in2p3.fr/");
	int va2 = strcmp(head_url,"http://info.in2p3.fr");
	printf("\n ---- %d\n",va);
	if( va == 0  || va2==0 )
	{
		printf("\n downImg ");
		downImg(argv);
	}
	
	return 0;
	/*int total_recv = recv_timeout(client_socket, 4);
	
	printf("\n\nDone. Received a total of %d bytes\n\n" , total_recv);
	return 0;*/
	
	
}

void downImg(char* argv[]) // need to implement argv 
{
	printf("\n Hello ");
	char *address;
	//address = argv[1];
	int client_socket;
	client_socket = socket(AF_INET,SOCK_STREAM,0);
	
	if(client_socket < 0)
	{
	printf("Error in creating socket");
	}
	
	int proxyport;
	sscanf(argv[3], "%d", &proxyport);
	
	struct sockaddr_in rem_addr;
	rem_addr.sin_family = AF_INET;
	rem_addr.sin_port = htons(proxyport);
	inet_aton(argv[2], &rem_addr.sin_addr.s_addr);
	
	int c_num = connect(client_socket, (struct sockaddr *) &rem_addr,sizeof(rem_addr));
	
	if(c_num < 0)
	{
	printf("Error in creating connection");
	}
	char* id_pass = (char*)malloc(500*sizeof(char));
	strcpy(id_pass,"");
	strcat(id_pass,argv[4]);
	strcat(id_pass,":");
	strcat(id_pass,argv[5]);
	char* auth = (char*)malloc(600*sizeof(char));
	
	auth = base64encode(id_pass, strlen(id_pass));
	
	
	//char* website= "go.com";
	char request[] = "GET http://info.in2p3.fr/cc.gif HTTP/1.1\nProxy-Authorization: Basic ";
	
	strcat(request, auth);
	strcat(request,"\nConnection: close\r\n\r\n");
	char response[1024];
	long long nb;//no of bytes recieved
	
	if(send(client_socket,request,strlen(request),0)<0)
	{
		printf("Error while sending");
	}
	FILE* fp = fopen(argv[7],"w");
	char* tot_res=(char*)malloc(545454545 * sizeof(char));
	strcpy(tot_res,"");
	int tot_size =0;
	while(1)
	{
		if((nb = recv(client_socket,response,1024,0))==0)
		{
			break;
		}
		//strcat(tot_res,response);
		for(int i=0;i<nb;i++)
		{
			tot_res[tot_size++] = response[i];
		}
		//fwrite((void* )response,1,nb,fp); //
	}
	printf("%d ----- %d",strlen(tot_res),strlen(tot_res));
	//printf("%s",tot_res);
	int flg = 0;
	for(int i=0;i<tot_size ;i++)
	{
		if(flg==0 && i+3<tot_size && tot_res[i]=='\r' && tot_res[i+1]=='\n' && tot_res[i+2]=='\r' && tot_res[i+3]=='\n' )
		{
			flg = 1;
			i=i+4;
			printf("File Reading started");
		} 
		if(flg == 1)
		{
			
			fputc(tot_res[i],fp);	
		}
	}
	
	
	close(client_socket);
}


/*int recv_timeout(int s , int timeout)
{
	int size_recv , total_size= 0;
	struct timeval begin , now;
	char chunk[CHUNK_SIZE];
	double timediff;
	
	//make socket non blocking
	fcntl(s, F_SETFL, O_NONBLOCK);
	
	//beginning time
	gettimeofday(&begin , NULL);
	
	while(1)
	{
		gettimeofday(&now , NULL);
		
		//time elapsed in seconds
		timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
		
		//if you got some data, then break after timeout
		if( total_size > 0 && timediff > timeout )
		{
			break;
		}
		
		//if you got no data at all, wait a little longer, twice the timeout
		else if( timediff > timeout*2)
		{
			break;
		}
		
		memset(chunk ,0 , CHUNK_SIZE);	//clear the variable
		if((size_recv =  recv(s , chunk , CHUNK_SIZE , 0) ) < 0)
		{
			//if nothing was received then we want to wait a little before trying again, 0.1 seconds
			usleep(100000);
		}
		else
		{
			total_size += size_recv;
			printf("%s" , chunk);
			//reset beginning time
			gettimeofday(&begin , NULL);
		}
	}
	
	return total_size;
}*/

