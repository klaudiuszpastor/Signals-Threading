#pragma once

#include "signals.h"
#include "cJSON.h"

// Funkcje do zapisu danych
void *write_output(void *filename);
void write_connection_request(cJSON *json);
void write_downlink_data(cJSON *json);
