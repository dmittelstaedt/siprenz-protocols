/**
 * Simple IEC 61850 Client. Thie Class represents the simple iec 61850 client
 * based on the library libIEC61850. 
 * @file simple-iec61850-client.c
 * @author David Mittelstädt
 * @see https://github.com/dmittelstaedt/siprenz-protocols
 * @see http://libiec61850.com/libiec61850/
 */

#include "iec61850_client.h"
#include "hal_thread.h"
#include <stdlib.h>
#include <stdio.h>
#include "argp.h"
#include <unistd.h>

/**
* Global variable for the version.
* This variable is used from argp to set the output
* for the version option.
*/
const char *argp_program_version = "Simple IEC 61850 Client version 1.0.0";

/**
* Global variable for the hostname.
* Details.
*/
char* host;

/**
* Global variable for the TCP Port.
* Details.
*/
int tcpPort = 10102;

/**
* Global variable for number of requests.
* Details.
*/
int count = -1;

/**
* Global variable for the time between sending requests.
* Details.
*/
int sleepInt = 1000;

/**
* Function which starts the client and checks the status.
* Details of the function.
*/
void runClient() {
     IedClientError error;

     IedConnection con = IedConnection_create();

     printf("Starting client...\n");

     // At this line ns-3 stops the execution without an error message, when the server isn't running
     IedConnection_connect(con, &error, host, tcpPort);

     if (error == IED_ERROR_OK) {
          printf("Client connected successfully to %s on port %d.\n", host, tcpPort);
          printf("Client starts reading total active power (TotW.mag) from Inverter of IED1.\n");

          int counter = 0;

          while (count == -1 || counter < count) {
               MmsValue* value = IedConnection_readObject(con, &error, "ied1Inverter/MMXU1.TotW.mag.f", IEC61850_FC_MX);

               // If an error occured during sending requests
               if (error != IED_ERROR_OK) {
                    printf("Lost connection to server %s.\n", host);
                    break;
               }

               if (value != NULL) {
                    float fval = MmsValue_toFloat(value);
                    printf("Reading TotW.mag: %.1f\n", fval);
                    MmsValue_delete(value);
               }
               Thread_sleep(sleepInt);
               counter++;
          }

          IedConnection_close(con);
     }
     else {
          printf("Connection failed!\n");
     }

     IedConnection_destroy(con);

     printf("Client stopped!\n");
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
static int parse_opt (int key, char *arg, struct argp_state *state) {
     int *arg_count = state->input;

     switch (key) {
          case 'p': {
               tcpPort = atoi(arg);
               break;
          }
          case 'c': {
               count = atoi(arg);
               break;
          }
          case 's': {
               sleepInt = atoi(arg);
               break;
          }
          case ARGP_KEY_ARG: {
               (*arg_count)--;
               if (*arg_count >= 0) {
                    host = arg;
               }
          }
          break;
          case ARGP_KEY_END: {
               if (*arg_count >= 1) {
                    argp_failure (state, 1, 0, "too few arguments");
               } else if (*arg_count < 0) {
                    argp_failure (state, 1, 0, "too many arguments");
               }
          }
          break;
     }
     return 0;
}

/**
* Main function.
* Starts the parsing function and then starts the
* server.
* @param argc Number of arguments
* @param argv Content of the arguments
* @return Exit status of the application
*/
int main(int argc, char** argv) {
     int arg_count = 1;

     struct argp_option options[] =
     {
          { "port", 'p', "PORT", 0, "TCP port for the server"},
          { "count", 'c', "NUMBER", 0, "Stop after sending NUMBER of requests"},
          { "sleep", 's', "TIME", 0, "Time between sending requests"},
          { 0 }
     };
     struct argp argp = { options, parse_opt, "host" };
     argp_parse (&argp, argc, argv, 0, 0, &arg_count);

     runClient();

     return 0;
}
