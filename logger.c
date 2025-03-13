#include <stdio.h>

#include "logger.h"

void Logger_log( char* msg ) {
  printf("INFO: %s\n", msg);
}

void Logger_error( char* msg ) {
  char buff[256];
  sprintf(buff, "ERROR: %s\n", msg);
  perror(buff);
}
