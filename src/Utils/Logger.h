#pragma once

#include <string>
#include <iostream>

namespace Luntik::Utils {
    class Logger {
    public:
        Logger(const std::string& name) :
            m_Name(name) {}

        void log(const std::string& message) {
#define LOG 1
#if LOG == 1
            std::cout << "[" << m_Name << "] " << message << "\n";
#endif
        }
    private:
        std::string m_Name;
    };
}