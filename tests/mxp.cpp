/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include <stdio.h>
#include "mxp.h"
int main (int argc, char *argv[]){
  printf("Creating new media explorer\n");
  mxp::IMediaExplorer* mxp = mxp_newInstance();

  printf("Initializing\n");
  mxp->Initialize("mxp.db", nullptr);

  printf("Discovering\n");
mxp->Discover("C:\\Users\\mgoldsmith\\Videos\\");
// _sleep(1000000*5);
  printf("Shutting down\n");
  mxp->Shutdown();

  return 0;
}