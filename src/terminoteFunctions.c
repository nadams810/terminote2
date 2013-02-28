#include "helperFunctions.h"
#include "linkedList.h"
#include "terminoteFunctions.h"
#include "defines.h"

/* Prints menu message */
void menuMessage(void) {
	printf("\nWelcome to Terminote interactive:\n"
			"(w) to insert new note\n"
			"(a) and (d) to navigate through notes\n"
			"(e) to print all notes\n"
			"(g) to delete all notes\n"
			"(f) to search for notes\n"
			"(m) to print this message\n"
			"(q) to quit\n");
}

/* Prints usage */
void printUsage()
{
	printf("Fill this in\n");
}

/* Prints current note */
void printCurrent(node *currP) {

	if (currP == NULL || currP->note_num == 0)
		fprintf(stdout, "Nothing to print.\n");
	else {
		printf("NoteNum: %d"
				"\nPath: %s"
				"\nTime: %s"
				"\nMessage: %s\n\n", currP->note_num, currP->path, currP->time,
				currP->message);
	}
}

/* Prints note number n */
void printN(node *currP, node *head, int n)
{
	currP = searchByNoteNum(currP, head, n);
	if (currP && currP->note_num == n)
		printCurrent(currP);
	else
		fprintf(stderr, "No note number: %d\n", n);
}

/* Asks user for search term then prints all notes that contain it. */
void printAllWithSubStringInteractive(node *currP, node *head) {
	int found = 0;
	currP = head;
	FLUSH_STDIN(Junk);
	printf("Enter search term:\n> ");
	getInput(inputBuffer, MAX_MESSAGE_SIZE);

	/* Don't check root node */
	if (currP->note_num == 0)
		currP = currP->next;

	while (currP) {
		if (hasSubstring(currP, inputBuffer)) {
			found++;
			printCurrent(currP);
		}
		currP = currP->next;
	}

	if (found == 0)
		printf("Nothing found.\n");

	FLUSH_STDIN(Junk);
}

/* Asks user for search term then prints all notes that contain it. */
void printAllWithSubString(node *currP, node *head, char *subString) {

	int found = 0;
	/* Don't check root node */
	if (currP->note_num == 0 && currP->next != NULL)
		currP = currP->next;

	if ( currP->note_num == 0 )
	{
		fprintf(stderr, "No notes to search\n");
		return;
	}

	while (currP) {
		if (hasSubstring(currP, subString)) {
			found++;
			printCurrent(currP);
		}
		currP = currP->next;
	}

	if (found == 0)
		fprintf(stderr, "Nothing found.\n");

}


/* Prompts user for y or n question. */
/* Returns true for y and false for anything else */
bool promtUserChoice(char *prompt) {
	char choice;
	printf("%s\n> ", prompt);
	scanf("%c", &choice);

	if (tolower(choice) == 'y')
		return true;
	else
		return false;
}

/* Prompts user for input, appends input to list.
 * Returns pointer to the new node. */
node *appendNote(char inputBuffer[], int buffSize, node *currP, node *head) {
	printf("Enter Note\n> ");
	FLUSH_STDIN(Junk);
	if ( getInput(inputBuffer, buffSize) >= MAX_MESSAGE_SIZE )
		fprintf(stderr, "Input too large, truncated\n");
	FLUSH_STDIN(Junk);
	currP = head;
	return append(currP, inputBuffer);
}

/* Asks user if they want to delete all notes. If so, deletes them */
void deleteAllNotesInteractive(node *currP, node *head) {
	FLUSH_STDIN(Junk);
	if (promtUserChoice("Delete all notes [y/n]?")) {
		currP = head;
		deleteAll(currP);
	} else {
		printf("Nothing deleted.\n");
	}
	/* Reset to head otherwise we are sitting in a NULL node */
	currP = head;

	FLUSH_STDIN(Junk);
}


/* Deletes current currP. Returns pointer to head. This version asks the user if they want to delete and deletes if the answer is y */
node *deleteCurrentInteractive(node *currP, node *head) {
	FLUSH_STDIN(Junk);
	if (currP->note_num == 0 && currP->next == NULL ) {
		printf("Nothing to delete.\n");
		return head;
	}

	if (promtUserChoice("Delete current note?")) {
		deleteNode(currP, head, currP->note_num);
		printf("Deleted.\n");
		currP = head;
		orderList(currP);
		FLUSH_STDIN(Junk);
		return head;
	} else {
		FLUSH_STDIN(Junk);
		return currP;
	}
}

/* Deletes current currP.
 * WARNING: This version is for non-interactive, it doesn't return anything, prompt the user or clean up after itself. */
void deleteCurrent(node *currP, node *head) {
	if (currP->note_num == 0 && currP->next == NULL ) {
		return;
	}
	deleteNode(currP, head, currP->note_num);
}

/* Deletes note number n */
void deleteN(node *currP, node *head, int n)
{
	currP = searchByNoteNum(currP, head, n);

	if ( currP ) {
		deleteCurrent(currP, head);
		currP = head;
		orderList(currP);
		saveList(head);
	} else {
		fprintf(stderr, "No note number: %d\n", n);
	}
}

