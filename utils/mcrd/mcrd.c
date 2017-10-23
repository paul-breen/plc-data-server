/******************************************************************************
* PROJECT:  PDS Utilities                                                     * 
* MODULE:   mcrd.c                                                            *
* PURPOSE:  Utility program to read multiple tags in the PDS's shared memory  *
*           segment - continuously                                            *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     2000-08-06                                                        *
******************************************************************************/

#include "mcrd.h"

/******************************************************************************
* Globals                                                                     *
******************************************************************************/
int quit_flag = 0;                /* Global quit flag */

extern int LINES;                 /* These define the physical screen size */
extern int COLUMNS;

/* Pointers to the window structures */
static WINDOW *mainwin = NULL, *datawin = NULL;
static WINDOW *errwin = NULL, *hdrwin = NULL;

/* Colour definition for error messages */
static short int err_colour = 0;

/******************************************************************************
* The main function.                                                          *
******************************************************************************/
int main(int argc, char *argv[])
{
  plctaglist taglist;

  /* Initialise curses & setup the error and header windows */
  setup_mainwin();
  setup_errwin();
  setup_hdrwin();

  parse_cmdln(&argc, argv);

  /* Set up the taglist structure using command line args */
  setup_taglist(argc, argv, &taglist);

  install_signal_handler();

  mcrd_main(&taglist);            /* Main loop.  Read data continuously */

  /* Clean up the windows and exit */
  delwin(errwin);
  delwin(hdrwin);
  terminate();

  return 0;
}



/******************************************************************************
* Function to clean up before terminating the program                         *
*                                                                             *
* Pre-condition:  Quit flag is true                                           *
* Post-condition: Program is cleaned up and terminated                        *
******************************************************************************/
void terminate()
{
  /* Print the final exit message */
  move((LINES - 4), 20);
  printw("%s: cleaning up and terminating", PROGNAME);
  refresh();

  endwin();                       /* Uninitialise the screen */

  exit(0);
}



/******************************************************************************
* Function to install a signal handler                                        *
*                                                                             *
* Pre-condition:  Program is running, signal is received                      *
* Post-condition: Signal handler is called                                    *
******************************************************************************/
void install_signal_handler()
{
  signal(SIGTERM, set_quit);
  signal(SIGINT, set_quit);
  signal(SIGQUIT, set_quit);
}



/******************************************************************************
* Function to handle quit signals                                             *
*                                                                             *
* Pre-condition:  Signal has been received                                    *
* Post-condition: Quit flag is set, signal handler is re-installed            *
******************************************************************************/
void set_quit()
{
  quit_flag = 1;
  install_signal_handler();
}



/******************************************************************************
* Function to setup the main window                                           *
*                                                                             *
* Pre-condition:  Function is called                                          *
* Post-condition: Curses is initialised.  Default program features are setup. *
*                 On success a zero is returned, on error the program is      *
*                 aborted                                                     *
******************************************************************************/
int setup_mainwin()
{
  int retval = 0;

  /* Initialise the screen */
  if(!(mainwin = initscr()))
  {
    fprintf(stderr, "%s: error initialising the screen\n", PROGNAME);
    exit(1);
  }

  /* Use colour if the terminal supports it */
  if(has_colors())
  {
    if(start_color() != ERR)
    {
      /* Initialise colour definition for error messages */
      init_pair(err_colour, COLOR_WHITE, COLOR_RED);
    }
  }

  cbreak();                       /* Don't wait for nl to get input */
  noecho();                       /* Don't echo input */
  nonl();                         /* Don't do nl conversions */
  keypad(stdscr, TRUE);           /* Enable translation of function keys */
  nodelay(mainwin, TRUE);         /* Make input functions non-blocking */
  leaveok(mainwin, TRUE);         /* Turn off the cursor */
  curs_set(0);                    /* Make the cursor invisible */

  return retval;
}



