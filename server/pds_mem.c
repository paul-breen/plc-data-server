/******************************************************************************
* PROJECT:  PLC data server                                                   * 
* MODULE:   pds_mem.c                                                         *
* PURPOSE:  The memory management module                                      *
* AUTHOR:   Paul M. Breen                                                     *
* DATE:     1999-09-22                                                        *
******************************************************************************/

#include "pds_srv.h"

extern int quit_flag;             /* Declared in the main file */
extern int errout;                /* Declared in the main file */
extern int dbgflag;               /* Declared in the main file */

/******************************************************************************
* Globals                                                                     *
******************************************************************************/
pdstag **block_index = NULL;

/******************************************************************************
* Function to setup a shared memory segment                                   *
*                                                                             *
* Pre-condition:  The shared memory key, size (in bytes), flags and an int to *
*                 hold the id are passed to the function                      *
* Post-condition: The shared memory segment is created and 'this' process is  *
*                 attached to it.  A pointer to the 'zero initialised' memory *
*                 is returned or a null pointer if an error occurs            *
******************************************************************************/
void* setup_shm(key_t shmkey, size_t shmsize, int shmflags, int *shmid)
{
  void *shm = (void *) 0;

  /* Create a shared memory segment */
  if((*shmid = shmget(shmkey, shmsize, shmflags)) == -1)
  {
    return (void *) NULL;
  }

  /* Attach the calling process to the shared memory */
  if((shm = shmat(*shmid, (void *) 0, 0)) == (void *) -1)
  {
    return (void *) NULL;
  }

  memset(shm, 0, shmsize);

  return shm;
}



/******************************************************************************
* Function to release a shared memory segment                                 *
*                                                                             *
* Pre-condition:  A valid shared memory pointer and valid shared memory id    *
*                 are passed to the function                                  *
* Post-condition: The shared memory segment is released (detached and freed). *
*                 If an error occurred a -1 is returned                       *
******************************************************************************/
int release_shm(void *shm, int shmid)
{
  /* Detach the calling process from the shared memory */
  if(shmdt(shm) == -1)
  {
    return -1;
  }

  /* Delete the shared memory segment */
  if(shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0) == -1)
  {
    return -1;
  }

  return 0;
}



/******************************************************************************
* Function to map PLC addresses to memory variable tags in shared memory      *
*                                                                             *
* Pre-condition:  The PLC configuration struct and a valid connection struct  *
*                 containing a shared memory pointer are passed to the        *
*                 function                                                    *
* Post-condition: The tags are created as memory variables (in shared memory) *
*                 and map directly to PLC addresses.  The total count of tags *
*                 is returned or -1 if an error occurs                        *
******************************************************************************/
int map_shm(plc_cnf *conf, pdsconn *conn)
{
  register int i = 0, j = 0, tag_count = 0;
  pdstag *p = NULL;

  if(!(block_index = (pdstag **) calloc(conf->nblocks, sizeof(pdstag *))))
  {
    err(errout, "%s: error allocating memory for block index\n", PROGNAME);
    return -1;
  }
 
  p = (pdstag *) conn->shm;
 
  for(i = 0; i < conf->nblocks; i++)
  {
    block_index[i] = p;           /* Add this block's 1st tag to index */

    /* Map configuration file data tags to memory structure tags */
    for(j = 0; j < conf->blocks[i].ntags; j++, p++) 
    {
      p->id = tag_count++;
      p->protocol = conf->blocks[i].protocol;
      p->function = conf->blocks[i].function;
      p->block_id = i;
      p->base_addr = conf->blocks[i].base_addr;
      strcpy(p->ascii_addr, conf->blocks[i].ascii_addr);
      strcpy(p->ip_addr, conf->blocks[i].ip_addr);
      p->port = conf->blocks[i].port;
      strcpy(p->tty_dev, conf->blocks[i].tty_dev);
      strcpy(p->path, conf->blocks[i].path);
      p->ref = conf->blocks[i].tags[j].ref;
      strcpy(p->ascii_ref, conf->blocks[i].tags[j].ascii_ref);
      strcpy(p->name, conf->blocks[i].tags[j].name);
      p->value = 0;
      p->type = conf->blocks[i].type;
      p->status = 0;
      p->mtime = 0;
    }
  }

  for(i = 0; i < conf->nplcs; i++, p++)
  {
    /* Map configuration file PLC status tags to memory structure tags */
    p->id = tag_count++;
    p->protocol = conf->plcs[i].protocol;
    p->function = 0;
    p->block_id = (conf->nblocks + i);
    p->base_addr = 0;
    p->ascii_addr[0] = '\0';
    strcpy(p->ip_addr, conf->plcs[i].ip_addr);
    p->port = conf->plcs[i].port;
    strcpy(p->tty_dev, conf->plcs[i].tty_dev);
    strcpy(p->path, conf->plcs[i].path);
    p->ref = 0;
    p->ascii_ref[0] = '\0';
    sprintf(p->name, "%s%d", PDS_PLC_PREFIX, i);
    p->value = 0;
    p->type = 0;
    p->status = 0;
    p->mtime = 0;
  }
 
  return tag_count;
}



/******************************************************************************
* Function to set the value of a semaphore                                    *
*                                                                             *
* Pre-condition:  A valid semaphore ID, the value for the operation and the   *
*                 semaphore set array number are passed to the function       *
* Post-condition: The semaphore is set with the passed value.  If an error    *
*                 occurs a -1 is returned                                     *
******************************************************************************/
int semset(int id, int op, int snum)
{
  struct sembuf sb;

  sb.sem_num = snum;              /* Set semaphore No. in this semaphore set */
  sb.sem_op = op;                 /* Set the value for this operation */
  sb.sem_flg = SEM_UNDO;          /* Ensure 'rollback' if an error occurs */

  return semop(id, &sb, 1);
}



/******************************************************************************
* Function to map SPI tags to memory variable tags in shared memory           *
*                                                                             *
* Pre-condition:  The SPI tag list and a valid SPI connection struct          *
*                 containing a shared memory pointer are passed to the        *
*                 function                                                    *
* Post-condition: The tags are created as memory variables (in shared memory) *
*                 and map directly to SPI tags.  If an error occurs, a -1 is  *
*                 returned                                                    *
******************************************************************************/
int map_SPI_shm(pds_spi_tag_list *tag_list, pds_spi_conn *conn)
{
  memcpy(conn->shm, tag_list->tags, (sizeof(pds_spi_tag) * tag_list->ntags));

  return 0;
}

