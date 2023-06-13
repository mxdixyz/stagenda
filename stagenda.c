// See LICENSE file for copyright and license information.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/stat.h>

struct {
	char title[512],
	     date[128],
	     class[512];
} agenda[5];
int agSize=sizeof(agenda)/sizeof(agenda[0]), items=0;
FILE *fptr;
char agFile[512];

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
	struct stat st = {0};
	sprintf(agFile, "%s/.config/stagenda/", getenv("HOME"));
	if(stat(agFile, &st) == -1)
		mkdir(agFile, 0700);
	strcat(agFile, "agenda");
	if(!(fptr=fopen(agFile, "r"))) {
		fptr=fopen(agFile, "w+");
		fprintf(fptr, "\n");
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
populate(void) {
	char buffer[1024], c;
	fptr=fopen(agFile, "r");

	c=fgetc(fptr);
	while(c != EOF) {
		if((int)c-48 > items && (int)c < 53) {
			c=fgetc(fptr);
			if(c == '.') {
				c=fgetc(fptr);
				if(c == ')')
					items++;
			}
		}
		c=fgetc(fptr);
	}
	rewind(fptr);

	if(items > agSize) {
		fprintf(stderr, "stagenda: Too many items\n");
		exit(EXIT_FAILURE);
	}

	for(int i=0; i<items; i++) {
		sprintf(buffer, " %i.) title=%%511[^\n]", i+1);
		fscanf(fptr, buffer, agenda[i].title);

		sprintf(buffer, " %i.) dueDate=%%127[^\n]", i+1);
		fscanf(fptr, buffer, agenda[i].date);

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
printAg(void) {
	if(items <= 0) {
		printf("No entries found\n"
		       "All caught up!\n\n"
		);
		return;
	}
	printf("%s's agenda\n"
	       "===============\n\n",
	       getenv("USER")
	);
	for(int i=0; i<items; i++) {
		printf("%i.) %s\n"
		       "Due: %s\n"
		       "Class: %s\n\n",
		       i+1, agenda[i].title, agenda[i].date, agenda[i].class
		);
	}
}


/** @brief Overwrite/add entries to agenda
 *
 *  User input is taken through stdin,
 *  overwrites/adds entries, and increments
 *  items counter;
 *
 *  @param Void.
 *  @return Void.
 */
void
writeAg(void) {
	int num;
	char input[2048];

	printf("\nEntry number: ");
	scanf("%d", &num);
	num--;
	if(num > items || num < 0)
		fprintf(stderr, "Invalid selection\n\n");
	else {
		if(agenda[num].title[0] == '\0')
			items++;
		printf("Title: ");
		scanf(" %511[^\n]", input);
		strcpy(agenda[num].title, input);
		memset(input, '\0', sizeof(input));

		printf("Due Date: ");
		scanf(" %511[^\n]", input);
		strcpy(agenda[num].date, input);
		memset(input, '\0', sizeof(input));

		printf("Class: ");
		scanf(" %511[^\n]", input);
		strcpy(agenda[num].class, input);
		memset(input, '\0', sizeof(input));

		system("clear");
		printf("Entry %d successfully added\n\n", num+1);
	}
}


/** @brief Removes items from agenda array
 *
 *  Index values are moved up, values at
 *  final index are null-terminated, and
 *  items counter increments down.
 *
 *  @param Void.
 *  @return Void.
 */
void
deleteAg(void) {
	int num;

	if(items == 0) {
		printf("\nThere's nothing to delete\n\n");
		return;
	}
	printf("\nEnter entry number: ");
	scanf("%d", &num);
	num--;
	if(num >= items || num < 0)
		fprintf(stderr, "Invalid selection\n\n");
	else {
		for(int i=num; i<items; i++) {
			strcpy(agenda[i].title, agenda[i+1].title);
			strcpy(agenda[i].date, agenda[i+1].date);
			strcpy(agenda[i].class, agenda[i+1].class);
		}
		memset(agenda[items].title, '\0', sizeof(agenda[items].title));
		memset(agenda[items].date, '\0', sizeof(agenda[items].date));
		memset(agenda[items].class, '\0', sizeof(agenda[items].class));
		items--;

		system("clear");
		printf("Entry %d successfully deleted\n\n", num+1);
	}
}

/** @brief Writes agenda to file
 *
 *  Parses "agenda" array and writes
 *  info to agenda file.
 *
 *  @param Void.
 *  @return Void.
 */
void
writeFile(void) {
	fptr=fopen(agFile, "w+");
	for(int i=0; i<items; i++) {
		fprintf(fptr, "%i.) title=%s\n"
			      "%i.) dueDate=%s\n"
			      "%i.) class=%s\n\n",
			      i+1, agenda[i].title,
			      i+1, agenda[i].date,
			      i+1, agenda[i].class
		);
	}
	fclose(fptr);
}

int
main(int argc, char **args) {
	init();
	populate();
	if(argc > 1) {
		if(strcmp(args[1], "list") == 0) {
			printAg();
			return EXIT_SUCCESS;
		} else if(strcmp(args[1], "-h") == 0 || strcmp(args[1], "--help") == 0) {
			printHelp();
			return EXIT_SUCCESS;
		} else {
			fprintf(stderr, "Invalid argument(s)\n"
					"Use \"-h\" or \"--help\" for help\n"
			);
			return EXIT_FAILURE;
		}
	}
	int e;
	char input[2048];

	system("clear");
	printf("Welcome to stagenda\n"
	       "Ctrl+D (EOF) to exit\n\n"
	);
	while(1) {
		printf("Options: read, write, delete\n> ");
		e=scanf(" %2047[^\n]", input);
		if(e == EOF)
			break;
		if(strcmp(input, "read") == 0) {
			system("clear");
			printAg();
		} else if(strcmp(input, "write") == 0)
			writeAg();
		else if(strcmp(input, "delete") == 0)
			deleteAg();
		else
			fprintf(stderr, "Invalid option\n");
		memset(input, '\0', sizeof(input));
	}
	writeFile();
	printf("\nExiting stagenda...\n\n");
}