/******************************************************************************
* Function to setup the error reporting window                                *
*                                                                             *
* Pre-condition:  Curses must be initialised.  Function is called             *
* Post-condition: A window exists for error reporting. On success a zero is   *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int setup_errwin()
{
  int retval = 0;

  /* Set up a window for printing error messages in */
  if(!(errwin = newwin(EW_LINES, EW_COLS, EW_Y, EW_X)))
  {
    retval = -1;
    move((LINES - 4), 20);
    printw("%s: error creating 'error' window", PROGNAME);
    refresh();
    sleep(ERRMSG_PAUSE);
    terminate();
  }
  else
  {
    box(errwin, 0, 0);            /* Draw the window's border */

    /* Set the error window's text attributes.  If this terminal supports
       colour then use it else use default highlighting */
    if(err_colour)
      wattron(errwin, COLOR_PAIR(err_colour));
    else
      wattron(errwin, A_REVERSE);
  }

  return retval;
}



/******************************************************************************
* Function to setup the header window                                         *
*                                                                             *
* Pre-condition:  Curses must be initialised.  The error window must exist.   *
*                 Function is called                                          *
* Post-condition: A window exists for header data. On success a zero is       *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int setup_hdrwin()
{
  int retval = 0;

  /* Set up the program's header message window */
  if(!(hdrwin = newwin(HW_LINES, HW_COLS, HW_Y, HW_X)))
  {
    retval = -1;
    mvwprintw(errwin, DEFP_Y, DEFP_X, "%s: error creating 'header' window",
    PROGNAME);
    wrefresh(errwin);
    sleep(ERRMSG_PAUSE);
    terminate();
  }
  else
  {
    box(hdrwin, 0, 0);
 
    /* Set text attributes and print the program's header message */
    wattron(hdrwin, A_BOLD);
    mvwprintw(hdrwin, DEFP_Y, DEFP_X, "(CTRL-C to quit)");
    wattroff(hdrwin, A_BOLD);
    wrefresh(hdrwin);
  }

  return retval;
}



/******************************************************************************
* Function to setup the data window                                           *
*                                                                             *
* Pre-condition:  Curses must be initialised.  The error window must exist.   *
*                 The number of lines for the window is passed to the         *
*                 function                                                    *
* Post-condition: A window exists for a data table. On success a zero is      *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int setup_datawin(int nlines)
{
  int retval = 0;

  /* Set up the program's data window */
  if(!(datawin = newwin((nlines + (DW_OFFSET * 2)), DW_COLS, DW_Y, DW_X)))
  {
    retval = -1;
    mvwprintw(errwin, DEFP_Y, DEFP_X, "%s: error creating 'data' window",
    PROGNAME);
    wrefresh(errwin);
    sleep(ERRMSG_PAUSE);
    terminate();
  }
  else
  {
    box(datawin, 0, 0);

    /* Print the data window's column headings */
    wattron(datawin, A_BOLD);
    mvwprintw(datawin, 0, DEFP_X, "%s", "TAG");
    mvwprintw(datawin, 0, (DEFP_X + DW_TAGLEN), "%s", "VALUE");
    wattroff(datawin, A_BOLD);
    wrefresh(datawin);
  }

  return retval;
}



/******************************************************************************
* Function to refresh the header window                                       *
*                                                                             *
* Pre-condition:  Curses must be initialised.  The header window must exist.  *
*                 Function is called                                          *
* Post-condition: The header window is refreshed. On success a zero is        *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int refresh_hdrwin()
{
  int retval = 0;

  box(hdrwin, 0, 0);
 
  /* Set text attributes and print the program's header message */
  wattron(hdrwin, A_BOLD);
  mvwprintw(hdrwin, DEFP_Y, DEFP_X, "(CTRL-C to quit)");
  wattroff(hdrwin, A_BOLD);
  wrefresh(hdrwin);

  return retval;
}



/******************************************************************************
* Function to refresh the data window                                         *
*                                                                             *
* Pre-condition:  Curses must be initialised.  The data window must exist.    *
*                 Function is called                                          *
* Post-condition: The data window is refreshed. On success a zero is          *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int refresh_datawin()
{
  int retval = 0;

  box(datawin, 0, 0);

  /* Print the data window's column headings */
  wattron(datawin, A_BOLD);
  mvwprintw(datawin, 0, DEFP_X, "%s", "TAG");
  mvwprintw(datawin, 0, (DEFP_X + DW_TAGLEN), "%s", "VALUE");
  wattroff(datawin, A_BOLD);
  wrefresh(datawin);

  return retval;
}



