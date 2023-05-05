// See LICENSE file for copyright and license details.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

void init(void),
	 populateAgenda(void),
	 printHelp(void),
	 printAgenda(void),
	 writeFile(void);

int count(void);

static char agendaFile[128];
static FILE *fptr;
static struct {
	char title[512],
		 dueDate[128],
		 class[512];
} agenda[5];
static int agendaSize=sizeof(agenda)/sizeof(agenda[0]);

int
main(int argc, char *argv[]) {
	init();
	populateAgenda();
	if(argc>1) {
		if(strcmp(argv[1], "list") == 0) {
			printAgenda();
			return EXIT_SUCCESS;
		} else if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
			printHelp();
			return EXIT_SUCCESS;
		} else {
			fprintf(stderr, "Invalid arguments\n"
							"Use \"-h\" or \"--help\" for help\n"
			);
			return EXIT_FAILURE;
		}
	}
	
	signal(SIGINT, SIG_IGN); // Disable SIGINT (Ctrl+C)
	char input[1024], eof;
	int num, items;
	
	printf("Welcome to stagenda\n"
		   "Ctrl+D (EOF) to exit\n\n"
	);
	while(1) {
		printf("Options: read, write, delete\n"
			   "> "
		);
		eof=scanf(" %1023[^\n]", input);
		if(eof==EOF) break;
		printf("\n");
		if(strcmp(input, "read") == 0) printAgenda();
		else if(strcmp(input, "write") == 0) {
			printf("Enter entry number: ");
			scanf("%2i", &num);
			num--;
			if(num >= agendaSize || num > count() || num < 0) printf("Invalid selection\n");
			else {
				memset(input, '\0', sizeof(input));

				printf("Title: ");
				scanf(" %511[^\n]", input);
				strcpy(agenda[num].title, input);
				memset(input, '\0', sizeof(input));

				printf("Due Date: ");
				scanf(" %127[^\n]", input);
				strcpy(agenda[num].dueDate, input);
				memset(input, '\0', sizeof(input));

				printf("Class: ");
				scanf(" %511[^\n]", input);
				strcpy(agenda[num].class, input);
				memset(input, '\0', sizeof(input));
			}
		} else if(strcmp(input, "delete") == 0 ) {
			printf("Enter entry number: ");
			scanf("%2i", &num);
			num--;
			if(num >= agendaSize || num >= count() || num < 0) printf("Invalid selection\n");
			else {
				items=count();
				for(int i=num; i<items+1; i++) {
					strcpy(agenda[i].title, agenda[i+1].title);
					strcpy(agenda[i].dueDate, agenda[i+1].dueDate);
					strcpy(agenda[i].class, agenda[i+1].class);
				}
				items--;
				memset(agenda[items].title, '\0', sizeof(agenda[items].title));
				memset(agenda[items].dueDate, '\0', sizeof(agenda[items].dueDate));
				memset(agenda[items].class, '\0', sizeof(agenda[items].class));
			}
		}
		  else fprintf(stderr, "Invalid option\n");
		memset(input, '\0', sizeof(input));
		printf("\n");
	}
	writeFile();
	printf("\n\n");
}

/** @brief Initializes agenda file
 *
 *  Sets static "agendaFile" variable.
 *
 *  Generates agenda file and/or directory if needed.
 *
 *  @param Void.
 *  @return Void.
 */
void
init(void) {
	struct stat st={0};
	sprintf(agendaFile, "%s/.config/stagenda/", getenv("HOME"));
	if(stat(agendaFile, &st)==-1) mkdir(agendaFile, 0700);
	strcat(agendaFile, "agenda");
	if(fopen(agendaFile, "r")==NULL) {
		fptr=fopen(agendaFile, "w+");
		fprintf(fptr, "Empty Agenda\n");
		fclose(fptr);
	}
}

/** @brief Parses agenda file to agenda struct
 *
 *  Reads and parses agenda file, populating
 *  "agenda" struct. 
 *
 *  @param Void.
 *  @return Void.
 */
void
populateAgenda(void) {
	char buffer[1024], c, s[1];
	int x=0, items=0;
	fptr=fopen(agendaFile, "r");

	c=fgetc(fptr);
	while(c!=EOF) {
		s[0]=c;
		if(atoi(s) != 0 && atoi(s) != x) {
			x=atoi(s);
			c=fgetc(fptr);
			if(c == '.') {
				c=fgetc(fptr);
				if(c == ')') items++;
			}
		}
		if(items > agendaSize) {
			printf("Too many items");
			exit(EXIT_FAILURE);
		}
		c=fgetc(fptr);
	}
	rewind(fptr);
	for(int i=0; i<items; i++) {
		sprintf(buffer, " %i.) title=%%511[^\n]", i+1);
		fscanf(fptr, buffer, agenda[i].title);

		sprintf(buffer, " %i.) date=%%127[^\n]", i+1);
		fscanf(fptr, buffer, agenda[i].dueDate);

		sprintf(buffer, " %i.) class=%%511[^\n]", i+1);
		fscanf(fptr, buffer, agenda[i].class);
	}
	fclose(fptr);
}


/** @brief Prints help info
 *
 *  Prints help info to stdout.
 *
 *  @param Void.
 *  @return Void.
 */
void
printHelp(void) {
	printf("Usage: stagenda [OPTION]\n\n"
		   "list\t\tPrints agenda; non-interactive\n"
		   "-h, --help\tPrints this menu\n\n"
		   "All output is printed to stdout or stderr\n"
	);
}


/** @brief Prints agenda contents
 *
 *  Iterates through "agenda" struct and
 *  prints information to stdout.
 *
 *  @param Void.
 *  @return Void.
 */
void
printAgenda(void) {
	if(!(count()>0)) {
		printf("No entries found\n"
			   "All caught up!\n"
		);
		return;
	}
	printf("%s's Agenda\n"
		   "===============\n\n",
		   getenv("USER")
	);
	for(int i=0; i<count(); i++)
		printf("%i.) %s\n"
			   "Due: %s\n"
			   "Class: %s\n\n",
			   i+1, agenda[i].title, agenda[i].dueDate, agenda[i].class
	);
}


/** @brief Writes agenda to file
 *
 *  Parses "agenda" struct and writes
 *  info to agenda file.
 *
 *  @param Void.
 *  @return Void.
 */
void
writeFile(void) {
	fptr=fopen(agendaFile, "w+");
	for(int i=0; i<count(); i++) {
		fprintf(fptr, "%i.) title=%s\n"
					  "%i.) date=%s\n"
					  "%i.) class=%s\n\n",
					  i+1, agenda[i].title,
					  i+1, agenda[i].dueDate,
					  i+1, agenda[i].class
		);
	}
	fclose(fptr);
}


/** @brief Counts agenda items
 *
 *  Counts sets of items in "agenda" struct.
 *
 *  @param Void.
 *  @return items - Number of agenda items
 */
int
count(void) {
	int items=0;
	for(int i=0; i<agendaSize; i++) {
		if(agenda[i].title[0]!='\0') items++;
	}
	return items;
}