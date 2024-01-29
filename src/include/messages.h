#ifndef MESSAGES_H
#define MESSAGES_H
#include "gl_instance.h"
#include <string>

bool yes_no(gl_instance &gl, const std::string &message);
void message(gl_instance &gl, const std::string &message);
#endif