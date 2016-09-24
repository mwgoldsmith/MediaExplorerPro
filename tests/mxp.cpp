/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include <stdio.h>
#include "mxp.h"
#include <unistd.h>

int main (int argc, char *argv[]){
  printf("Creating new media explorer\n");
  mxp::IMediaExplorer* mxp = mxp_newInstance();

  printf("Initializing\n");
  mxp->Initialize("mxp.db", nullptr);

  printf("Discovering\n");
mxp->Discover("/home/michael/Videos");
 usleep(1000000*5);
  printf("Shutting down\n");
  mxp->Shutdown();

  return 0;
}