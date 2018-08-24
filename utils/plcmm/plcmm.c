/******************************************************************************
* PROJECT:  PLC Utilities                                                     * 
* MODULE:   plcmm.c                                                           *
* PURPOSE:  Utility program to read a shared memory segment with its data     *
*           populated from a PLC                                              *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-10-06                                                        *
******************************************************************************/

#include "plcmm.h"

/******************************************************************************
* The main function.                                                          *
******************************************************************************/
int main(int argc, char *argv[])
{
  pdsconn *conn = NULL;
  plcmm_args args;
  int len = 0;

  if(parse_plcmm_cmdln(argc, argv, &args) == -1)
    exit(1);

  /* Connect to the server */
  if(!(conn = (pdsconn *) PDSconnect(PDS_IPCKEY)))
  {
    fprintf(stderr, "PDS memory allocation error\n", PROGNAME);
    exit(1);
  }

  /* N.B.: The CHECK_PRINT() macro checks to see if a particular report section
           (indicated by the single char specifier) has been asked to be
           included in the output by the user */

  if(PDScheck_conn_status(conn) == PDS_CONN_OK)
  {
    /* Calc. the longest tagname for setting up the field length */
    PDSprint_data_memmap(stdout, conn, calc_max_taglen, (void *) &args);

    /* Print the data tags heading (also get the heading line length) */
    if(CHECK_PRINT(args.sections, 'd') && CHECK_PRINT(args.sections, 'h'))
    {
      /* If called as a CGI script, print the beginning table tag */
      if(args.cgi_mode)
        fprintf(stdout, "<table border>\n");

      len = print_data_tag_headers(stdout, &args);

      if(CHECK_PRINT(args.sections, 'l') && !args.cgi_mode)
        UNDERLINE(len);
    }

    /* Print the data tags data */
    if(CHECK_PRINT(args.sections, 'd'))
    {
      PDSprint_data_memmap(stdout, conn, print_data_tag, (void *) &args);

      if(args.cgi_mode)
        fprintf(stdout, "</table>\n");

      if(CHECK_PRINT(args.sections, 'l') && !args.cgi_mode)
        UNDERLINE(len);
    }

    /* Print the status tags heading */
    if(CHECK_PRINT(args.sections, 's') && CHECK_PRINT(args.sections, 'h'))
    {
      if(args.cgi_mode)
        fprintf(stdout, "<table border>\n");

      len = print_status_tag_headers(stdout, &args);

      if(CHECK_PRINT(args.sections, 'l') && !args.cgi_mode)
        UNDERLINE(len);
    }

    /* Print the status tags data */
    if(CHECK_PRINT(args.sections, 's'))
    {
      PDSprint_status_memmap(stdout, conn, print_status_tag, (void *) &args);

      if(args.cgi_mode)
        fprintf(stdout, "</table>\n");

      if(CHECK_PRINT(args.sections, 'l') && !args.cgi_mode)
        UNDERLINE(len);
    }

    /* Print the summary footer */
    if(CHECK_PRINT(args.sections, 'f'))
      PDSprint_footer_memmap(stdout, conn, print_footer, (void *) &args);

    PDSdisconnect(conn);
  }
  else
  {
    fprintf(stderr, "%s: error connecting to the PDS\n", PROGNAME);
    fprintf(stderr, "%s: %s\n", PROGNAME, PDSprint_conn_status(conn));
    exit(1);
  }

  return 0;
}


 
/******************************************************************************
* Function to parse the command line arguments                                *
*                                                                             *
* Pre-condition:  The command line arguments and a struct for storage are     *
*                 passed to the function                                      *
* Post-condition: Arguments are parsed and if found are stored in the         *
*                 appropriate member of the structure.  On error a -1 is      *
*                 returned                                                    *
******************************************************************************/
int parse_plcmm_cmdln(int argc, char *argv[], plcmm_args *args)
{
  int opt = 0, retval = 0;
  extern char *optarg;
  extern int opterr, optind;

  /* Set up some defaults */
  strcpy(args->fields, (char *) DEFAULT_FIELDS);
  strcpy(args->sections, (char *) DEFAULT_SECTIONS);
  args->cgi_mode = 0;
  args->max_taglen = DEFAULT_MAX_TAGLEN;

  opterr = 0;                     /* Turn off getopt()'s error messages */

  while((opt = getopt(argc, argv, "f:s:ch")) != -1)
  {
    switch(opt)
    {
      case 'f' :                  /* The output fields flag */ 
        if(optarg)
          strcpy(args->fields, (char *) optarg);
      break; 

      case 's' :                  /* The output sections flag */ 
        if(optarg)
          strcpy(args->sections, (char *) optarg);
      break; 

      case 'c' :                  /* Run in CGI mode */ 
        args->cgi_mode = 1;
      break; 

      /* Option should be followed by a command line argument */ 
      case ':' :
        fputs("Option should take an argument\n", stderr);
        retval = -1;
      break;

      /* Unknown option OR the help option - print usage */
      case '?' :  
      case 'h' :  
        fprintf(stderr, "Usage: %s [-f field_list ] [-s section_list]"
        " [-c]\n\n"
        "  -f allows you to specify which fields to include in the output\n"
        "  -s allows you to specify which sections to include in the output\n"
        "  -c runs the program in CGI mode (outputs HTML tables)\n"
        "  -h prints this help text\n\n"
        "  Field list specifiers: i = tag fully-qualified ID,\n"
        "  n = tag name, v = tag value, r = tag reference,\n"
        "  s = tag status, m = tag last mod. time,\n"
        "  default = %s\n"
        "  Section list specifiers: h = field headers, l = underlines,\n"
        "  d = data tag block, s = status tag block, f = report footer,\n"
        "  default = %s (N.B.: Some specifiers require others e.g., h {d|s})\n",
        PROGNAME, DEFAULT_FIELDS, DEFAULT_SECTIONS);
        retval = -1;
      break;
    }
  }   

  return retval;
}



