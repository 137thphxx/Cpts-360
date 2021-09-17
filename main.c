/*
Name: Peilin.Wu
ID:11593587
Answer for demo question:
1. How do you divide a pathname into stoken strings?
Strcpy the input file path and using the split part.

2. How to convert a pathname to its NODE pointer in the binary tree?
Change the current directory.

3. How does  ls /a/b/c   work?
List the directory contents.

4. How to mkdir /a/b/c?
Create new directory if it doesn't exist.

5. How to pwd?
Print the pathname of the current directory.

6. How to implement a general tree as a binay tree?
Go through it and print all of binay tree.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"

int (*fptr[])(char*) = { (int(*)())mkdir, rmdir, ls, cd, pwd, creat, rm, quit, reload, save };

int main()
{
    int r, com;
    root = malloc(sizeof(NODE));
    strcpy(root->name, "/");

    root->type = 'D';
    root->siblingN = NULL;
    root->parentN = NULL;
    root->childN = NULL;

    cwd = root;

    printf("enter \"menu\" to get the command.\n");
    while (1)
    {
        printf("=>");
        fgets(line, 128, stdin);
        scanf(line, "%s %s", command, pathname);
        printf("****%s\n", line);
        com = findCmd(command);
        if (com != -1)
            r = fptr[com](pathname);
        else printf("err: command invalid\n");

        clearmem();
    }

    return 0;
}

/*
int main()
{
    int index;

    initialize();

    printf("NOTE: commands = [mkdir|ls|quit]\n");

    while (1) {
        printf("Enter command line : ");
        fgets(line, 128, stdin);
        line[strlen(line) - 1] = 0;

        command[0] = pathname[0] = 0;
        sscanf(line, "%s %s", command, pathname);
        printf("command=%s pathname=%s\n", command, pathname);

        if (command[0] == 0)
            continue;

        index = findCmd(command);

        switch (index) {
        case 0: mkdir(pathname); break;
        case 1: ls();            break;
        case 2: quit();          break;
        }
    }
}

*/
