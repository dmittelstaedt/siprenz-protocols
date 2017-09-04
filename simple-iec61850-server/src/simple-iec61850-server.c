/**
 * Simple IEC 61850 Server. This C-File implements a simple IEC 61850 client. The library libIEC61850
 * is used for implementing the protocol IEC 61850. Argp is used to parse
 * the options and arguments from the command line.
 * @file simple-iec61850-server.c
 * @author David Mittelstädt
 * @see https://github.com/dmittelstaedt/siprenz-protocols
 * @see http://libiec61850.com/libiec61850/
 */

#include "iec61850_server.h"
#include "hal_thread.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "argp.h"

#include "static_model.h"

/**
* Global variable for the version.
* This variable is used from argp to set the output
* for the version option.
*/
const char *argp_program_version = "Simple IEC 61850 Server version 1.0.0";

/* import IEC 61850 device model created from SCL-File */
extern IedModel iedModel;

/**
* Global variable for state of server
*/
static int running = 0;

/**
* Global variable for tcp port
*/
int tcpPort = 10102;

/**
* Global variable for the
* the default value of the power
*/
float power = 500.f;

/**
* Global variable for verbose output
*/
int isVerbose = 0;

/**
* Function to implement signal handler for abort
*/
void sigint_handler(int signalId)
{
     running = 0;
     printf("Server stopped!.\n");
}

/**
* Function for running the server
*/
void runServer() {
     IedServer iedServer = IedServer_create(&iedModel);

     /* MMS server will be instructed to start listening to client connections. */
     IedServer_start(iedServer, tcpPort);

     if (!IedServer_isRunning(iedServer)) {
          printf("Starting server failed! Exit.\n");
          IedServer_destroy(iedServer);
          exit(-1);
     } else {
          printf("Server started successfully on port %d.\n", tcpPort);
          printf("Total active power (TotW.mag) at start: %.1f\n", power);
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
               printf("Setting TotW.mag to: %.1f\n", power);
          }

          power += 0.1f;

          Thread_sleep(1000);
     }

     /* stop MMS server - close TCP server socket and all client sockets */
     IedServer_stop(iedServer);

     /* Cleanup - free all resources */
     IedServer_destroy(iedServer);
}

/**
* Function which parses the given options and arguments.
* Function to parse the given options and arguments with the
* help of argp.
* @param key Key for identifying
* @param arg Value of the key
* @param state Struct
* @return Return code of parsing the arguments
*/
static int parse_opt (int key, char *arg, struct argp_state *state)
{
     switch (key) {
          case 'p': {
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

/**
* Main function.
* Starts the parsing function and then starts the
* server
* @param argc Number of arguments
* @param argv Content of the arguments
* @return Exit status of the application
*/
int main(int argc, char** argv) {

     struct argp_option options[] =
     {
          { "port", 'p', "PORT", 0, "TCP port for the server"},
          { "power", 'w', "POWER", 0, "Start value of the power for the server"},
          { "verbose", 'v', 0, 0, "Verbose Output"},
          { 0 }
     };
     struct argp argp = { options, parse_opt };
     argp_parse (&argp, argc, argv, 0, 0, 0);
     runServer();

     return 0;

} /* main() */
