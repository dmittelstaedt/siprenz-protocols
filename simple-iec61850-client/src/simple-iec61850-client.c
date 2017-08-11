#include "iec61850_client.h"
#include <stdlib.h>
#include <stdio.h>
#include "argp.h"
#include <unistd.h>

static char* host;
static int tcpPort = 10102;

void runClient() {
     IedClientError error;

     IedConnection con = IedConnection_create();

     printf("Starting client...\n");

     // At this line ns-3 stops the execution without an error message, when the server isn't running
     IedConnection_connect(con, &error, host, tcpPort);

     if (error == IED_ERROR_OK) {
          printf("Client started successfully. Client is connected to %s on port %d.\n", host, tcpPort);

          // while (true) {
               MmsValue* value = IedConnection_readObject(con, &error, "ied1Inverter/MMXU1.TotW.mag.f", IEC61850_FC_MX);

               if (value != NULL) {
                    float fval = MmsValue_toFloat(value);
                    printf("Read power value: %.1f\n", fval);
                    MmsValue_delete(value);
               }
               sleep(1);
          // }

          IedConnection_close(con);
     }
     else {
          printf("Connection failed!\n");
     }

     IedConnection_destroy(con);

     printf("Client stopped!\n");
}

static int parse_opt (int key, char *arg, struct argp_state *state) {
     int *arg_count = state->input;

     switch (key) {
          case 'p': {
               tcpPort = atoi(arg);
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

int main(int argc, char** argv) {
     int arg_count = 1;

     struct argp_option options[] =
     {
          { "port", 'p', "PORT", 0, "TCP port for the server"},
          { 0 }
     };
     struct argp argp = { options, parse_opt, "host" };
     argp_parse (&argp, argc, argv, 0, 0, &arg_count);

     printf("Host: %s\n", host);
     printf("Port: %d\n", tcpPort);

     runClient();

     return 0;
}
