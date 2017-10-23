%{

/******************************************************************************
* PROJECT:  PLC data server                                                   *
* MODULE:   pds_plc_cnf_scan.l                                                *
* PURPOSE:  lex code to tokenise an ascii text file containing PLC config.    *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-08-24                                                        *
******************************************************************************/

#include "pds_plc_cnf.h"

extern int errout;                /* Declared in the main file */

/******************************************************************************
* Re-define the scanner's function prototype "yylex()"                        *
******************************************************************************/
#define YY_DECL	int yylex(plc_cnf *conf) 

/******************************************************************************
* Globals                                                                     *
******************************************************************************/

/* A name/value list of function codes supported by the PDS */
static pds_lookup_item __pds_funcs[] =
{
{"UNKNOWN", PDS_UNKNOWN_FUNCTION},
{"BREAD", PDS_BREAD}, {"BWRITE", PDS_BWRITE}, {"BRDWR", PDS_BRDWR},
{"CREAD", PDS_CREAD}, {"CWRITE", PDS_CWRITE}, {"CRDWR", PDS_CRDWR},
{"WREAD", PDS_WREAD}, {"WWRITE", PDS_WWRITE}, {"WRDWR", PDS_WRDWR},
{"LREAD", PDS_LREAD}, {"LWRITE", PDS_LWRITE}, {"LRDWR", PDS_LRDWR},
{"FREAD", PDS_FREAD}, {"FWRITE", PDS_FWRITE}, {"FRDWR", PDS_FRDWR},
{"DREAD", PDS_DREAD}, {"DWRITE", PDS_DWRITE}, {"DRDWR", PDS_DRDWR},
{"STAT", PDS_STAT}, {"DIAG", PDS_DIAG}
};

static pds_lookup_list __pds_func_list =
{(sizeof(__pds_funcs) / sizeof(pds_lookup_item)),  __pds_funcs};

/* A name/value list of protocols supported by the PDS */
static pds_lookup_item __pds_protos[] =
{
{"UNKNOWN", PDS_UNKNOWN_COMMS_PROTOCOL},
{"MB_TCPIP", MB_TCPIP},
{"MB_SERIAL_TCPIP", MB_SERIAL_TCPIP},
{"MB_SERIAL", MB_SERIAL},
{"DH_SERIAL_TCPIP", DH_SERIAL_TCPIP},
{"DH_SERIAL", DH_SERIAL},
{"CIP_TCPIP", CIP_TCPIP}
};

static pds_lookup_list __pds_proto_list =
{(sizeof(__pds_protos) / sizeof(pds_lookup_item)),  __pds_protos};

%}

%option noyywrap
%option never-interactive

  /* Line ID chars */
