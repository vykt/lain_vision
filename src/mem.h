#ifndef MEM_H
#define MEM_H

#include <string>
#include <optional>

#include <cstdio>

#include <unistd.h>

#include <libpwu.h>


class mem {

    //attributes
    private:
        //main libpwu structure
        maps_data m_data;
        
        //procfs handles
        int fd_mem;
        FILE * stream_maps;

    //methods
    private:
        std::optional<std::string> get_pid(pid_t * cs2_pid);
        std::optional<std::string> get_mem_handles(pid_t cs2_pid);

    public:
        //init & fini
        std::optional<std::string> init();
        std::optional<std::string> fini();

        //important
        template <typename T> std::optional<std::string> read_addr(uintptr_t addr, 
                                                                   T * buf);
        template <typename T> std::optional<std::string> read_array_addr(uintptr_t addr,
                                                                         T * buf,
                                                                         int len);
        std::optional<std::string> get_backing_file_addr(const char * backing_file,
                                                         uintptr_t * addr);
};

#endif
