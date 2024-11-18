#pragma once

#include <filesystem>
#include <fstream>

namespace std {

    using InFile = ifstream;

    using OutFile = ofstream;

    using File = fstream; 

    using FilePath = filesystem::path;

}