/* Deletes all notes. Non interactive. */
void deleteA(node *currP, node *head)
{
	currP = head;
	if ( listLength(currP) )
	{
		deleteAll(currP);
		saveList(head);
	} else {
		fprintf(stderr, "Nothing to delete\n");
	}
}

/* Pops a note off the list. In other words, prints last note then deletes it. If there are no notes to print, send an error to stderr. */
void popNote(node *currP, node *head)
{
	currP = lastNode(currP, head);

	if ( currP )
	{
		printCurrent(currP);
		deleteCurrent(currP, head);
		saveList(head);
	} else {
		fprintf(stderr, "No notes to pop\n");
	}
}

/* Pops note number n and deletes it. */
void popN(node *currP, node *head, int n)
{
	currP = searchByNoteNum(currP, head, n);
	if ( currP ) {
		popNote(currP, head);
		orderList(currP);
	} else {
		fprintf(stderr, "No note number: %d\n", n);
	}
}


/* Enters UI loop */
void uiLoop(node *currP, node *head) {
	char opt;
	int lastNoteNum = 0;
	menuMessage();

	while (keepRunning) {

		/* Loop until we get something other then a newline */
		while( ( opt = getchar() ) == '\n' );

		/* Prevents the switch being excecuted one last time on SIGINT */
		if ( keepRunning == 0 )
					break;

		lastNoteNum = currP->note_num;

		switch (opt) {

		/* Exit */
		case 'q':
			keepRunning = 0;
			break;

			/* Next */
		case 'd':
			if (listLength(currP)) {
				currP = next(head, currP);
				/* Don't print the note if nothing changed. */
				if (lastNoteNum != currP->note_num && lastNoteNum != 0)
					printCurrent(currP);
			}
			break;

			/* Previous */
		case 'a':
			if (listLength(currP)) {
				currP = previous(head, currP);
				/* Don't print the note if nothing changed. */
				if (lastNoteNum != currP->note_num && lastNoteNum != 0)
					printCurrent(currP);
			}
			break;

			/* Get input and append to list */
		case 'w':
			currP = appendNote(inputBuffer, MAX_MESSAGE_SIZE, currP, head);
			break;

			/* Print */
		case 'e':
			printList(currP);
			break;

			/* Delete all notes */
		case 'g':
			deleteAllNotesInteractive(currP, head);
			currP = head;
			break;

			/* Find and print all notes containing search term */
		case 'f':
			printAllWithSubStringInteractive(currP, head);
			break;

			/* Print menu message */
		case 'm':
			menuMessage();
			break;

			/* Delete Current note */
		case 'c':
			currP = deleteCurrentInteractive(currP, head);
			break;

		default:
			printf("Invalid Entry\n");
			break;
		}
	}
}

/* Handles SIGINT by killing run loop */
void sigintHandler(int sig)
{
	printf("\nSIGINT received, press any key to quit.\n");
	keepRunning = 0;
}

/* Runs Terminote in interactive mode */
void runInteractive()
{
	node *head, *currP;
	create_list(&head, &currP);

	char *path;
	if ( getDataPath(pathBuffer, MAX_PATH_SIZE, "terminote.data") )
		path = pathBuffer;
	else
	{
		fprintf(stderr, "Error retrieving path\nAborting\n");
		exit(1);
	}

	loadList(currP);

	signal(SIGINT, sigintHandler);

	uiLoop(currP, head);

	currP=head;
	saveList(head);
	destroy(currP);
}

/* Runs Terminote in non interactive mode */
void runNonInteractive(Options *options, int argc, char **argv)
{

	 /* If there are no arguments we'll go ahead and add the data to the list */
	if (argc <= 1) {
		/* Set up the list */
		node *head, *currP;
		create_list(&head, &currP);
		loadList(head);

		/* Read data from the pipe */
		if ( getInputPipe(inputBuffer, MAX_MESSAGE_SIZE) >= MAX_MESSAGE_SIZE )
			fprintf(stderr, "Input too large, truncated\n");

		/* If there is only a newline in the buffer then don't add anything */
		if (strlen(inputBuffer) == 1 && (int) inputBuffer[0] == 10) {
			fprintf(stderr, "No Input\n");
		} else {
			/* Otherwise append to list and save */
			append(currP, inputBuffer);
			saveList(head);
		}

	} else  {
		/* Parse the arguments and make sure they are valid */
		parseOptions(options, argc, argv);

		/* Set up the list */
		node *head, *currP;
		create_list(&head, &currP);
		loadList(head);
		executeOptions(options, currP, head);
		exit(0);
	}


}

/* Initialize options struct */
void initOptions(Options *opts)
{
	opts-> pop = 0;
	opts-> popN = 0;

	opts-> delN = 0;
	opts-> delA = 0;

	opts-> printN = 0;
	opts-> printA = 0;

	opts-> searchNotes = 0;
	strncpy(opts->searchTerm, "\0", MAX_MESSAGE_SIZE-1);
}

