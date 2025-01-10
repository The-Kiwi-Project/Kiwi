#include "./connection.hh"

namespace kiwi::circuit {

    Connection::Connection(int sync, Pin input, Pin output) :
        _sync{sync},
        _input{input},
        _output{output}
    {
    }

    

}