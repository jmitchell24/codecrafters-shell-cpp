//
// Created by james on 02/04/25.
//

#pragma once

//
// std
//
#include <string>
#include <vector>

namespace sh
{
    using strlist_type = std::vector<std::string>;

    strlist_type getEnvPaths();
    strlist_type getEnvExes();
    bool fileExists(std::string const& path, std::string const& file);
    strlist_type getPathFiles(std::string const& path);
}