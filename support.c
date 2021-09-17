
#include <stdio.h>             // for I/O
#include <stdlib.h>            // for I/O
#include "support.h"           // for dirname()/basename()
#include <string.h>

//               0       1      2   3    4     5        6       7     8         9        10     11
char* cmd[] = { "mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm", "quit", "reload", "save", "?", 0 };

int findCmd(char* command)
{
    int i = 0;
    while (cmd[i]) {
        if (strcmp(command, cmd[i]) == 0)
            return i;
        i++;
    }
    return -1;
}
/*
NODE* search_child(NODE* parent, char* name)
{
    NODE* p;
    printf("search for %s in parent DIR\n", name);
    p = parent->child;
    if (p == 0)
        return 0;
    while (p) {
        if (strcmp(p->name, name) == 0)
            return p;
        p = p->sibling;
    }
    return 0;
}
*/
/*
int insert_child(NODE* parent, NODE* q)
{
    NODE* p;
    printf("insert NODE %s into end of parent child list\n", q->name);
    p = parent->child;
    if (p == 0)
        parent->child = q;
    else {
        while (p->sibling)
            p = p->sibling;
        p->sibling = q;
    }
    q->parent = parent;
    q->child = 0;
    q->sibling = 0;
}
*/

/***************************************************************
 This mkdir(char *name) makes a DIR in the current directory
 You MUST improve it to mkdir(char *pathname) for ANY pathname
****************************************************************/

int mkdir(char* name)
{
    if (strcmp(path, "") == 0)
    {
        printf("err: empty pathname\n");
        return 0;
    }
    else if (strcmp(path, "/") == 0) return 0;

    NODE* child, * parent;
    if (path[0] == '/') parent = root;
    else parent = cwd;

    split(path);

    char* mover = strtok(dirname, "/");

    while (mover != NULL)
    {
        child = parent->childN;
        while (1)
        {
            if (child == NULL)
            {
                return 0;
            }
            if (strcmp(child->name, mover) == 0 && 'D' == child->type)
            {
                parent = child;
                break;
            }

            child = child->siblingN;
        }
        mover = strtok(NULL, "/");
    }

    NODE* sibling = child = parent->childN;
    NODE* prevSibling;
    while (sibling != NULL)
    {
        if (strcmp(sibling->name, basename) == 0)
        {
            printf("err: pathname used\n");
            return 0;
        }
        prevSibling = sibling;
        sibling = sibling->siblingN;
    }

    if (child == NULL)
    {
        child = malloc(sizeof(NODE));
        child->parentN = parent;
        strcpy(child->name, basename);
        child->type = 'D';
        child->siblingN = NULL;
        child->childN = NULL;

        parent->childN = child;
    }
    else
    {
        sibling = malloc(sizeof(NODE));
        sibling->parentN = parent;
        strcpy(sibling->name, basename);
        sibling->type = 'D';
        sibling->childN = NULL;
        sibling->siblingN = NULL;

        prevSibling->siblingN = sibling;
    }
}

// This ls() list CWD. You MUST improve it to ls(char *pathname)
int ls()
{
    NODE *child, *parent;
  if((strcmp(pathname, "") == 0) || pathname[0] != '/') parent = cwd;
  else parent = root;
  
  char *mover = strtok(pathname, "/");
  child = parent->childN;
  while(mover != NULL)
  {
    while(1)
    {
      if(child == NULL)
      {
        printf("err: pathname not exist\n");
        return 0;
      }
      if(strcmp(child->name, mover) == 0)
      {
        parent = child;
        break;
      }
      child = child->siblingN;
    }
    child = parent->childN;
    mover = strtok(NULL, "/");
  }
  
  child = parent->childN;
  while(child != NULL)
  {
    printf("   %c\t%s\n", child->type, child->name);
    child = child->siblingN;
  }
}

int quit()
{
    save();
    printf("Exit\n");
    exit(0);
    // improve quit() to SAVE the current tree as a Linux file
    // for reload the file to reconstruct the original tree
}
/*
int initialize()
{
    root = (NODE*)malloc(sizeof(NODE));
    strcpy(root->name, "/");
    root->parent = root;
    root->sibling = 0;
    root->child = 0;
    root->type = 'D';
    cwd = root;
    printf("Root initialized OK\n");
}
*/

int pwd()
{
    printf("   ");
    rpwd(cwd);
    printf("\n");
}

