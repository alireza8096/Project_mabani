#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

void listen_accept();
void register_user();
void login_user();
void create_channel();
void token_make();
void send_message();
void channel_members();
void refresh();
int check_tekrari_username(char tocheck[]);
int check_tekrari_channelname(char tocheck[]);
int token_check();
struct sockaddr_in server, client;
struct online_users{
    char online_username[30];
    char online_token[32];
    char online_channelname[30];
    int read_message;
};
struct online_users online_list[20];
int list_counter;
int server_socket, client_socket;
int onlineuser_number;
char message[100];
char username[30];
char password[30];
char channelname[30];
char usertoken[50];
char sefr[5];
char successfull[50] = "{\"type\":\"Successful\", \"content\": \"\"}";
char wrongpassword[50] = "{\"type\":\"Error\",\"content\":\"Wrong password.\"}";
char username_notavailable[70] = "{\"type\":\"Error\",\"content\":\"this username is not available.\"}";
char username_notvalid[70] = "{\"type\":\"Error\",\"content\":\"Username is not valid.\"}";
char channelname_notavailable[70] = "{\"type\":\"Error\",\"content\":\"Channel name is not available.\"}";
char token_notvalid[70] = "{\"type\":\"Error\",\"content\":\"Authentication failed!\"}";
char member_notvalid[70] = "{\"type\":\"Error\",\"content\":\"User is not a member of the channel\"}";
char channel_notvalid[70] = "{\"type\":\"Error\",\"content\":\"Channel not found.\"}";
char members[100] = "{\"type\":\"List\",\"content\":[";
char new_messages[300] = "{\"type\":\"List\",\"content\":[";
// Driver function
int main()
{
    printf("In the name of Allah, server without cJSON\n\n");
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
	wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
    //else
        //printf("Socket successfully created..\n");
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed...\n");
        exit(0);
    }
    //else
        //printf("Socket successfully bound..\n");




    char buffer[200];
    char command[10];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        memset(command, 0, sizeof(command));
        listen_accept();
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("FROM CLIENT : %s", buffer);
        sscanf(buffer, "%[^ ]s", command);
        if(strncmp(command, "register", 8) == 0)
        {
            memset(username, 0, sizeof(username));
            memset(password, 0, sizeof(password));
            sscanf(buffer + 9, "%s %s", username, password);
            username[strlen(username) - 1] = NULL;
            register_user();
        }
        if(strncmp(command, "login", 5) == 0)
        {
            memset(username, 0, sizeof(username));
            memset(password, 0, sizeof(password));
            sscanf(buffer + 6, "%s %s", username, password);
            username[strlen(username) - 1] = NULL;
            login_user();
        }
        if(strncmp(command, "create", 6) == 0)
        {
            memset(usertoken, 0, sizeof(usertoken));
            memset(channelname, 0, sizeof(channelname));
            sscanf(buffer + 15, "%s %s", channelname, usertoken);
            channelname[strlen(channelname) - 1] = NULL;
            create_channel();
        }
        if(strncmp(command, "logout", 6) == 0)
        {
            memset(usertoken, 0, sizeof(usertoken));
            sscanf(buffer + 7, "%s", usertoken);
            if((onlineuser_number = token_check()) == -1)
            {
                send(client_socket, token_notvalid,sizeof(token_notvalid), 0);
                printf("FROM SERVER : %s\n", token_notvalid);
            }
            else
            {
                memset(online_list[onlineuser_number].online_channelname, 0, sizeof(online_list[onlineuser_number].online_channelname));
                memset(online_list[onlineuser_number].online_token, 0, sizeof(online_list[onlineuser_number].online_token));
                memset(online_list[onlineuser_number].online_username, 0, sizeof(online_list[onlineuser_number].online_username));
                send(client_socket, successfull, sizeof(successfull), 0);
                printf("FROM SERVER : %s\n", successfull);
                //ye hofre to online_list miofteh
            }
        }
        if(strncmp(command, "leave", 5) == 0)
        {
            memset(usertoken, 0, sizeof(usertoken));
            sscanf(buffer + 6, "%s", usertoken);
            if((onlineuser_number = token_check()) == -1)
            {
                send(client_socket, token_notvalid,sizeof(token_notvalid), 0);
                printf("FROM SERVER : %s\n", token_notvalid);
            }
            else
            {
                if(strncmp(sefr, online_list[onlineuser_number].online_channelname, 5) == 0)
                {
                        send(client_socket, member_notvalid,sizeof(member_notvalid), 0);
                        printf("FROM SERVER : %s\n", member_notvalid);
                }
                else
                {
                    memset(channelname, 0, sizeof(channelname));
                    strcpy(channelname, online_list[onlineuser_number].online_channelname);
                    channelname[strlen(channelname)] = '.';
                    channelname[strlen(channelname)] = 'k';
                    FILE *userfile = fopen(channelname, "a");
                    fprintf(userfile, "{\"sender\":\"server\",\"content\":\"%s leaved the channel.\"},\n8", online_list[onlineuser_number].online_username);
                    fclose(userfile);
                    memset(online_list[onlineuser_number].online_channelname, 0, sizeof(online_list[onlineuser_number].online_channelname));
                    send(client_socket, successfull, sizeof(successfull), 0);
                    printf("FROM SERVER : %s\n", successfull);
                }
            }
        }
        if(strncmp(command, "join", 4) == 0)
        {
            memset(channelname, 0, sizeof(channelname));
            memset(usertoken, 0, sizeof(usertoken));
            sscanf(buffer + 13, "%s %s", channelname, usertoken);
            channelname[strlen(channelname) - 1] = NULL;
            if((onlineuser_number = token_check()) == -1)
            {
                send(client_socket, token_notvalid,sizeof(token_notvalid), 0);
                printf("FROM SERVER : %s\n", token_notvalid);
            }
            else if(check_tekrari_channelname(channelname) == 0)
            {
                send(client_socket, channel_notvalid,sizeof(channel_notvalid), 0);
                printf("FROM SERVER : %s\n", channel_notvalid);
            }
            else
            {
                memset(online_list[onlineuser_number].online_channelname, 0, sizeof(online_list[onlineuser_number].online_channelname));
                FILE *userfile = fopen(channelname, "a");
                fprintf(userfile, "{\"sender\":\"server\",\"content\":\"%s joined.\"},\n8", online_list[onlineuser_number].online_username);
                fclose(userfile);
                channelname[strlen(channelname) - 1] = NULL;
                channelname[strlen(channelname) - 1] = NULL;
                strcpy(online_list[onlineuser_number].online_channelname, channelname);
                send(client_socket, successfull,sizeof(successfull), 0);
                printf("FROM SERVER : %s\n", successfull);
            }

        }
        if(strncmp(command, "send", 4) == 0)
        {
            memset(message, 0, sizeof(message));
            memset(usertoken, 0, sizeof(usertoken));
            strncpy(usertoken, buffer + strlen(buffer) - 33, 32);
            strncpy(message, buffer + 5, strlen(buffer) - 40);
            //sscanf(buffer + 5, "%[^,]s", message);
            send_message();
        }
        if(strncmp(command, "channel", 7) == 0)
        {
            memset(usertoken, 0, sizeof(usertoken));
            sscanf(buffer + 16, "%s", usertoken);
            channel_members();
        }
        if(strncmp(command, "refresh", 7) == 0)
        {
            memset(usertoken, 0, sizeof(usertoken));
            sscanf(buffer + 8, "%s", usertoken);
            refresh();
        }

    }

    closesocket(server_socket);
}



