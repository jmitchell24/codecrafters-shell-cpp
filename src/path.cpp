//
// Created by james on 02/04/25.
//

//
// sh
//
#include "path.hpp"
using namespace sh;

//
// std
//
#include <sstream>
#include <filesystem>
using namespace std;
namespace fs = std::filesystem;


//
// unix
//
#include <dirent.h>

strlist_type sh::getPathFiles(string const& path)
{
    strlist_type files;
    for (auto&& it : fs::directory_iterator(path))
        if (it.is_regular_file())
            files.push_back(it.path().filename().string());
    return files;
}


bool sh::fileExists(string const& path, string const& file)
{
    for (auto&& it: fs::directory_iterator(path))
        if (it.is_regular_file() && it.path().filename().compare(file) == 0)
            return true;
    return false;
}

strlist_type sh::getEnvPaths()
{
    auto path_stream = istringstream{getenv("PATH")};

    string it;
    strlist_type paths;
    while (getline(path_stream, it, ':'))
        paths.push_back(it);
    return paths;
}

strlist_type sh::getEnvExes()
{
    strlist_type files;
    for (auto&& it: getEnvPaths())
        for (auto&& jt: getPathFiles(it))
            files.push_back(jt);
    return files;
}