/******************************************************************************
* Function to print a message in the error window                             *
*                                                                             *
* Pre-condition:  The error window must exist.  The message to print is       *
*                 passed to the function                                      *
* Post-condition: The message is printed in the error window. On success a    *
*                 zero is returned, on error a -1 is returned                 *
******************************************************************************/
int print_err(char *msg)
{
  int retval = 0;
  char tmpbuf[EW_COLS + 1];

  /* Format the error message for the error window.  This ensures that the
     whole of the error window is rendered (with the setup attributes) */ 
  snprintf(tmpbuf, EW_COLS, "%-*s%-*s", DEFP_X, " ", EW_COLS, msg);

  /* Print the message in the error window - overlaying the data window if
     necessary */
  overlay(datawin, errwin);
  werase(errwin);
  mvwprintw(errwin, DEFP_Y, 0, tmpbuf);
  box(errwin, 0, 0);
  wrefresh(errwin);

  sleep(ERRMSG_PAUSE);            /* Allow user to read the error message */

  return retval;
}
 


/******************************************************************************
* Function to parse the program's command line                                *
*                                                                             *
* Pre-condition:  The count of command line args and the args are passed to   *
*                 function                                                    *
* Post-condition: The program's command line is parsed. On success a zero is  *
*                 returned, on error a -1 is returned                         *
******************************************************************************/
int parse_cmdln(int *argc, char **argv)
{
  int retval = 0;
  char *inbuf = NULL, *cp = NULL;
  FILE *fin = NULL;

  if(*argc < 2)
  {
    if((fin = stdin))
    {
      /* N.B.:  Because the argv matrix points to this buffer and is used
         outside of this function, we don't free the allocated storage for
         the buffer */
      if(!(inbuf = (char *) malloc(MCRD_BUFSIZ * sizeof(char))))
      {
        print_err("Memory allocation error");
        return -1;
      }

      while(!feof(fin))           /* Read stdin into buffer */
      {
        if(fread(inbuf, MCRD_BUFSIZ, 1, fin) < 0)
        {
          retval = -1;
          mvwprintw(errwin, DEFP_Y, DEFP_X, "%s: args overflowed buffer", PROGNAME);
          wrefresh(errwin);
          sleep(ERRMSG_PAUSE);
          terminate();
        }
      }

      /* Split buffer on whitespace and assign to arg vector */
      for(*argc = 1, cp = inbuf; *argc < MAX_ARGV - 1; cp = NULL)
      {
        if(!(argv[(*argc)++] = (char *) strtok(cp, " \t\n")))
          break;
      }
      argv[MAX_ARGV - 1] = NULL;  /* Mark end of args */
      (*argc)--;
    }
  }
  else
  {
     if(strcmp(argv[1], HELP_OPT) == 0)
       print_usage();
  }

  return retval;
}
 


/******************************************************************************
* Function to print the program's usage message                               *
*                                                                             *
* Pre-condition:  The error window must exist.  The function is called        *
* Post-condition: The program's usage message is printed                      *
******************************************************************************/
void print_usage()
{
  /* Print the program's usage message and quit */
  mvwprintw(errwin, DEFP_Y, DEFP_X, "Usage: %s tagname [tagname ...] OR %s <stdin>", PROGNAME, PROGNAME);
  wrefresh(errwin);
  sleep(ERRMSG_PAUSE);
  terminate();
}



/******************************************************************************
* Function to setup the taglist struct                                        *
*                                                                             *
* Pre-condition:  The count of command line args, the args and storage for    *
*                 the taglist are passed to the function                      *
* Post-condition: The taglist struct is filled with the program's command     *
*                 line args. On success a zero is returned, on error a -1 is  *
*                 returned                                                    *
******************************************************************************/
int setup_taglist(int argc, char **argv, plctaglist *taglist)
{
  int retval = 0;
  plctag *tags = NULL;
  int i = 0, ntags = 0, tagssz = 0;

  ntags = argc - NPRETAG_ARGS;    /* Calc number of tags & size of array */
  tagssz = sizeof(plctag) * ntags;

  if(!(tags = (plctag *) malloc(tagssz)))
  {
    retval = -1;
    mvwprintw(errwin, DEFP_Y, DEFP_X, "%s: memory allocation error", PROGNAME);
    wrefresh(errwin);
    sleep(ERRMSG_PAUSE);
    terminate();
  }
  else
  {
    memset(tags, 0, tagssz);

    /* Set up the tags array from args passed on the command line */
    for(i = 0; i < ntags; i++)
    {
      strcpy(tags[i].name, argv[i+NPRETAG_ARGS]);
      tags[i].type = 'i';         /* Default type of integer */
    }

    taglist->tags = tags;         /* Now construct a plc taglist struct */ 
    taglist->ntags = ntags;
  }

  return retval;
}



