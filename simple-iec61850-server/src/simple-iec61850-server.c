#include "iec61850_server.h"
#include "hal_thread.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "argp.h"

#include "static_model.h"

/* import IEC 61850 device model created from SCL-File */
extern IedModel iedModel;

static int running = 0;
static int tcpPort = 10102;
static float power = 500.f;
static int isVerbose = 0;

void sigint_handler(int signalId)
{
     running = 0;
     printf("Server stopped!.\n");
}

void runServer() {
     IedServer iedServer = IedServer_create(&iedModel);

     // set initial measurement and status values from process

     /* MMS server will be instructed to start listening to client connections. */
     IedServer_start(iedServer, tcpPort);

     if (!IedServer_isRunning(iedServer)) {
          printf("Starting server failed! Exit.\n");
          IedServer_destroy(iedServer);
          exit(-1);
     } else {
          printf("Server started successfully on port %d.\n", tcpPort);
          printf("Power at start: %.1f\n", power);
          if (isVerbose == 1) {
               printf("Running in verbose mode.\n");
          }
     }

     running = 1;

     signal(SIGINT, sigint_handler);

     while (running) {
          uint64_t timeval = Hal_getTimeInMs();

          IedServer_lockDataModel(iedServer);
          IedServer_updateUTCTimeAttributeValue(iedServer, IEDMODEL_Inverter_MMXU1_TotW_t, timeval);
          IedServer_updateFloatAttributeValue(iedServer, IEDMODEL_Inverter_MMXU1_TotW_mag_f, power);

          IedServer_unlockDataModel(iedServer);

          if (isVerbose == 1) {
               printf("Setting power to: %.1f\n", power);
          }

          power += 0.1f;

          Thread_sleep(1000);
     }

     /* stop MMS server - close TCP server socket and all client sockets */
     IedServer_stop(iedServer);

     /* Cleanup - free all resources */
     IedServer_destroy(iedServer);
}

static int parse_opt (int key, char *arg, struct argp_state *state)
{
     switch (key) {
          case 't': {
               tcpPort = atoi(arg);
               break;
          }
          case 'w': {
               power = atof(arg);
               break;
          }
          case 'v': {
               isVerbose = 1;
               break;
          }
     }
     return 0;
}

int main(int argc, char** argv) {

     struct argp_option options[] =
     {
          { 0, 'p', "PORT", 0, "TCP port for the server"},
          { 0, 'w', "POWER", 0, "Power value for the server"},
          { 0, 'v', 0, 0, "Verbose Output"},
          { 0 }
     };
     struct argp argp = { options, parse_opt };
     argp_parse (&argp, argc, argv, 0, 0, 0);
     runServer();

     return 0;

} /* main() */
