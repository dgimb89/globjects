#pragma once

#include <set>

namespace globjects
{
class File;

class FileRegistry
{
public:
    static void registerFile(File * file);
    static void deregisterFile(File * file);

    static void reloadAll();
protected:
    FileRegistry();
    virtual ~FileRegistry();

    std::set<File*> m_registeredFiles;
    static FileRegistry* s_instance;
};

} // namespace globjects