comment_id ^[#]{1}
blank_line_id ^[\n]{1}
block_header_id ^[\^/]{1}
tag_id ^[\t ]{1}

  /* Start states */
%s blockprotocol_state
%s blockfunction_state
%s blockpath_state
%s blockip_addr_state
%s blockport_state
%s blocktty_dev_state
%s blockbase_addr_state
%s blockascii_addr_state
%s blockpollrate_state

%s tagname_state
%s tagref_state
%s tagascii_ref_state
%s tagbit_state

%%

  register int i = 0, j = 0;      /* Declare local vars */

  /* If a line ID is found, then start the appropriate state */
{comment_id} { BEGIN 0; }
{blank_line_id} { BEGIN 0; }
{block_header_id} { j = 0; BEGIN blockprotocol_state; }
{tag_id} { BEGIN tagname_state; }

  /************************ The token definitions ****************************/

  /*** The block header data ***/

  /* The block protocol state.  Define the block protocol token */
<blockprotocol_state>[A-Z_]{8,15} {

  /* Copy the block protocol */  
  conf->blocks[i].protocol = get_plc_cnf_block_protocol(yytext); 

  if(conf->blocks[i].protocol == PDS_UNKNOWN_COMMS_PROTOCOL)
    return -1;

  /* Start block function state */
  BEGIN blockfunction_state;
}

  /* The block function state.  Define the block function token */
<blockfunction_state>[A-Z_]{4,6} {

  /* Copy the block function */  
  conf->blocks[i].function = get_plc_cnf_block_function(yytext); 

  /* Map the function to a data type for tags in this block */  
  conf->blocks[i].type = PDS_FUNC_TYPE_MAP(conf->blocks[i].function);

  if(conf->blocks[i].function == PDS_UNKNOWN_FUNCTION)
    return -1;

  /* Start block routing path state */
  BEGIN blockpath_state;
}

  /* The block routing path state.  Define the block routing path token */
<blockpath_state>[0-9\.]{1,15} {

  /* Copy the block routing path (string representation) */  
  strcpy(conf->blocks[i].path, yytext); 

  /* Set the comms method dependent on the PLC comms protocol */
  switch(conf->blocks[i].protocol)
  {
    case MB_TCPIP :               /* Comms via network */
    case MB_SERIAL_TCPIP :
    case DH_SERIAL_TCPIP :
    case CIP_TCPIP :
      /* Start block IP address state */
      BEGIN blockip_addr_state;
    break;

    case MB_SERIAL :              /* Comms via serial port */
    case DH_SERIAL :
      /* Start block TTY device state */
      BEGIN blocktty_dev_state;
    break;
  }
}

  /* The block IP address state.  Define the block IP address token */
<blockip_addr_state>[0-9\.]{7,19} {

  /* Copy the block IP address (string representation) */  
  strcpy(conf->blocks[i].ip_addr, yytext); 

  /* Zero init. the block TTY device */  
  conf->blocks[i].tty_dev[0] = '\0'; 

  /* Start block TCP port state */
  BEGIN blockport_state;
}

  /* The block TCP port state.  Define the block TCP port token */
<blockport_state>[0-9]{1,5} {

  /* Copy the port (if no port is configured, use the default) */  
  if(yytext)
    conf->blocks[i].port = (unsigned short int) atoi(yytext); 
  else
    conf->blocks[i].port = (unsigned short int) PLC_CNF_DEFAULT_PORT;

  /* Set the addressing scheme dependent on the PLC comms protocol */
  switch(conf->blocks[i].protocol)
  {
    case MB_TCPIP :
    case MB_SERIAL_TCPIP :
      /* Start block base address state */
      BEGIN blockbase_addr_state;
    break;

    case DH_SERIAL_TCPIP :
    case CIP_TCPIP :
      /* Start block ascii address state */
      BEGIN blockascii_addr_state;
    break;
  }
}

  /* The block TTY device state.  Define the block TTY device token */
<blocktty_dev_state>\"[a-zA-Z0-9/]{4,80}["\n] {

  /* Copy the block TTY device (remove quotes & terminate) */  
  strncpy(conf->blocks[i].tty_dev, &yytext[1], (yyleng - 2)); 
  conf->blocks[i].tty_dev[yyleng-1] = '\0';

  /* Zero init. the block IP address & TCP port */  
  conf->blocks[i].ip_addr[0] = '\0';
  conf->blocks[i].port = 0;

  /* Set the addressing scheme dependent on the PLC comms protocol */
  switch(conf->blocks[i].protocol)
  {
    case MB_SERIAL :
      /* Start block base address state */
      BEGIN blockbase_addr_state;
    break;

    case DH_SERIAL :
      /* Start block ascii address state */
      BEGIN blockascii_addr_state;
    break;
  }
}

  /* The block base address state.  Define the block base address token */
<blockbase_addr_state>[0-13-4]{5,6} {

  /* Copy the block base address */  
  conf->blocks[i].base_addr = atoi(yytext); 

  /* Zero init. the block ascii address */  
  conf->blocks[i].ascii_addr[0] = '\0'; 

  /* Start block poll rate state */
  BEGIN blockpollrate_state;
}

  /* The block ascii address state.  Define the block ascii address token */
<blockascii_addr_state>[a-zA-Z0-9_:$\.\[\]]{1,51} {

  /* Copy the block ascii address */  
  strcpy(conf->blocks[i].ascii_addr, yytext);

  /* Zero init. the block base address */  
  conf->blocks[i].base_addr = 0; 

  /* Start block poll rate state */
  BEGIN blockpollrate_state;
}

  /* The block poll rate state.  Define the block poll rate token */