/******************************************************************************
* Function to print the user specified field headers of a data tag            *
*                                                                             *
* Pre-condition:  An output stream & the cmdln args struct containing the     *
*                 field specifiers to be printed are passed to the function   *
* Post-condition: The data tags heading is printed to the output stream.      *
*                 The length of the header is returned or -1 on error         *
******************************************************************************/
int print_data_tag_headers(FILE *os, plcmm_args *args)
{
  int retval = -1, nfields = 0, i = 0, line_len = 0;
  char header[512] = "\0";

  nfields = strlen(args->fields);

  if(nfields > 0)
  {
    if(args->cgi_mode)
      fprintf(os, "<tr>\n");

    for(i = 0; i < nfields; i++)
    {
      switch(args->fields[i])
      {
        case 'i' :
        case 'n' :
          if(args->cgi_mode)
            sprintf(header, "%s<th>%s</th>", header, "TAG");
          else
            sprintf(header, "%s%c%-*s", header, COL_SEP(line_len),
            args->max_taglen + 1, "TAG");
          line_len += args->max_taglen + 2;
        break;

        case 'v' :
          if(args->cgi_mode)
            sprintf(header, "%s<th>%s</th>", header, "VALUE");
          else
            sprintf(header, "%s%c%10s", header, COL_SEP(line_len), "VALUE");
          line_len += 11;
        break;

        case 'r' :
          if(args->cgi_mode)
            sprintf(header, "%s<th>%s</th>", header, "REF");
          else
            sprintf(header, "%s%c%10s", header, COL_SEP(line_len), "REF");
          line_len += 11;
        break;

        case 's' :
          if(args->cgi_mode)
            sprintf(header, "%s<th>%s</th>", header, "STATUS");
          else
            sprintf(header, "%s%c%10s", header, COL_SEP(line_len), "STATUS");
          line_len += 11;
        break;

        case 'm' :
          if(args->cgi_mode)
            sprintf(header, "%s<th>%s</th>", header, "MTIME");
          else
            sprintf(header, "%s%c%-24s", header, COL_SEP(line_len), "MTIME");
          line_len += 31;
        break;

        default :
          fprintf(stderr, "%s: unknown field specifier %c\n",
          PROGNAME, args->fields[i]);
          return retval;
        break;
      }
    }
    fprintf(os, "%s\n", header);       /* Print the header line */

    if(args->cgi_mode)
      fprintf(os, "</tr>\n");

    retval = line_len;
  }

  return retval;
}



