#pragma once

#include <iostream>

size_t write_varlength(std::ostream& out, size_t val);
size_t read_varlength(std::istream& in);

