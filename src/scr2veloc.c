#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "mpi.h"

#include "kvtree.h"
#include "kvtree_util.h"
#include "kvtree_mpi.h"

#include "scr.h"
#include "veloc.h"

static char SCR2VELOC_NAME[]      = "scr.dataset";
static int  SCR2VELOC_MAX_VERSION = INT_MAX;
static char SCR2VELOC_VERSION[]   = "scr2veloc-v1.0";

static char*   scr2veloc_name_path = NULL;
static kvtree* scr2veloc_name_map  = NULL;

static MPI_Comm comm = MPI_COMM_NULL;
static int rank      = MPI_PROC_NULL;

static char current_name[1024];
static int  current_version = -1;

void set_name(int id, const char* name)
{
  kvtree* names = kvtree_set_kv_int(scr2veloc_name_map, "ID", id);
  kvtree_util_set_str(names, "NAME", name);
}

char* get_name(int id)
{
  char* name = NULL;
  const kvtree* names = kvtree_get_kv_int(scr2veloc_name_map, "ID", id);
  kvtree_util_get_str(names, "NAME", &name);
  return name;
}

int SCR_Init(void)
{
  // need to do some MPI communication, so dup comm_world
  MPI_Comm_dup(MPI_COMM_WORLD, &comm);

  // lookup our rank
  MPI_Comm_rank(comm, &rank);

  // this will record a map of veloc checkpoint ids to name strings
  scr2veloc_name_map = kvtree_new();

  // read in existing map if one exists
  const char* value = getenv("SCR2VELOC_NAMES");
  if (value != NULL) {
    // path to the map file
    scr2veloc_name_path = strdup(value);

    // read map on rank 0 and bcast
    if (rank == 0) {
      kvtree_read_file(scr2veloc_name_path, scr2veloc_name_map);
    }
    kvtree_bcast(scr2veloc_name_map, 0, comm);
  }

  // path to veloc config file
  const char* config = getenv("SCR2VELOC_CONFIG");

  // start up veloc
  int rc = VELOC_Init(comm, config);
  return (rc == VELOC_SUCCESS) ? SCR_SUCCESS : (!SCR_SUCCESS);
}

int SCR_Finalize(void)
{
  // free map and path to map file
  kvtree_delete(&scr2veloc_name_map);
  if (scr2veloc_name_path != NULL) {
    free(scr2veloc_name_path);
    scr2veloc_name_path = NULL;
  }

  // release our communicator
  MPI_Comm_free(&comm);

  // shut down veloc
  int cleanup = 0;
  int rc = VELOC_Finalize(cleanup);
  return (rc == VELOC_SUCCESS) ? SCR_SUCCESS : (!SCR_SUCCESS);
}

int SCR_Route_file(const char* name, char* file)
{
  // TODO: check that VELOC_MAX_NAME doesn't overflow SCR_MAX_FILENAME
  char routed[VELOC_MAX_NAME];
  int rc = VELOC_Route_file(name, routed);
  strncpy(file, routed, SCR_MAX_FILENAME);
  return (rc == VELOC_SUCCESS) ? SCR_SUCCESS : (!SCR_SUCCESS);
}

int SCR_Have_restart(int* flag, char* name)
{
  // TODO: need to record map from veloc id to name user wanted
  int rc = VELOC_Restart_test(SCR2VELOC_NAME, SCR2VELOC_MAX_VERSION);
  if (rc == VELOC_FAILURE) {
    // nothing found
    *flag = 0;
  } else {
    // got one, latest version number is in rc
    *flag = 1;
    current_version = rc;

    // given id, lookup name user expects from map
    const char* value = get_name(current_version);

    // return name to caller
    snprintf(name, SCR_MAX_FILENAME, value);
  }
  return SCR_SUCCESS;
}

int SCR_Start_restart(char* name)
{
  // check that name matches name in map corresponding to current id
  const char* value = get_name(current_version);
  if (strcmp(value, name) != 0) {
    printf("ERROR: %s does not match expected name for checkpoint id %d (%s)\n", name, current_version, value);
    return (! SCR_SUCCESS);
  }

  int rc = VELOC_Restart_begin(SCR2VELOC_NAME, current_version);
  return (rc == VELOC_SUCCESS) ? SCR_SUCCESS : (!SCR_SUCCESS);
}

int SCR_Complete_restart(int valid)
{
  int rc = VELOC_Restart_end(valid);
  return (rc == VELOC_SUCCESS) ? SCR_SUCCESS : (!SCR_SUCCESS);
}

int SCR_Need_checkpoint(int* flag)
{
  // TODO: no way to determine this from veloc anymore?
  // just assume always true for now
  *flag = 1;
  return SCR_SUCCESS;
}

int SCR_Start_checkpoint(void)
{
  // increment our checkpoint counter
  current_version++;

  // didn't get a name from user, so make one up
  char ckptname[VELOC_MAX_NAME];
  snprintf(ckptname, VELOC_MAX_NAME, "%s.%d", SCR2VELOC_NAME, current_version);

  // record id-to-name entry in map file, write file
  set_name(current_version, ckptname);
  if (rank == 0) {
    kvtree_write_file(scr2veloc_name_path, scr2veloc_name_map);
  }

  int rc = VELOC_Checkpoint_begin(SCR2VELOC_NAME, current_version);
  return (rc == VELOC_SUCCESS) ? SCR_SUCCESS : (!SCR_SUCCESS);
}

int SCR_Complete_checkpoint(int valid)
{
  int rc = VELOC_Checkpoint_end(valid);
  return (rc == VELOC_SUCCESS) ? SCR_SUCCESS : (!SCR_SUCCESS);
}

int SCR_Start_output(const char* name, int flags)
{
  if (flags & SCR_FLAG_CHECKPOINT) {
    // increment our checkpoint counter
    current_version++;

    // record id-to-name entry in map file, write file
    set_name(current_version, name);
    if (rank == 0) {
      kvtree_write_file(scr2veloc_name_path, scr2veloc_name_map);
    }

    int rc = VELOC_Checkpoint_begin(SCR2VELOC_NAME, current_version);
    return (rc == VELOC_SUCCESS) ? SCR_SUCCESS : (!SCR_SUCCESS);
  } else {
    // TODO: can't support pure output in veloc
    return (! SCR_SUCCESS);
  }
}

int SCR_Complete_output(int valid)
{
  int rc = VELOC_Checkpoint_end(valid);
  return (rc == VELOC_SUCCESS) ? SCR_SUCCESS : (!SCR_SUCCESS);
}

char* SCR_Get_version(void)
{
  return SCR2VELOC_VERSION;
}

int SCR_Should_exit(int* flag)
{
  // TODO: no way to know in veloc?
  *flag = 0;
  return SCR_SUCCESS;
}