/******************************************************************************
* Callback function to print the user specified fields of a data tag          *
*                                                                             *
* Pre-condition:  An output stream, the tag struct to be printed & the cmdln  *
*                 args struct cast as a (void *) are passed to the function.  *
*                 If the output stream is NULL, a default is used             *
* Post-condition: The specified fields of the tag (in the fields member of    *
*                 the args struct - aux) are printed to the output stream.    *
*                 On error a -1 is returned                                   *
******************************************************************************/
int print_data_tag(FILE *os, pdstag *tag, void *aux)
{
  char fqid[PDS_TAG_FQID_LEN] = "\0", mtimestr[TMSTAMP_LEN] = "\0";
  char data[DATA_LEN] = "\0";
  time_t mtime = 0;
  int retval = -1, nfields = 0, i = 0;
  plcmm_args *args = (plcmm_args *) aux;

  if(tag && args)
  {
    nfields = strlen(args->fields);

    if(!os)
      os = PDS_DEFAULT_PRINT_DATA_TAG_STREAM;

    if(nfields > 0)
    {
      if(args->cgi_mode)
        fprintf(os, "<tr>\n");

      for(i = 0; i < nfields; i++)
      {
        switch(args->fields[i])
        {
          case 'i' :
            /* Construct tag's fully-qualified ID */
            PDS_GET_TAG_FQID(fqid, tag);

            if(args->cgi_mode)
              sprintf(data, "%s<td>%s</td>", data, fqid);
            else
              sprintf(data, "%s%c%-*s", data, COL_SEP(strlen(data)), args->max_taglen + 1, fqid);
          break;

          case 'n' :
            if(args->cgi_mode)
              sprintf(data, "%s<td>%s</td>", data, PDStag_get_name(tag));
            else
              sprintf(data, "%s%c%-*s", data, COL_SEP(strlen(data)), args->max_taglen + 1, PDStag_get_name(tag));
          break;

          case 'v' :
            if(args->cgi_mode)
              sprintf(data, "%s<td>%u</td>", data, PDStag_get_value(tag));
            else
              sprintf(data, "%s%c%10u", data, COL_SEP(strlen(data)), PDStag_get_value(tag));
          break;

          case 'r' :
            if(args->cgi_mode)
              sprintf(data, "%s<td>%d</td>", data, PDStag_get_ref(tag));
            else
              sprintf(data, "%s%c%10d", data, COL_SEP(strlen(data)), PDStag_get_ref(tag));
          break;

          case 's' :
            if(args->cgi_mode)
              sprintf(data, "%s<td>%d</td>", data, PDStag_get_status(tag));
            else
              sprintf(data, "%s%c%10d", data, COL_SEP(strlen(data)), PDStag_get_status(tag));
          break;

          case 'm' :
            /* Construct this tag's last modification time as a string */
            mtime = PDStag_get_mtime(tag);
            strftime(mtimestr, TMSTAMP_LEN, TMSTAMP_FMT, localtime(&mtime));

            if(args->cgi_mode)
              sprintf(data, "%s<td>%s</td>", data, mtimestr);
            else
              sprintf(data, "%s%c%-24s", data, COL_SEP(strlen(data)), mtimestr);
          break;

          default :
            fprintf(stderr, "%s: unknown field specifier %c\n", PROGNAME, args->fields[i]);
            return retval;
          break;
        }
      }
      fprintf(os, "%s\n", data);       /* Print the tag's data */

      if(args->cgi_mode)
        fprintf(os, "</tr>\n");

      retval = 0;
    }
  }

  return retval;
}



