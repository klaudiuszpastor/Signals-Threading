#pragma once

#include "signals.h"

// Funkcje do odczytu i walidacji danych
void *read_input(void *filename);
int read_line(char *line, int *values, int max_values);
int validate_connection_request(int *values, int count);
int validate_downlink_data(int *values, int count);
void write_connection_request_to_memory(int *values);
void write_downlink_data_to_memory(int *values);

