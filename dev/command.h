#ifndef _0COMMAND0_
#define _0COMMAND0_

#include <stdbool.h>
#include <utmp.h>
#include "apps.h"

char server_username[MAX_STRING_SIZE] = "";

//------------------------------------------------
// help:

bool is_present(const char* const username) 
{
    FILE* file = fopen(path_database, "r");
    char* line = 0;
    size_t line_size = 0;

    while(getline(&line, &line_size, file) != -1)
    {
        line[strlen(line) - 1] = '\0';
        if(0 == strcmp(line, username))
            return true;
    }
    
    return false;
}

char* get_argument(const char* const command)
{
    return strstr(command, " : ") + 3;
}

//------------------------------------------------
// api:

char* get_logged_users()
{
    size_t count = 0, 
    size_line = UT_NAMESIZE + UT_HOSTSIZE + sizeof(int32_t);
    
    setutent();
    struct utmp* user = getutent();
    
    char* message = (char*)malloc(count * size_line);
    
    for(struct utmp* user = getutent(); user; user = getutent())
    {
        //strcpy(message, user->ut_user); // username
        strcat(message, " ");
        //strcat(message, user->ut_host); // hostname 
        strcat(message, " ");

        char string[MAX_STRING_SIZE];
        //strcat(message, itoa(user->ut_tv.tv_sec, string, 10)); // time entry in seconds
    }

    return message;
}
     
char* get_proc_info(const char* const command)
{
    /*
    char pid[MAX_STRING_SIZE];
    strcpy(pid, get_argument(command));

    char path[MAX_STRING_SIZE] = "/proc/";
    strcat(path, pid);
    strcat(path, "/status");

    FILE* file = fopen(path, "r");
    
    int n = 0;
    char* message = 0;

    // valid pid
    if(file && strstr(command, "get-proc-info : "))
    {
        message = (char*)malloc(1024);
        char* buffer = 0;
        size_t buffer_size = 0; 
        
        while(getline(&buffer, &buffer_size, file) != -1)
        {
            if(strstr(buffer, "Name: ") ||
            strstr(buffer, "State: ") ||
            strstr(buffer, "PPid: ") ||
            strstr(buffer, "Uid: ") ||
            strstr(buffer, "VmSize: ")            
            )
                strcat(message, buffer);
                strcat(message, "\n");
        }
    
        return message;
    }

    // invalid pid
    const char errorM[] = "invalid pid.\n";
    
    n = strlen(errorM);
    message = (char*)malloc(2 + n + 1);
    sprintf(message, "%d", n);
    strcat(message, errorM);
    message[2 + n] = 0;
    */
    char* message = (char*)malloc(1);
    return message;
}

//------------------------------------------------

char* const login_user(const char* const command)
{    
    char* username = get_argument(command);
    if(is_present(username) && 0 == server_username[0])
    {
        strcpy(server_username, username);

        char buffer[MAX_OUTPUT_SIZE] = "welcome, ";
        strcat(buffer, username);
        strcat(buffer, "!");
        return convert_line(buffer);
    }
    
    const char buffer[] = "can not conenct.";
    return convert_line(buffer);
}

char* logout_user()
{
    if(server_username[0]) // if logged-in
    {
        char buffer[MAX_OUTPUT_SIZE] = "we will miss you, ";
        strcat(buffer, server_username);
        strcat(buffer, "...");

        size_t n = strlen(server_username);
        for(size_t i = 0; i < n; i++)
            server_username[i] = 0;

        return convert_line(buffer);
    }

    const char buffer[] = "you're not logged-in.";
    return convert_line(buffer);
}

char* quit()
{
    const char buffer[] = "closing appilication.";
    return convert_line(buffer);
}

//------------------------------------------------
// command parser:

char* parse_command(const char* const command)
{
    if('q' == command[0])
        if(0 == strcmp(command, "quit"))
            return quit();

    // logs
    if('l' == command[0])
    {
        if(0 == strcmp(command, "logout"))
            return logout_user();
        if(NULL != strstr(command, "login : ") &&
        strlen(command) > 9)
            return login_user(command);
    }

    // queries
    if('g' == command[0])
    {
        if(0 == strcmp(command, "get-logged-users"))
            return get_logged_users();
        if(NULL != strstr(command, "get-proc-info : ") &&
            strlen(command) > 17)
            return get_proc_info(command);
    }

    // invalid
    const char buffer[] = "invalid command.";
    return convert_line(buffer);
}

#endif