<blockpollrate_state>[0-9]+ {

  /* Copy the block poll rate */  
  conf->blocks[i].pollrate = atoi(yytext); 

  /* We have all this block's data now.  Check if this block's PLC is already
     in the array of PLCs for this configuration.  If not, then add it */
  configure_plcs(i, conf);

  /* This is the last block header data item so increment the array counter */
  i++; 

  conf->nblocks = i;              /* The file's blocks counter */

  /* Start zero state */
  BEGIN 0;
}

  /*** The tag data ***/

  /* N.B.:  The blocks array counter is (i - 1) because the block's data */
  /*        has been read in and it's counter incremented.  */

  /* The tag name state.  Define a tag name token */
<tagname_state>[a-zA-Z0-9_:\[\]]{1,64} { 

  if((i - 1) < 0)                 /* Ensure a block header has been read */
  {
    err(errout, "cannot access block header with index < 0!\n");
    return -1;
  }

  /* Copy the tag name */  
  strcpy(conf->blocks[i-1].tags[j].name, yytext); 

  /* Set the addressing scheme dependent on the PLC comms protocol */
  switch(conf->blocks[i-1].protocol)
  {
    case MB_TCPIP :
    case MB_SERIAL_TCPIP :
    case MB_SERIAL :
    case DH_SERIAL_TCPIP :
    case DH_SERIAL :
    case CIP_TCPIP :
      /* Start tag reference state */
      BEGIN tagref_state;
    break;

    default :
      /* Start tag ascii reference state */
      BEGIN tagascii_ref_state;
    break;
  }
}

  /* The tag reference state.  Define a tag reference token */
<tagref_state>[0-9]{1,6} { 

  /* Copy the tag reference */  
  conf->blocks[i-1].tags[j].ref = atoi(yytext); 

  /* Zero init. the tag ascii reference */  
  conf->blocks[i-1].tags[j].ascii_ref[0] = '\0';

  /* Start tag bit state */
  BEGIN tagbit_state;
}

  /* The tag ascii reference state.  Define a tag ascii reference token */
<tagascii_ref_state>[a-zA-Z0-9:$]{1,51} {

  /* Copy the tag ascii reference */  
  strcpy(conf->blocks[i-1].tags[j].ascii_ref, yytext);

  /* Zero init. the tag reference */  
  conf->blocks[i-1].tags[j].ref = 0;

  /* Start tag bit state */
  BEGIN tagbit_state;
}

  /* The tag bit state.  Define a tag bit token */
<tagbit_state>[0-9]{1,2} { 

  /* Copy the tag bit */  
  conf->blocks[i-1].tags[j].bit = atoi(yytext); 

  /* This is the last tag data item so increment the array counter */
  j++;

  conf->blocks[i-1].ntags = j;    /* The block's tags counter */
  conf->ndata_tags++;             /* The file's data tags counter */

  /* Start zero state */
  BEGIN 0;
}

  /*** Default action for undefined tokens ***/
. ;                                         /* Do nothing with any 'loose' */
[\n] ;                                      /* chars or empty lines */

%%

/******************************************************************************
* Function to read a PLC configuration file                                   *
*                                                                             *
* Pre-condition:  The successfully opened config file handle is passed to     *
*                 the function                                                *
* Post-condition: Each configured tag is read into a structure.  On success   *
*                 the struct is returned, on error a null is returned         *
******************************************************************************/
plc_cnf* read_plc_cnf_file(FILE *handle)
{
  plc_cnf *conf = NULL;

  if((conf = malloc(sizeof(plc_cnf))))
  {
    memset(conf, 0, sizeof(plc_cnf));

    yyin = handle;                     /* Set file to be lexer input */

    if(yylex(conf) != -1)              /* Call the lexer */
    {
      /* Set this file's total tags (data + status) */
      conf->ttags = (conf->ndata_tags + conf->nstatus_tags);
    }
    else
    {
      err(errout, "error parsing PLC config file\n");
      if(conf) free(conf);
      conf = (plc_cnf *) NULL;
    }
  }
  else
  {
    err(errout, "memory allocation error for PLC config file struct\n");
  }

  return conf;
}



