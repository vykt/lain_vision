#include <string>
#include <optional>

#include <cstdio>

#include <unistd.h>

#include <libpwu.h>

#include "mem.h"


/*
 *  private
 */

//retrieve process ID of cs2
std::optional<std::string> mem::get_pid(pid_t * cs2_pid) {

    int ret;
    name_pid n_pid;
    
    ret = new_name_pid(&n_pid, (char *) "cs2");
    if (ret) {
        return "[mem::get_pid()] new_name_pid() returned -1";
    }

    ret = pid_by_name(&n_pid, cs2_pid);
    switch (ret) {
        case -1:
            return "[mem::get_pid()] pid_by_name() "
                   "returned -1";
        case 0:
            return "[mem::get_pid()] pid_by_name() "
                   "returned 0 | is cs2 running?";
        case 1:
            break;
        default:
            return "[mem::get_pid()] pid_by_name() "
                   "returned > 1 | are many instances of cs2 running?";
    }

    ret = del_name_pid(&n_pid);
    if (ret) {
        return "[mem::get_pid()] del_name_pid() "
               "returned -1";
    }

    return std::nullopt;
}


//open /proc/`pidof cs2`/{maps,mem}
std::optional<std::string> mem::get_mem_handles(pid_t cs2_pid) {

    int ret;

    ret = open_memory(cs2_pid, &this->stream_maps, &this->fd_mem);
    if (ret) {
        return "[mem::get_mem_handles()] open_memory() "
               "returned -1";
    }

    return std::nullopt;
}


/*
 *  public
 */

//initialiser
std::optional<std::string> mem::init() {

    int ret;
    std::optional<std::string> o_ret;

    pid_t cs2_pid;

    o_ret = get_pid(&cs2_pid);
    if (o_ret) return o_ret;

    o_ret = get_mem_handles(cs2_pid);
    if (o_ret) return o_ret;

    ret = new_maps_data(&this->m_data);
    if (ret) return "[mem::init] new_maps_data() "
                    "returned -1";

    ret = read_maps(&this->m_data, this->stream_maps);
    if (ret) return "[mem::init] read_maps() "
                    "returned -1";
    
    return std::nullopt;
}


//finisher
std::optional<std::string> mem::fini() {

    int ret;

    ret = close(this->fd_mem);
    if (ret) return "[mem::fini] close() "
                    "returned -1";

    ret = fclose(this->stream_maps);
    if (ret) return "[mem::fini] fclose() "
                    "returned -1";

    ret = del_maps_data(&this->m_data);
    if (ret) return "[mem::fini] del_maps_data() "
                    "returned -1";

    return std::nullopt;
}


//read arbitrary type T from remote address
template <typename T> std::optional<std::string> mem::read_addr(uintptr_t addr,
                                                                T * buf) {
    off_t ret;
    ssize_t read_bytes;

    ret = lseek(this->fd_mem, addr, SEEK_SET);
    if (ret == -1) return "[mem::read_addr] lseek() "
                          "returned -1";
    

    read_bytes = read(this->fd_mem, buf, sizeof(*buf));
    if (read_bytes == -1) return "[mem::read_addr] read() "
                                 "returned -1";

    return std::nullopt;
}


//read array of type T from remote address
template <typename T> std::optional<std::string> mem::read_array_addr(uintptr_t addr,
                                                                      T * buf, 
                                                                      int len) {
    off_t ret;
    ssize_t read_bytes;

    ret = lseek(this->fd_mem, addr, SEEK_SET);
    if (ret == -1) return "[mem::read_array_addr] lseek() "
                          "returned -1";

    read_bytes = read(this->fd_mem, buf, sizeof(*buf) * len);
    if (read_bytes == -1) return "[mem::read_array_addr] read() "
                                 "returned -1";
}

//get start address of remote backing file
std::optional<std::string> mem::get_backing_file_addr(const char * backing_file,
                                                      uintptr_t * addr) {
    int ret;
    maps_obj * matched_obj;
    maps_entry * first_segment_entry;

    ret = get_obj_by_basename((char *) backing_file, 
                              &this->m_data, &matched_obj);
    //if error
    if (ret == -2) return "[mem::get_backing_file_addr] get_obj_by_base_name() "
                          "returned -2";
    //if not found
    if (ret == -1) {
        *addr = 0;
        return std::nullopt;
    }


    ret = vector_get(&matched_obj->entry_vector, 0, (byte *) &first_segment_entry);
    if (ret) return "[mem::get_backing_file_addr] vector_get() "
                    "returned -1";

    *addr = (uintptr_t) first_segment_entry->start_addr;

    return std::nullopt;
} 

