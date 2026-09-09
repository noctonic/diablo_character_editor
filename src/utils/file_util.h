// Derived from DevilutionX's Source/utils/file_util.h
// License: https://github.com/diasurgical/devilutionX/blob/master/LICENSE.md

#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <string_view>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

namespace devilution {

inline bool FileExists(const char *path)
{
    struct stat st;
    return stat(path, &st) == 0;
}

inline bool GetFileSize(const char *path, std::uintmax_t *size)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return false;
    if (size != nullptr)
        *size = st.st_size;
    return true;
}

inline bool ResizeFile(const char *path, std::uintmax_t size)
{
#ifdef _WIN32
    FILE *file = std::fopen(path, "rb+");
    if (file == nullptr)
        return false;
    int result = _chsize_s(_fileno(file), size);
    std::fclose(file);
    return result == 0;
#else
    return truncate(path, size) == 0;
#endif
}

inline std::string_view Dirname(const char *path)
{
    const char *slash = std::strrchr(path, '/');
#ifdef _WIN32
    const char *backslash = std::strrchr(path, '\\');
    if (backslash != nullptr && (slash == nullptr || backslash > slash))
        slash = backslash;
#endif
    if (slash == nullptr)
        return {};
    return std::string_view(path, slash - path);
}

inline bool RecursivelyCreateDir(const char *path)
{
    if (path == nullptr || *path == '\0')
        return true;
    struct stat st;
    if (stat(path, &st) == 0) {
#ifdef _WIN32
        return (st.st_mode & _S_IFMT) == _S_IFDIR;
#else
        return S_ISDIR(st.st_mode);
#endif
    }
    std::string_view parent = Dirname(path);
    if (!parent.empty()) {
        std::string parentStr(parent);
        if (!RecursivelyCreateDir(parentStr.c_str()))
            return false;
    }
#ifdef _WIN32
    return _mkdir(path) == 0;
#else
    return mkdir(path, 0777) == 0;
#endif
}

} // namespace devilution