/******************************************************************************
* Function to print the user specified field headers of a status tag          *
*                                                                             *
* Pre-condition:  An output stream & the cmdln args struct containing the     *
*                 field specifiers to be printed are passed to the function   *
* Post-condition: The status tags heading is printed to the output stream.    *
*                 The length of the header is returned or -1 on error         *
******************************************************************************/
int print_status_tag_headers(FILE *os, plcmm_args *args)
{
  int retval = -1, nfields = 0, i = 0, line_len = 0;
  char header[512] = "\0";

  nfields = strlen(args->fields);

  if(nfields > 0)
  {
    if(args->cgi_mode)
      fprintf(os, "<tr>\n");

    for(i = 0; i < nfields; i++)
    {
      switch(args->fields[i])
      {
        case 'i' :
        case 'n' :
          if(args->cgi_mode)
            sprintf(header, "%s<th>%s</th>", header, "PLC");
          else
            sprintf(header, "%s%c%-*s", header, COL_SEP(line_len), args->max_taglen + 1, "PLC");
          line_len += args->max_taglen + 2;
        break;

        case 'v' :
          if(args->cgi_mode)
            sprintf(header, "%s<th>%s</th>", header, "STATUS");
          else
            sprintf(header, "%s%c%10s", header, COL_SEP(line_len), "STATUS");
          line_len += 11;
        break;

        case 'r' :
          if(args->cgi_mode)
            sprintf(header, "%s<th>%s</th>", header, "PATH");
          else
            sprintf(header, "%s%c%10s", header, COL_SEP(line_len), "PATH");
          line_len += 11;
        break;

        case 's' :                     /* Not applicable */
          if(!args->cgi_mode)
            sprintf(header, "%s%c%10s", header, COL_SEP(line_len), "");
          line_len += 11;
        break;

        case 'm' :                     /* Not applicable */
          if(!args->cgi_mode)
            sprintf(header, "%s%c%-24s", header, COL_SEP(line_len), "");
          line_len += 31;
        break;

        default :
          fprintf(stderr, "%s: unknown field specifier %c\n", PROGNAME, args->fields[i]);
          return retval;
        break;
      }
    }
    fprintf(os, "%s\n", header);       /* Print the header line */

    if(args->cgi_mode)
      fprintf(os, "</tr>\n");

    retval = line_len;
  }

  return retval;
}



/******************************************************************************
* Callback function to print the user specified fields of a status tag        *
*                                                                             *
* Pre-condition:  An output stream, the tag struct to be printed & the cmdln  *
*                 args struct cast as a (void *) are passed to the function.  *
*                 If the output stream is NULL, a default is used             *
* Post-condition: The specified fields of the tag (in the fields member of    *
*                 the args struct - aux) are printed to the output stream.    *
*                 On error a -1 is returned                                   *
******************************************************************************/
int print_status_tag(FILE *os, pdstag *tag, void *aux)
{
  char fqid[PDS_TAG_FQID_LEN] = "\0", data[512] = "\0";
  int retval = -1, nfields = 0, i = 0;
  plcmm_args *args = (plcmm_args *) aux;

  if(tag && args)
  {
    nfields = strlen(args->fields);

    if(!os)
      os = PDS_DEFAULT_PRINT_STATUS_TAG_STREAM;

    if(nfields > 0)
    {
      if(args->cgi_mode)
        fprintf(os, "<tr>\n");

      for(i = 0; i < nfields; i++)
      {
        switch(args->fields[i])
        {
          case 'i' :
            /* Construct tag's fully-qualified ID */
            PDS_GET_TAG_FQID(fqid, tag);

            if(args->cgi_mode)
              sprintf(data, "%s<td>%s</td>", data, fqid);
            else
              sprintf(data, "%s%c%-*s", data, COL_SEP(strlen(data)), args->max_taglen + 1, fqid);
          break;

          case 'n' :
            if(args->cgi_mode)
              sprintf(data, "%s<td>%s</td>", data, PDStag_get_name(tag));
            else
              sprintf(data, "%s%c%-*s", data, COL_SEP(strlen(data)), args->max_taglen + 1, PDStag_get_name(tag));
          break;

          case 'v' :
            if(args->cgi_mode)
              sprintf(data, "%s<td>%d</td>", data, PDStag_get_status(tag));
            else
              sprintf(data, "%s%c%10d", data, COL_SEP(strlen(data)), PDStag_get_status(tag));
          break;

          case 'r' :
            if(args->cgi_mode)
              sprintf(data, "%s<td>%s</td>", data, PDStag_get_path(tag));
            else
              sprintf(data, "%s%c%10s", data, COL_SEP(strlen(data)), PDStag_get_path(tag));
          break;

          case 's' :                   /* Not applicable */
            if(!args->cgi_mode)
              sprintf(data, "%s%c%10s", data, COL_SEP(strlen(data)), "");
          break;

          case 'm' :                   /* Not applicable */
            if(!args->cgi_mode)
              sprintf(data, "%s%c%-24s", data, COL_SEP(strlen(data)), "");
          break;

          default :
            fprintf(stderr, "%s: unknown field specifier %c\n", PROGNAME, args->fields[i]);
            return retval;
          break;
        }
      }
      fprintf(os, "%s\n", data);       /* Print the tag's data */

      if(args->cgi_mode)
        fprintf(os, "</tr>\n");

      retval = 0;
    }
  }

  return retval;
}



