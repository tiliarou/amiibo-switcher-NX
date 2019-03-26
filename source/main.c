#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <switch.h>

#define MAXLEN 256
#define MAXCNT 1000
char options[MAXCNT][MAXLEN];
const char path[] = "/amiibos/";
int selection = 0, count = 0;

void printMenu()
{
        printf(CONSOLE_ESC(2J));
        printf("File listing for '%s':\n", path);
        for (int i = 0; i < count; i++)
        {
                if (selection == i)
                        printf(" >");
                else
                        printf("  ");
                printf("%s\n", options[i]);
        }
        printf("Found %d amiibos\nUse the left stick to change the selected file\nPress + to exit\n",
	        count);
        consoleUpdate(NULL);
}

bool cp(const char * from, const char * to)
{
	char buf[BUFSIZ];
	size_t size;
	FILE * source = fopen(from, "rb");
	FILE * dest = fopen(to, "wb");

	if (source == NULL || dest == NULL)
		return false;

	while ((size = fread(buf, 1, BUFSIZ, source)) != 0)
		fwrite(buf, 1, size, dest);

	fclose(source);
	fflush(dest);
	fclose(dest);
	return true;
}

int main(int argc, char ** argv)
{
	consoleInit(NULL);
	DIR * dir = opendir(path);
	struct dirent * ent;
	int ok = 0;
        
	if (dir == NULL)
		printf("Failed to open dir.\nPress + to exit");
	else {
		printf("File listing for '%s':\n", path);
		while ((ent = readdir(dir)))
		{
			if (strlen(ent->d_name) >= 5)
				if (!strcmp(".bin", ent->d_name + strlen(ent->d_name) - 4))
				{
                                        strcpy(options[count], ent->d_name);
					printf("  %s\n", options[count]);
                                        count++;
				}
		}
		if (!count)
			printf("No bin files detected\nPress + to exit");
		else {
			ok = 1;
			printf("Found %d amiibos\nUse the left stick to change the selected file\nPress + to exit\n",
				count);
		}
	}

        consoleUpdate(NULL);
        closedir(dir);

	while (appletMainLoop()) {
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_PLUS)
			break;

		if (ok) {
			if ((kDown & KEY_LSTICK_UP || kDown & KEY_DUP) && selection > 0) {
				selection--;
                                printMenu();
                        }
			if ((kDown & KEY_LSTICK_DOWN || kDown & KEY_DDOWN) && selection < count - 1) {
				selection++;
                                printMenu();
                        }
			if (kDown & KEY_A) {

				printf("\n%s is selected.\nDo you want to continue?\nA - yes\nB - back\n"
					"+ - exit\n", options[selection]);
				consoleUpdate(NULL);

				while (1) {
					hidScanInput();
					kDown = hidKeysDown(CONTROLLER_P1_AUTO);
					if (kDown & KEY_PLUS)
						goto SHUTDOWN;

					if (kDown & KEY_A) {
						char fullPath[strlen(options[selection]) + strlen(path) + 1];

						strcpy(fullPath, path);
						strcpy(fullPath + strlen(path), options[selection]);

						if (!cp(fullPath, "/amiibo.bin"))
							printf("\nCopying the file failed!\n");
						else
							printf(CONSOLE_GREEN"OK!\n");

						ok = 0;
						goto WAITING;
                                        }
					if (kDown & KEY_B)
						goto MAINLOOP;
				}
                        }
                }
                else {
		        WAITING:while(1) {
				hidScanInput();
				kDown = hidKeysDown(CONTROLLER_P1_AUTO);
				printf("\rPress + to exit");

				if (kDown & KEY_PLUS)
					goto SHUTDOWN;

				consoleUpdate(NULL);
			}
                }
		MAINLOOP:;
		
	}
	consoleUpdate(NULL);
	SHUTDOWN:consoleExit(NULL);
	return 0;
}
