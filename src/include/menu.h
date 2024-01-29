#ifndef MESSAGES_H
#define MESSAGES_H
#include "gl_instance.h"
#include "button.h"
#include <string>
#include <vector>

class menu
{
public:
    menu(gl_instance &gl, const rect &space, const std::vector<std::string> &options);
    
    // returns size() on escape pressed
    std::size_t run(gl_instance &gl) const;
    std::size_t size() const { return m_options.size(); }
private:
    std::vector<button> m_options;
};

bool yes_no(gl_instance &gl, const std::string &message);
void message(gl_instance &gl, const std::string &message);
#endif