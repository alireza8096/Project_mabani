#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <winsock2.h>
#include <windows.h>
#include "cJSON.h"
#define MAX 120
#define PORT 12345
#define SA struct sockaddr

void make_socket();
void login_menu();
void karbari_menu();
void chat_menu();

char global_channel_name[MAX];
char global_username[MAX];
int client_socket;
int state = 1;//1 for login menu; 2 for working menu; 3 for chat menu
char user_token[MAX];

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }
    while (1)
    {
        while (1)
        {
            if (state == 1)
            {
                //make_socket();
                login_menu();
                //closesocket(client_socket);
                break;
            }
            if (state == 2)
            {
                //make_socket();
                karbari_menu();
                //closesocket(client_socket);
                break;
            }
            if (state == 3)
            {
                //make_socket();
                chat_menu();
                //closesocket(client_socket);
                break;
            }
        }
    }
}

void make_socket()
{
    struct sockaddr_in servaddr, cli;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
    printf("Socket creation failed...\n");
    exit(0);
    }
    //else
    //printf("Socket successfully created..\n");
    // Assign IP and port
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    // Connect the client socket to server socket
    if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
    printf("Connection to the server failed...\n");
    exit(0);
    }
}


void login_menu()
{
    system("color E");
    while(state == 1)
    {
        printf("In his name\n\n1: Register\n2: Login\n3: Help (recommended to read)\n4: About us...!\n");
        char username[MAX];
        char password[MAX];
		char reg[MAX] = "register ";
		char recieve[MAX];
		char log[MAX] = "login ";
		memset(username, 0, sizeof(username));
		memset(password, 0, sizeof(password));
		memset(recieve, 0, sizeof(recieve));
		int n = 0;
		char bn;
		int log_or_reg = 0;
		scanf("%d%c", &log_or_reg, &bn);
		if(log_or_reg == 3)
        {
            system("cls");
            printf("Points\n\n1- You have 10 seconds time to communicate with server\n2- If you send an invalid character it may crash\n3- You can send about 900 characters in every message sending to channel\n");
        }
        if(log_or_reg == 4)
        {
            system("cls");
            printf("This is an academic project...\nAlireza Hosseinkhani\nStd Num: 98101403\nContact me: Alireza.8096@chmail.ir\n\n");
        }
		if((log_or_reg == 1) || (log_or_reg == 2))
        {
        printf("Please send username then press Enter\n");
        while ((username[n++] = getchar()) != '\n');
        memset(global_username, 0, sizeof(global_username));
        username[n-1] = ',';
        username[n] = ' ';
        memmove(global_username, username, n);
        int m = 0;
        printf("Please send your password then press Enter\n");
        while ((password[m++] = getchar()) != '\n');
        strcat(username, password);
        system("cls");
        make_socket();
        if(log_or_reg==1)
        {
            strcat(reg, username);
            send(client_socket, reg, sizeof(reg), 0);
            recv(client_socket, recieve, sizeof(recieve), 0);
        }

        if(log_or_reg==2)
        {
            strcat(log, username);
            send(client_socket, log, sizeof(log), 0);
            recv(client_socket, recieve, sizeof(recieve), 0);
        }
        system("cls");
        cJSON* result_json = cJSON_Parse(recieve);
        char *tocheck = cJSON_GetObjectItem(result_json, "type")->valuestring;
        if(strcmp(tocheck, "Successful") == 0)
        {
            printf("SUCCESSFUL : \",%s successfully registered. Please login for use\"\n\n", global_username);
        }
        if(strcmp(tocheck, "AuthToken") == 0)
        {
            printf("SUCCESSFUL : \",%s successfully logged in.\"\n\n", global_username);
            state = 2;//go to next menu
            memset(user_token, 0, sizeof(user_token));
            memcpy(user_token, &recieve[31], 32);
            user_token[32] = '\n';
        }
        if(strcmp(tocheck, "Error") == 0)
        {
            char *error_type = cJSON_GetObjectItem(result_json, "content")->valuestring;
            printf("ERROR : \"%s\"\n\n", error_type);
        }
        closesocket(client_socket);
        }
    }
}



