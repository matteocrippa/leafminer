#ifndef STORAGE_H
#define STORAGE_H
#include "model/configuration.h"
void storage_setup();
void storage_save(const Configuration &conf);
void storage_load(Configuration *conf);
#endif // STORAGE_H