int check_tekrari_channelname(char tocheck[30])
{
    tocheck[strlen(tocheck)] = '.';
    tocheck[strlen(tocheck)] = 'k';
    if(fopen(tocheck, "r") == 0)
        return 0;//no tekrari
    else
        return -1;//tekrari
}
int check_tekrari_username(char tocheck[30])
{
    tocheck[strlen(tocheck)] = '.';
    tocheck[strlen(tocheck)] = 'u';
    if(fopen(tocheck, "r") == 0)
        return 0;//no tekrari
    else
        return -1;//tekrari
}

void register_user()
{
    if(check_tekrari_username(username) == -1)
    {
        send(client_socket, username_notavailable,sizeof(username_notavailable), 0);
        printf("FROM SERVER : %s\n", username_notavailable);
    }
    else
    {
        FILE *newuser = fopen(username, "w");
        username[strlen(username) - 1] = NULL;
        username[strlen(username) - 1] = NULL;
        fprintf(newuser, "%s\n%s", password, username);
        fclose(newuser);
        send(client_socket, successfull, sizeof(successfull), 0);
        printf("FROM SERVER : %s\n", successfull);
    }
}
void login_user()
{
    if(check_tekrari_username(username) == 0)
    {
        send(client_socket, username_notvalid,sizeof(username_notvalid), 0);
        printf("FROM SERVER : %s\n", username_notvalid);
    }
    else
    {
        FILE *user = fopen(username, "r");
        char real_password[30] = {};
        fscanf(user, "%s", real_password);
        //printf("%s", password);
        fclose(user);
        if(strcmp(password, real_password) == 0)
        {
            token_make();
            username[strlen(username) - 1] = NULL;
            username[strlen(username) - 1] = NULL;
            strcpy(online_list[list_counter].online_username, username);
            //printf("%s\n%s", online_list[list_counter].online_username, online_list[list_counter].online_token);//bug
            char tokensend[100] ="{\"type\":\"AuthToken\",\"content\":\"";
            strcat(tokensend, online_list[list_counter].online_token);
            tokensend[strlen(tokensend)] = '"';
            tokensend[strlen(tokensend)] = '}';
            tokensend[strlen(tokensend)] = NULL;
            send(client_socket, tokensend, sizeof(tokensend), 0);
            printf("FROM SERVER : %s\n", tokensend);
            list_counter++;
        }
        else
        {
            send(client_socket, wrongpassword, sizeof(wrongpassword), 0);
            printf("FROM SERVER : %s\n", wrongpassword);
        }
    }
}


