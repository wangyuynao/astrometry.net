/*
  This file is part of the Astrometry.net suite.
  Copyright 2006, 2007 Dustin Lang, Keir Mierle and Sam Roweis.

  The Astrometry.net suite is free software; you can redistribute
  it and/or modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation, version 2.

  The Astrometry.net suite is distributed in the hope that it will be
  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the Astrometry.net suite ; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include "starutil.h"
#include "mathutil.h"
#include "boilerplate.h"
#include "bl.h"
#include "solvedfile.h"

const char* OPTIONS = "hum:SM:j";

void printHelp(char* progname) {
	boilerplate_help_header(stderr);
	fprintf(stderr, "\nUsage: %s <solved-file> ...\n"
			"    [-u]: print UNsolved fields\n"
			"    [-j]: just the field numbers, no headers, etc.\n"
			"    [-m <max-field>]: for unsolved mode, max field number.\n"
			"    [-w]: format for the wiki.\n"
			"    [-M <variable-name>]: format for Matlab.\n"
			"\n", progname);
}

extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char** args) {
    int argchar;
	char* progname = args[0];
	char** inputfiles = NULL;
	int ninputfiles = 0;
	int i;
	bool unsolved = FALSE;
	int maxfield = 0;
	bool wiki = FALSE;
	char* matlab = NULL;
	bool justnums = FALSE;

    while ((argchar = getopt (argc, args, OPTIONS)) != -1) {
		switch (argchar) {
		case 'j':
			justnums = TRUE;
			break;
		case 'M':
			matlab = optarg;
			break;
		case 'w':
			wiki = TRUE;
			break;
		case 'u':
			unsolved = TRUE;
			break;
		case 'm':
			maxfield = atoi(optarg);
			break;
		case 'h':
		default:
			printHelp(progname);
			exit(-1);
		}
	}
	if (optind < argc) {
		ninputfiles = argc - optind;
		inputfiles = args + optind;
	} else {
		printHelp(progname);
		exit(-1);
	}

	if (matlab)
		printf("%s=[", matlab);

	for (i=0; i<ninputfiles; i++) {
		int j;
		il* list;

		if (!matlab && !justnums)
			printf("File %s\n", inputfiles[i]);
		if (wiki)
			printf("|| %i || ", i+1);

		if (unsolved)
			list = solvedfile_getall(inputfiles[i], 1, maxfield, 0);
		else
			list = solvedfile_getall_solved(inputfiles[i], 1, maxfield, 0);

		if (!list) {
			fprintf(stderr, "Failed to get list of fields.\n");
			exit(-1);
		}
		for (j=0; j<il_size(list); j++)
			printf("%i ", il_get(list, j));
		il_free(list);

		if (wiki)
			printf(" ||");

		printf("\n");
	}
	if (matlab)
		printf("];\n");

	return 0;
}
