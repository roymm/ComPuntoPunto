#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

/**
 * Imprime un mensaje de error y luego sale, devolviendo my_error al proceso padre
 * @param my_error Un número de error (puede ser la variable errno)
 * @param message Un mensaje de error que puede servir para identificar en dónde se dió el error
 */
void error_exit(int my_error, const char * message);
#endif 