/* Print options for debugging */
void printOpts(Options *opts)
{
	printf(
			"\npopNote: %d"
			"\npopN: %d"
			"\ndelN: %d"
			"\ndeleteAll: %d"
			"\nprintN: %d"
			"\nprintAll: %d"
			"\nsearchNotes: %d"
			"\nsearchTerm: %s\n",
			opts->pop,
			opts->popN,
			opts->delN,
			opts->delA,
			opts->printN,
			opts->printA,
			opts->searchNotes,
			opts->searchTerm
			);
}

/* Parse command line options */
void parseOptions(Options *options, int argc, char **argv)
{
	char opt;
	initOptions(options);

	char *nArg, *dArg, *sArg, *fArg;
	nArg=dArg=sArg=fArg=NULL;

	while ( (opt = getopt(argc, argv, "hpn:d:rs:cf:a:") ) != -1)
	{
		switch (opt) {

			case 'h':
				printf("Print help\n");
				exit(0);
				break;

			/* Pop Note */
			case 'p':
				options->pop = 1;
				break;

			/* Pop n */
			case 'n':
				nArg = optarg;
				break;

			/* Delete n */
			case 'd':
				dArg = optarg;
				break;

			/* Delete all notes */
			case 'r':
				options->delA = 1;
				break;

			/* Print n */
			case 's':
				sArg = optarg;
				break;

			/* Print all notes */
			case 'c':
				options->printA = 1;
				break;

			case 'f':
				options->searchNotes = 1;
				strcpy(options->searchTerm, optarg);
				break;

			/* Append note to list */
			case 'a':
				options->append = 1;
				options->appendStr = optarg;
				break;

			case '?':
				if ( optopt == 'n')
				{
					fprintf(stderr, "Error: -n requires a an argument (integer)\n");
					exit(1);
				} else if ( optopt == 'd' )
				{
					fprintf(stderr, "Error: -d requires a an argument (integer)\n");
					exit(1);
				} else if ( optopt == 's')
				{
					fprintf(stderr, "Error: -s requires an argument (integer)\n");
					exit(1);
				} else if ( optopt == 'f' )
				{
					fprintf(stderr, "Error: -f requires an argument (string)\n");
					exit(1);
				} else if ( optopt == 'a' )
				{
					fprintf(stderr, "Error: -a requires an argument (string)\n");
					exit(1);
				} else {
					/* Probably some random option that isn't supported, just exit */
					exit(1);
				}

				break;

			default:
				fprintf(stderr, "When strange things happen, are you going round the twist? Aborting.\n");
				abort();
				break;
		}
	}

	/* Ensures -n option is an integer */
	if ( nArg )
	{
		if ( !isInteger(nArg) )
		{
			fprintf(stderr, "Error: -n requires an integer\n");
			exit(1);
		} else {
			sscanf(nArg, "%d", &options->popN);
		}
	}

	/* Ensures -d option is an integer */
	if ( dArg )
	{
		if ( !isInteger(dArg) )
		{
			fprintf(stderr, "Error: -d requires an integer\n");
			exit(1);
		} else {
			sscanf(dArg, "%d", &options->delN);
		}
	}
	/* Ensures -s option is an integer */
	if ( sArg )
	{
		if ( !isInteger(sArg) )
		{
			fprintf(stderr, "Error: -s requires an integer\n");
			exit(1);
		} else {
			sscanf(sArg, "%d", &options->printN);
		}
	}

	validateOptions(options);
}

/* Ensures options make sense */
void validateOptions(Options *opts) {
	int optArr[OPT_NUM];
	enum {
		POP, POPN, DELN, DELA, PRINTN, PRINTA, SEARCH, APPEND
	};
	optArr[POP] = opts->pop;
	optArr[POPN] = opts->popN;
	optArr[DELN] = opts->delN;
	optArr[DELA] = opts->delA;
	optArr[PRINTN] = opts->printN;
	optArr[PRINTA] = opts->printA;
	optArr[SEARCH] = opts->searchNotes;
	optArr[APPEND] = opts->append;

	for (int i = 0; i < OPT_NUM; ++i) {
		if (optArr[i])
			for (int j = i + 1; j < OPT_NUM; ++j) {
				if (optArr[j] && j != OPT_NUM - 1) {
					fprintf(stderr, "Too many arguments.\n");
					exit(1);
				}
			}
	}
}



void executeOptions(Options *opts, node *currP, node *head)
{
	if ( opts->pop )
	{
		popNote(currP, head);
	} else if ( opts->popN ) {
		popN(currP, head, opts->popN);
	} else if ( opts->delA ) {
		deleteA(currP, head);
	} else if ( opts->delN ) {
		deleteN(currP, head, opts->delN);
	} else if ( opts->printN ) {
		printN(currP, head, opts->printN);
	} else if ( opts->printA ) {
		printList(currP);
	} else if ( opts->searchNotes ) {
		printAllWithSubString(currP, head, opts->searchTerm);
	} else if ( opts->append )
	{
		append(currP, opts->appendStr);
		saveList(head);
	} else {
		fprintf(stderr, "Something strange happened in executeOptions()\n");
	}

	destroy(head);
}




