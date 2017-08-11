#include "iec61850_client.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {

     char* hostname;
     int tcpPort = 10102;

     if (argc > 1)
     hostname = argv[1];
     else
     hostname = "127.0.0.1";

     if (argc > 2)
     tcpPort = atoi(argv[2]);

     IedClientError error;

     IedConnection con = IedConnection_create();

     printf("Starting client...\n");

     // At this line ns-3 stops the execution without an error message, when the server isn't running
     IedConnection_connect(con, &error, hostname, tcpPort);

     if (error == IED_ERROR_OK) {
          printf("Client started successfully. Client is connected to %s on port %d.\n", hostname, tcpPort);

          MmsValue* value = IedConnection_readObject(con, &error, "ied1Inverter/MMXU1.TotW.mag.f", IEC61850_FC_MX);

          if (value != NULL) {
               float fval = MmsValue_toFloat(value);
               printf("Read power value: %.1f\n", fval);
               MmsValue_delete(value);
          }

          IedConnection_close(con);
     }
     else {
          printf("Connection failed!\n");
     }

     IedConnection_destroy(con);

     printf("Client stopped!\n");

}
