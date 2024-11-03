#pragma once 

#include <exception>
#include <stdexcept>

namespace std {

    using Exception = exception;

    using RunTimeError = runtime_error;

    using LogicError = logic_error;

}