void rmdir(char* path)
{
    if (strcmp(path, "") == 0)
    {
        printf("err: pathname empty\n");
        return 0;
    }

    split(path);

    NODE* child, * parent;
    if (dirname[0] == '/') parent = root;
    else parent = cwd;

    char* mover = strtok(dirname, "/");
    while (mover != NULL)
    {
        child = parent->childN;
        while (1)
        {
            if (child == NULL)
            {
                printf("err: pathname not exist\n");
                return 0;
            }
            if (strcmp(child->name, mover) == 0 && 'D' == child->type)
            {
                parent = child;
                break;
            }

            child = child->siblingN;
        }
        mover = strtok(NULL, "/");
    }

    NODE* sibling = child = parent->childN;
    if (child != NULL)
    {
        if (strcmp(child->name, basename) == 0)
        {
            if ('F' == child->type)
            {
                printf("err: wrong type\n");
                return 0;
            }
            if (child->childN == NULL)
            {
                parent->childN = child->siblingN;
            }
            else printf("err: dir not empty\n");
            return 0;
        }
        else
        {
            while (sibling != NULL)
            {
                if (strcmp(sibling->name, basename) == 0 && 'D' == sibling->type)
                {
                    if (sibling->childN == NULL)
                    {
                        child->siblingN = sibling->siblingN;
                    }
                    else
                    {
                        printf("err: dir not empty\n");
                    }
                    return 0;
                }
                child = sibling;
                sibling = sibling->siblingN;
            }
        }
    }
    printf("err: dir not exist\n");
}


int save()
{
    file = fopen("myfile", "w+");

    if (file == NULL)
    {
        printf("err: file not created\n");
        return 0;
    }

    preOrderWrite(root->childN);

    fclose(file);
}

void preOrderWrite(NODE* node)
{
    if (node == NULL)
    {
        return 0;
    }

    fprintf(file, "%c\t", node->type);

    NODE* temp = node;
    Stack* top = malloc(sizeof(Stack)), * pusher;
    strcpy(top->name, temp->name);
    top->prev = NULL;

    while (temp->parentN != NULL)
    {
        temp = temp->parentN;

        if (temp != NULL)
        {
            pusher = malloc(sizeof(Stack));
            strcpy(pusher->name, temp->name);

            pusher->prev = top;
            top = pusher;
        }
    }

    top = top->prev;

    while (top != NULL)
    {
        fprintf(file, "/%s", top->name);
        top = top->prev;
    }
    fprintf(file, "\n");

    if (node->childN != NULL)
    {
        preOrderWrite(node->childN);
    }
    if (node->siblingN != NULL)
    {
        preOrderWrite(node->siblingN);
    }
}

int rpwd(NODE* node)
{
    if (strcmp(node->name, "/"))
    {
        rpwd(node->parentN);
        printf("%s/", node->name);
    }
    else printf("%s", node->name);
}

int creat(char path[])
{
    if (strcmp(path, "") == 0)
    {
        printf("err: empty filename\n");
        return 0;
    }

    NODE* child, * parent;
    if (path[0] == '/') parent = root;
    else parent = cwd;

    split(path);

    char* mover = strtok(dirname, "/");

    while (mover != NULL)
    {
        child = parent->childN;
        while (1)
        {
            if (child == NULL)
            {
                printf("err: pathname not exist\n");
                return 0;
            }
            if (strcmp(child->name, mover) == 0 && 'D' == child->type)
            {
                parent = child;
                break;
            }

            child = child->siblingN;
        }
        mover = strtok(NULL, "/");
    }

    child = parent->childN;
    if (child == NULL)
    {
        // Create file node
        child = malloc(sizeof(NODE));
        strcpy(child->name, basename);
        child->type = 'F';
        child->parentN = parent;
        child->childN = NULL;
        child->siblingN = NULL;
        parent->childN = child;
        return 0;
    }
    NODE* sibling;
    while (child != NULL)
    {
        if (strcmp(child->name, basename) == 0)
        {
            printf("err: filename exist\n");
            return 0;
        }
        sibling = child;
        child = child->siblingN;
    }

    child = malloc(sizeof(NODE));
    strcpy(child->name, basename);
    child->type = 'F';
    child->parentN = parent;
    child->siblingN = NULL;
    child->childN = NULL;
    sibling->siblingN = child;
}


int save()
{
    file = fopen("myfile", "w+");

    if (file == NULL)
    {
        printf("err: file not created\n");
        return 0;
    }

    preOrderWrite(root->childN);

    fclose(file);
}

void preOrderWrite(NODE* node)
{
    if (node == NULL)
    {
        return 0;
    }

    fprintf(file, "%c\t", node->type);

    NODE* temp = node;
    Stack* top = malloc(sizeof(Stack)), * pusher;
    strcpy(top->name, temp->name);
    top->prev = NULL;

    while (temp->parentN != NULL)
    {
        temp = temp->parentN;

        if (temp != NULL)
        {
            pusher = malloc(sizeof(Stack));
            strcpy(pusher->name, temp->name);

            pusher->prev = top;
            top = pusher;
        }
    }

    top = top->prev;

    while (top != NULL)
    {
        fprintf(file, "/%s", top->name);
        top = top->prev;
    }
    fprintf(file, "\n");

    if (node->childN != NULL)
    {
        preOrderWrite(node->childN);
    }
    if (node->siblingN != NULL)
    {
        preOrderWrite(node->siblingN);
    }
}