/******************************************************************************
* Function to encapsulate the program's main loop                             *
*                                                                             *
* Pre-condition:  The filled taglist struct is passed to the function         *
* Post-condition: The program enters its main loop.  If a signal is caught,   *
*                 it exits the loop                                           *
******************************************************************************/
int mcrd_main(plctaglist *taglist)
{
  int retval = 0;
  pdsconn *conn = NULL;
  char tagvalue[PDS_TAGVALUE_LEN] = "\0", date_heading[DTHDR_LEN] = "\0";
  time_t clock;
  int i = 0, ival = 0;

  /* Connect to the server */
  if(!(conn = (pdsconn *) PDSconnect(PDS_IPCKEY)))
  {
    mvwprintw(errwin, DEFP_Y, DEFP_X, "PDS memory allocation error", PROGNAME);
    wrefresh(errwin);
    sleep(ERRMSG_LGPAUSE);
    terminate();
  }

  if(PDScheck_conn_status(conn) == PDS_CONN_OK)
  {
    /* Setup the program's data window */
    setup_datawin(taglist->ntags);

    while(!quit_flag)
    {
      /* Get the user specified tag's value and display in the data window */
      if(PDSget_taglist(conn, taglist) != -1)
      {
        /* Check the status of the PLC at query time.  If a status condition
           exists on the connection, print the PLC status message */
        if(PDScheck_plc_status(conn) != PDS_PLC_OK)
        {
          mvwprintw(errwin, DEFP_Y, DEFP_X, "%s: error - %s", PROGNAME, PDSprint_plc_status(conn));
          wrefresh(errwin);
        }
        else
        {
          werase(datawin);        /* Clear window of old data */

          /* Cycle through the taglist and print tagname & tagvalue... */
          for(i = 0; i < taglist->ntags; i++)
          {
            /* Work out the ASCII value for this tag's value.
               If it's printable, print it else print blanks */
            ival = atoi(taglist->tags[i].value);
            sprintf(tagvalue, "%c%c", isprint((ival >> 8)) ? (char) (ival >> 8) : ' ', isprint((ival & 0xff)) ? (char) (ival & 0xff) : ' ');

            mvwprintw(datawin, (i + DW_OFFSET), DEFP_X, "%s", taglist->tags[i].name);
            mvwprintw(datawin, (i + DW_OFFSET), (DEFP_X + DW_TAGLEN), "%s", taglist->tags[i].value);
            mvwprintw(datawin, (i + DW_OFFSET), (DEFP_X + DW_TAGLEN + DW_VALLEN), "%s", tagvalue);
          }
          refresh_datawin();      /* Now refresh the data window */
        }
      }
      else
      {
        mvwprintw(datawin, DEFP_Y, DEFP_X, "tag(s) not found!!");
        refresh_datawin(); 
      }

      /* Construct the date/time heading and print it */
      clock = time(NULL); 
      strftime(date_heading, DTHDR_LEN, "%d/%m/%y %H:%M:%S", localtime(&clock));

      mvwprintw(hdrwin, DEFP_Y, (HW_COLS - (DTHDR_LEN + DEFP_X)), "%s", date_heading);
      wrefresh(hdrwin);
    }

    delwin(datawin);

    PDSdisconnect(conn);
  }
  else
  {
    retval = -1;
    mvwprintw(errwin, DEFP_Y, DEFP_X, "%s: error connecting to the PDS: %s", PROGNAME, PDSprint_conn_status(conn));
    wrefresh(errwin);
    sleep(ERRMSG_LGPAUSE);
    terminate();
  }

  return retval;
}