void create_channel()
{
    if((onlineuser_number = token_check()) == -1)
    {
        send(client_socket, token_notvalid,sizeof(token_notvalid), 0);
        printf("FROM SERVER : %s\n", token_notvalid);
    }
    else
    {
        if(check_tekrari_channelname(channelname) == -1)
        {
            send(client_socket, channelname_notavailable,sizeof(channelname_notavailable), 0);
            printf("FROM SERVER : %s\n", successfull);
        }
        else
        {
            FILE *newuser = fopen(channelname, "w");
            channelname[strlen(channelname) - 1] = NULL;
            channelname[strlen(channelname) - 1] = NULL;
            fprintf(newuser, "{\"sender\":\"server\",\"content\":\"%s created %s.\"},\n8", online_list[onlineuser_number].online_username, channelname);
            fclose(newuser);
            strcpy(online_list[onlineuser_number].online_channelname, channelname);
            send(client_socket, successfull, sizeof(successfull), 0);
            printf("FROM SERVER : %s\n", successfull);
        }
        //ye struct barayeazaye channel mikhad (gozineye azaye channel)
        //dakhele structe online user ye adad baraye tedad payamaye khoondeh shodeh tarif konim
    }

}
int token_check()
{
    int flag = -1;
    for(int i = 0; i <= list_counter; i++)
    {
        //if(strncmp("'\0'", online_list[i].online_token, 32) == 0) continue;
        if(strncmp(usertoken, online_list[i].online_token, 32) == 0)
        {
            flag = 0;
            return i;//token okay hast va shomareye user to online_list return misheh
        }
    }
    if (flag = -1) return -1;//token motabar nist
}


void token_make()
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789-_+!@#$%^&*";
    for (int n = 0; n < 32; n++)
    {
        int key = rand() % (int) (sizeof charset - 1);
        online_list[list_counter].online_token[n] = charset[key];
    }
    online_list[list_counter].online_token[32] = NULL;
}