int pwd()
{
    printf("   ");
    rpwd(cwd);
    printf("\n");
}

int rpwd(NODE* node)
{
    if (strcmp(node->name, "/"))
    {
        rpwd(node->parentN);
        printf("%s/", node->name);
    }
    else printf("%s", node->name);
}

int creat(char path[])
{
    if (strcmp(path, "") == 0)
    {
        printf("err: empty filename\n");
        return 0;
    }

    NODE* child, * parent;
    if (path[0] == '/') parent = root;
    else parent = cwd;

    split(path);

    char* mover = strtok(dirname, "/");

    while (mover != NULL)
    {
        child = parent->childN;
        while (1)
        {
            if (child == NULL)
            {
                printf("err: pathname not exist\n");
                return 0;
            }
            if (strcmp(child->name, mover) == 0 && 'D' == child->type)
            {
                parent = child;
                break;
            }

            child = child->siblingN;
        }
        mover = strtok(NULL, "/");
    }

    child = parent->childN;
    if (child == NULL)
    {
        // Create file node
        child = malloc(sizeof(NODE));
        strcpy(child->name, basename);
        child->type = 'F';
        child->parentN = parent;
        child->childN = NULL;
        child->siblingN = NULL;
        parent->childN = child;
        return 0;
    }
    NODE* sibling;
    while (child != NULL)
    {
        if (strcmp(child->name, basename) == 0)
        {
            printf("err: filename exist\n");
            return 0;
        }
        sibling = child;
        child = child->siblingN;
    }

    child = malloc(sizeof(NODE));
    strcpy(child->name, basename);
    child->type = 'F';
    child->parentN = parent;
    child->siblingN = NULL;
    child->childN = NULL;
    sibling->siblingN = child;
}

int split(char* path)
{
    char temp[64];
    strcpy(temp, path);
    char* mover = strtok(temp, "/");
    while (mover != NULL)
    {
        strcpy(basename, mover);
        mover = strtok(NULL, "/");
    }

    if (strlen(path) - strlen(basename) == 0)
    {
        strncpy(dirname, path, strlen(path) - strlen(basename));
    }
    else strncpy(dirname, path, strlen(path) - strlen(basename) - 1);
}

int rm(char* path)
{
    if (strcmp(path, "") == 0)
    {
        printf("err: pathname empty\n");
        return 0;
    }

    NODE* child, * parent;
    if (path[0] == '/') parent = root;
    else parent = cwd;

    split(path);

    char* mover = strtok(dirname, "/");
    while (mover != NULL)
    {
        child = parent->childN;
        while (1)
        {
            if (child == NULL)
            {
                printf("err: pathname not exist\n");
                return 0;
            }
            if (strcmp(child->name, mover) == 0 && 'D' == child->type)
            {
                parent = child;
                break;
            }

            child = child->siblingN;
        }
        mover = strtok(NULL, "/");
    }

    child = parent->childN;
    NODE* sibling = child->siblingN;
    if (child != NULL)
    {
        if (strcmp(child->name, basename) == 0)
        {
            if ('D' == child->type)
            {
                printf("err: wrong file type\n");
                return 0;
            }
            parent->childN = child->siblingN;
            return 0;
        }
        else
        {
            while (sibling != NULL)
            {
                if (strcmp(sibling->name, basename) == 0 && 'F' == sibling->type)
                {
                    child->siblingN = sibling->siblingN;
                    return 0;
                }
                child = sibling;
                sibling = sibling->siblingN;
            }
        }
    }
    printf("err: file does not exist\n");
}

int reload()
{
    file = fopen("myfile", "r");

    if (file == NULL)
    {
        printf("err: cannot open file\n");
        return 0;
    }

    char type, path[100], line[100];
    while (1)
    {

        fgets(line, 100, file);

        if (feof(file)) break;

        line[strlen(line)] = 0; line[strlen(line) - 1] = 0;
        if (line != NULL || strcmp(line, "") != 0)
        {

            char* splitter = strtok(line, "\t");
            type = *splitter;
            splitter = strtok(NULL, "\t");
            strcpy(path, splitter);
            switch (type)
            {
            case 'F':
                creat(path);
                break;
            case 'D':
                mkdir(path);
                break;
            }
        }
        clearmem();
    }
    fclose(file);
}

int clearmem()
{
    memset(dirname, 0, sizeof(dirname));
    memset(pathname, 0, sizeof(pathname));
    memset(basename, 0, sizeof(basename));
}