void karbari_menu()
{
    system("color E");
    while(state == 2)
    {
        printf("1: Make new channel\n2: Join a channel\n3: logout\n");
        char channel_name[MAX];
        char join_channel[MAX] = "join channel ";
        char create_channel[MAX] = "create channel ";
        char logout[MAX] = "logout ";
        char recieve[MAX];
        memset(channel_name, 0, sizeof(channel_name));
        memset(recieve, 0, sizeof(recieve));
        int n = 0;
        char bn;
        int create_join_logout;
        scanf("%d%c", &create_join_logout, &bn);
        make_socket();
        if (create_join_logout == 3)
        {
            strcat(logout, user_token);
            send(client_socket, logout, sizeof(logout), 0);
            recv(client_socket, recieve, sizeof(recieve), 0);
            closesocket(client_socket);
            cJSON* result_json = cJSON_Parse(recieve);
            char *tocheck = cJSON_GetObjectItem(result_json, "type")->valuestring;
            if(strcmp(tocheck, "Successful") == 0)
            {
                system("cls");
                state = 1;
                printf("SUCCESSFUL ! \",%s Successfully logged out.\"\n\n", global_username);
            }
            else
            {
                char *error_type = cJSON_GetObjectItem(result_json, "content");
                printf("%s", error_type);
            }
        }
        else
        {
            printf("Please send requested channel name then press Enter\n");
            while ((channel_name[n++] = getchar()) != '\n');
            channel_name[n] = ' ';
            channel_name[n-1] = ',';
            memset(global_channel_name, 0, sizeof(global_channel_name));
            memmove(global_channel_name, channel_name, n);
            strcat(channel_name, user_token);
            if (create_join_logout == 1)
            {
                strcat(create_channel, channel_name);
                send(client_socket, create_channel, sizeof(create_channel), 0);
                recv(client_socket, recieve, sizeof(recieve), 0);
            }
            if (create_join_logout == 2)
            {
                strcat(join_channel, channel_name);
                send(client_socket, join_channel, sizeof(join_channel), 0);
                recv(client_socket, recieve, sizeof(recieve), 0);
            }
            cJSON* result_json = cJSON_Parse(recieve);
            char *tocheck = cJSON_GetObjectItem(result_json, "type")->valuestring;
            if(strcmp(tocheck, "Successful") == 0)
            {
                if(create_join_logout == 1)
                {
                system("cls");
                state = 3;
                printf("SUCCESSFULL ! \"Making ,%s channel was successful.\"\n\n", global_channel_name);
                }
                if(create_join_logout == 2)
                {
                system("cls");
                state = 3;
                printf("SUCCESSFULL ! \"You joined the ,%s channel successfully.\"\n\n", global_channel_name);
                }
            }
            else
            {
                system("cls");
                char *error_type = cJSON_GetObjectItem(result_json, "content")->valuestring;
                printf("ERROR ! %s\n\n", error_type);
            }
        closesocket(client_socket);
        }//end of else
    }//end of while
}
void chat_menu()
{
    system("color E");
    while(state == 3)
    {
        int n = 0;
        char message_array[MAX];
        char recieve[1000];
        memset(recieve, 0, sizeof(recieve));
        char bn;
        int send_refresh_member_leave;
        char leave_channel[MAX] = "leave ";
        char send_message[1000] = "send ";
        char channel_members[MAX] = "channel members ";
        char refresh[MAX] = "refresh ";
        printf("1: Send message\n2: Refresh channel\n3: Channel members\n4: Leave channel\n");
        scanf("%d%c", &send_refresh_member_leave, &bn);
        make_socket();
        if(send_refresh_member_leave == 4)
        {
            strcat(leave_channel, user_token);
            send(client_socket, leave_channel, sizeof(leave_channel), 0);
            recv(client_socket, recieve, sizeof(recieve), 0);
            cJSON* result_json = cJSON_Parse(recieve);
            char *tocheck = cJSON_GetObjectItem(result_json, "type")->valuestring;
            if(strcmp(tocheck, "Successful") == 0)
            {
                system("cls");
                printf("SUCCESSFULL : \"You successfully left the ,%s channel.\"\n\n", global_channel_name);
                state = 2;
            }
            else
            {
                char *error_type = cJSON_GetObjectItem(result_json, "content")->valuestring;
                printf("ERROR : \"%s\"", error_type);
            }
            closesocket(client_socket);

        }
        else if(send_refresh_member_leave == 3)
        {
            strcat(channel_members, user_token);
            send(client_socket, channel_members, sizeof(channel_members), 0);
            memset(recieve, 0, sizeof(recieve));
            recv(client_socket, recieve, sizeof(recieve), 0);
            int i = 26;
            system("cls");
            printf("Channel members are:\n");
            cJSON *monitor_json = cJSON_Parse(recieve);
            cJSON *contents = cJSON_GetObjectItemCaseSensitive(monitor_json, "content");
            cJSON *content = NULL;
            cJSON_ArrayForEach(content, contents)
            {
                printf("%s\n", content->valuestring);
            }
            printf("\n");
        }
        else if(send_refresh_member_leave == 2)
        {
            strcat(refresh, user_token);
            send(client_socket, refresh, sizeof(refresh), 0);
            memset(recieve, 0, sizeof(recieve));
            recv(client_socket, recieve, sizeof(recieve), 0);
            int j = 26;
            system("cls");
            printf("The messages sent from last refresh are :\n\n");
            cJSON* items=NULL;
            cJSON* rpage_json = cJSON_Parse(recieve);
            cJSON* mycontent = cJSON_GetObjectItemCaseSensitive(rpage_json, "content");
            cJSON_ArrayForEach(items,mycontent)
            {
            printf("%s : %s\n\n",cJSON_GetObjectItemCaseSensitive(items,"sender")->valuestring,cJSON_GetObjectItemCaseSensitive(items,"content")->valuestring);
            }
            printf("\n");

        }
        else if(send_refresh_member_leave == 1)
        {
        system("cls");
        printf("Write your message then press Enter to send in the ,%s channel\n", global_channel_name);
        n = 0;
        memset(message_array, 0, sizeof(message_array));
        while ((message_array[n++] = getchar()) != '\n');
        message_array[n-1] = ',';
        message_array[n] = ' ';
        strcat(send_message, message_array);
        strcat(send_message, user_token);
        send(client_socket, send_message, sizeof(send_message), 0);
        memset(recieve, 0, sizeof(recieve));
        recv(client_socket, recieve, sizeof(recieve), 0);
        system("cls");
        cJSON* result_json = cJSON_Parse(recieve);
        char *tocheck = cJSON_GetObjectItem(result_json, "type")->valuestring;
        if(strcmp(tocheck, "Successful") == 0)
            printf("SUCCESSFUL\n\n");
        }
        closesocket(client_socket);
    }
}