void send_message()
{
    if((onlineuser_number = token_check()) == -1)
    {
        send(client_socket, token_notvalid, sizeof(token_notvalid), 0);
        printf("FROM SERVER : %s\n", token_notvalid);
    }
    else if (strncmp(sefr, online_list[onlineuser_number].online_channelname, 5) == 0)
    {
        send(client_socket, member_notvalid, sizeof(member_notvalid), 0);
        printf("FROM SERVER : %s\n", member_notvalid);
    }
    else
    {
        memset(channelname, 0 , sizeof(channelname));
        strcpy(channelname, online_list[onlineuser_number].online_channelname);
        channelname[strlen(channelname)] = '.';
        channelname[strlen(channelname)] = 'k';
        FILE *userfile = fopen(channelname, "a");
        fprintf(userfile, "{\"sender\":\"%s\",\"content\":\"%s\"},\n8", online_list[onlineuser_number].online_username, message);
        fclose(userfile);
        send(client_socket, successfull, sizeof(successfull), 0);
        printf("FROM SERVER : %s\n", successfull);
    }
}


void channel_members()
{
    if((onlineuser_number = token_check()) == -1)
    {
        send(client_socket, token_notvalid, sizeof(token_notvalid), 0);
        printf("FROM SERVER : %s\n", token_notvalid);
    }
    else if(strncmp(sefr, online_list[onlineuser_number].online_channelname, 5) == 0)
        {
            send(client_socket, member_notvalid, sizeof(member_notvalid), 0);
            printf("FROM SERVER : %s\n", member_notvalid);
        }
    else
    {
        //make list of members and send it to the client
        for(int i = 0; i <= list_counter; i++)
        {
            if(strcmp(online_list[i].online_channelname, online_list[onlineuser_number].online_channelname) == 0)
            {
                members[strlen(members)] = '"';
                strcat(members, online_list[i].online_username);
                members[strlen(members)] = '"';
                members[strlen(members)] = ',';
            }
        }
        members[strlen(members) - 1] = NULL;
        members[strlen(members)] = ']';
        members[strlen(members)] = '}';
        send(client_socket, members, strlen(members), 0);
        printf("FROM SERVER : %s\n", members);
        memset(members, 0, sizeof(members));
        strcpy(members, "{\"type\":\"List\",\"content\":[");
    }
}
void refresh()
{
    if((onlineuser_number = token_check()) == -1)
    {
        send(client_socket, token_notvalid, sizeof(token_notvalid), 0);
        printf("FROM SERVER : %s\n", token_notvalid);
    }
    else if(strncmp(sefr, online_list[onlineuser_number].online_channelname, 5) == 0)
    {
        send(client_socket, member_notvalid, sizeof(member_notvalid), 0);
        printf("FROM SERVER : %s\n", member_notvalid);
    }
    else
    {
        int check = 0;
        memset(channelname, 0, sizeof(channelname));
        strcpy(channelname, online_list[onlineuser_number].online_channelname);
        channelname[strlen(channelname)] = '.';
        channelname[strlen(channelname)] = 'k';
        FILE *mychannel = fopen(channelname, "r");
        char trash [100] = {};
        char trash2 [2] = {};
        while(check < online_list[onlineuser_number].read_message)
        {
            fscanf(mychannel, "%[^\n]s", trash);
            fscanf(mychannel, "%1s", trash2);
            check++;
        }
        memset(new_messages, 0, sizeof(new_messages));
        strcpy(new_messages, "{\"type\":\"List\",\"content\":[");
        while(!feof(mychannel))
        {
            memset(trash, 0, sizeof(trash));
            fscanf(mychannel, "%[^\n]s", trash);
            fscanf(mychannel, "%1s", trash2);
            strcat(new_messages, trash);
            if (strncmp(trash, sefr, 3) != 0) online_list[onlineuser_number].read_message++;
        }
        if(new_messages[strlen(new_messages) - 1] == ',') new_messages[strlen(new_messages) - 1] = ']';
        else new_messages[strlen(new_messages)] = ']';
        new_messages[strlen(new_messages)] = '}';
        send(client_socket, new_messages, sizeof(new_messages), 0);
        printf("FROM SERVER : %s\n", new_messages);
    }
}
void listen_accept()
{
    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening...\n");
    int len = sizeof(client);
    client_socket = accept(server_socket, (SA *)&client, &len);
    if (client_socket < 0)
    {
        printf("Server accceptance failed...\n");
        exit(0);
    }
    //else
    //    printf("Server acccepted the client..\n");

}
