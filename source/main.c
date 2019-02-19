#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <switch.h>

const char path[] = "/amiibos/";

bool cp(const char *from, const char *to)
{
        char buf[BUFSIZ];
        size_t size;
        FILE* source = fopen(from, "rb");
        FILE* dest = fopen(to, "wb");

        if (source == NULL || dest == NULL)
                return false;

        while ((size = fread(buf, 1, BUFSIZ, source)) != 0)
                fwrite(buf, 1, size, dest);

        fclose(source);
        fflush(dest);
        fclose(dest);
        return true;
}

int main(int argc, char **argv)
{
        consoleInit(NULL);

        DIR* dir;
        struct dirent* ent;
        int amiiboCounter = 0, ok = 0, selection = 1;

        dir = opendir(path);

        if(dir == NULL)
                printf("Failed to open dir.\nPress + to exit");

        else
        {
                printf("Bin-listing for '%s':\n", path);

                while ((ent = readdir(dir)))
                {
                        if(strlen(ent->d_name) >= 5)
                                if(!strcmp(".bin", ent->d_name + strlen(ent->d_name) - 4))
                                {
                                        amiiboCounter++;
                                        printf("%d. %s\n", amiiboCounter, ent->d_name); 
                                }
                }
                if(!amiiboCounter)
                        printf("No bin files detected\nPress + to exit");

                else
                {
                        ok = 1;
                        printf("Found %d amiibos\nUse the left stick to change the selection\nPress + to exit\n", amiiboCounter);
                }

        }

        while(appletMainLoop())
        {
                hidScanInput();
                u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

                if (kDown & KEY_PLUS)
                        break;

                if(ok)
                {
                        int selectionCounter = 0;

                        printf(CONSOLE_ESC(2K)"%d\r", selection);

                        if(kDown & KEY_LSTICK_UP && selection < amiiboCounter)
                                selection++;

                        if(kDown & KEY_LSTICK_DOWN && selection > 1)
                                selection--;

                        if(kDown & KEY_A)
                        {

                                while ((ent = readdir(dir)))
                                {
                                        if(strlen(ent->d_name) >= 5)
                                                if(!strcmp(".bin", ent->d_name + strlen(ent->d_name) - 4))
                                                {
                                                        selectionCounter++;

                                                        if(selectionCounter == selection)
                                                        {
                                                                printf("\n%s is selected.\nDo you want to continue?\nA - yes\nB - back\n+ - exit\n", ent->d_name);
                                                                consoleUpdate(NULL);

                                                                while(1)
                                                                {
                                                                        hidScanInput();
                                                                        kDown = hidKeysDown(CONTROLLER_P1_AUTO);
                                                                        if(kDown & KEY_PLUS)
                                                                                goto SHUTDOWN;
                                                                
                                                                        if(kDown & KEY_A)
                                                                        {
                                                                                char fullPath[strlen(ent->d_name) + strlen(path) + 1];
                                                                                strcpy(fullPath, path);
                                                                                strcpy(fullPath + strlen(path), ent->d_name);
                                                                                
                                                                                if(!cp(fullPath, "/amiibo.bin"))
                                                                                        printf("\nCopying the file failed!\n");

                                                                                else
                                                                                        printf("OK!\n");

                                                                                ok = 0;
                                                                                goto WAITING;
                                                                        }
                                                                        if(kDown & KEY_B)
                                                                        goto MAINLOOP;
                                                                }
                                                        }
                                                }
                                }
                                WAITING:while(1)
                                {
                                        hidScanInput();
                                        kDown = hidKeysDown(CONTROLLER_P1_AUTO);
                                        printf("\rPress + to exit.");

                                        if (kDown & KEY_PLUS)
                                                goto SHUTDOWN;
                                        
                                        consoleUpdate(NULL);
                                }
                                MAINLOOP:;
                        }
                }
                consoleUpdate(NULL);
        }
        SHUTDOWN:closedir(dir);
        consoleExit(NULL);
        return 0;
}