/******************************************************************************
* Function to translate a block function string into an integer constant      *
*                                                                             *
* Pre-condition:  The block function string is passed to the function         *
* Post-condition: The block function is returned as an integer or 0 if the    *
*                 block function is unknown                                   *
******************************************************************************/
static int get_plc_cnf_block_function(char *str)
{
  int i = 0, nfuncs = 0, fcode = PDS_UNKNOWN_FUNCTION, found = 0;
  pds_lookup_item *func = NULL;

  /* Get the number of functions in the list & a pointer to the functions */
  nfuncs = __pds_func_list.nitems;
  func = __pds_func_list.items;

  /* Find the corresponding code for the block function string */
  for(i = 0; i < nfuncs && func->name; i++, func++)
  {
    if(strcmp(str, (char *) func->name) == 0)
    {
      fcode = func->value;
      found = 1;
      break;
    }
  }

  if(!found)
    err(errout, "unknown function '%s' in block header\n", str);

  return fcode;
}



/******************************************************************************
* Function to translate a block protocol string into an integer constant      *
*                                                                             *
* Pre-condition:  The block protocol string is passed to the function         *
* Post-condition: The block protocol is returned as an integer or 0 if the    *
*                 block protocol is unknown                                   *
******************************************************************************/
static int get_plc_cnf_block_protocol(char *str)
{
  int i = 0, nprotos = 0, pcode = PDS_UNKNOWN_COMMS_PROTOCOL, found = 0;
  pds_lookup_item *proto = NULL;

  /* Get the number of protocols in the list & a pointer to the protocols */
  nprotos = __pds_proto_list.nitems;
  proto = __pds_proto_list.items;

  /* Find the corresponding code for the block protocol string */
  for(i = 0; i < nprotos && proto->name; i++, proto++)
  {
    if(strcmp(str, (char *) proto->name) == 0)
    {
      pcode = proto->value;
      found = 1;
      break;
    }
  }

  if(!found)
    err(errout, "unknown protocol '%s' in block header\n", str);

  return pcode;
}



/******************************************************************************
* Function to add, if not already present, this block's PLC to the PLC array  *
*                                                                             *
* Pre-condition:  The block's array index and the configuration struct are    *
*                 passed to the function                                      *
* Post-condition: If not already present, the block's PLC is added to the PLC *
*                 array                                                       *
******************************************************************************/
static int configure_plcs(int block, plc_cnf *conf) 
{
  register int i = 0;
  int found = 0;

  /* Cycle through PLCs that are already configured and check if this block's
     PLC is present */
  for(i = 0; i < conf->nplcs; i++)
  {
    switch(conf->blocks[block].protocol)
    {
      case MB_TCPIP :             /* Comms via network */
      case MB_SERIAL_TCPIP :
      case DH_SERIAL_TCPIP :
      case CIP_TCPIP :
        /* If IP address, port & path match, this PLC is already in list */
        if((strcmp(conf->blocks[block].ip_addr, conf->plcs[i].ip_addr) == 0) &&
           (conf->blocks[block].port == conf->plcs[i].port) &&
           (strcmp(conf->blocks[block].path, conf->plcs[i].path) == 0))
        {
          found = 1;
        }
      break;

      case MB_SERIAL :            /* Comms via serial port */
      case DH_SERIAL :
        /* If TTY device & path match, this PLC is already in list */
        if((strcmp(conf->blocks[block].tty_dev, conf->plcs[i].tty_dev) == 0) &&
           (strcmp(conf->blocks[block].path, conf->plcs[i].path) == 0))
        {
          found = 1;
        }
      break;
    }

    if(found)
      break;
  }

  if(!found)
  {
    /* Append this PLC's data to the PLC array */
    strcpy(conf->plcs[conf->nplcs].ip_addr, conf->blocks[block].ip_addr);
    conf->plcs[conf->nplcs].port = conf->blocks[block].port;
    strcpy(conf->plcs[conf->nplcs].tty_dev, conf->blocks[block].tty_dev);
    strcpy(conf->plcs[conf->nplcs].path, conf->blocks[block].path);
    conf->plcs[conf->nplcs].protocol = conf->blocks[block].protocol;

    conf->nplcs++;                     /* The file's PLCs counter */
    conf->nstatus_tags = conf->nplcs;  /* Set this file's status tags */
  }

  return conf->nplcs;
}