/******************************************************************************
* Callback function to print shared memory statistics in a default manner     *
*                                                                             *
* Pre-condition:  An output stream, the connection struct containing summary  *
*                 data to be printed & any auxilliary data are passed to the  *
*                 function.  If the output stream is NULL, a default is used  *
* Post-condition: The statistics are printed to the output stream.  On error  *
*                 a -1 is returned                                            *
******************************************************************************/
int print_footer(FILE *os, pdsconn *conn, void *aux)
{
  int retval = -1;
  plcmm_args *args = (plcmm_args *) aux;

  if(conn)
  {
    if(!os)
      os = PDS_DEFAULT_PRINT_FOOTER_STREAM;

    if(args->cgi_mode)
      fprintf(os, "<p>\n");

    /* Print the summary footer */
    fprintf(os, "Total number of tags: %d%s", PDSconn_get_ttags(conn), PRINT_EOL(args->cgi_mode));
    fprintf(os, "Number of data tags: %d%s", PDSconn_get_ndata_tags(conn), PRINT_EOL(args->cgi_mode));
    fprintf(os, "Number of status tags: %d%s", PDSconn_get_nstatus_tags(conn), PRINT_EOL(args->cgi_mode));
    fprintf(os, "Query status: %s (%d)%s", PDSprint_plc_status(conn), PDScheck_plc_status(conn), PRINT_EOL(args->cgi_mode));

    if(args->cgi_mode)
      fprintf(os, "</p>\n");

    retval = 0;
  }

  return retval;
}



/******************************************************************************
* Callback function to calc. max. tagname length of all tags in shared mem.   *
*                                                                             *
* Pre-condition:  An output stream, the tag struct to be processed & the      *
*                 args struct cast as a (void *) are passed to the function.  *
*                 The output stream is not used                               *
* Post-condition: The tagname length of this tag is compared to the current   *
*                 max. tagname length & the longer of the two is stored       *
*                 appropriately in the args struct.  The length of the        *
*                 tagname is returned or -1 on error                          *
******************************************************************************/
int calc_max_taglen(FILE *os, pdstag *tag, void *aux)
{
  int len = -1;
  char fqid[PDS_TAG_FQID_LEN] = "\0";
  plcmm_args *args = (plcmm_args *) aux;

  if(tag && args)
  {
    if(strchr(args->fields, (int) 'i'))
    {
      /* Construct tag's fqid & get its length */
      PDS_GET_TAG_FQID(fqid, tag);
      len = strlen(fqid);
    }
    else
    {
      /* Get length of this tag's name */
      len = strlen(PDStag_get_name(tag));
    }

    args->max_taglen = (args->max_taglen > len ? args->max_taglen : len);
  }

  return len;
}

