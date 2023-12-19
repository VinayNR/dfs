#include "utils.h"

void deleteAndNullifyPointer(char *& ptr, bool isArray) {
    isArray ? delete[] ptr : delete ptr;
    ptr = nullptr;
}