/*
 * split from fs
 * guzhoudiaoke@126.com
 * 2018-01-20
 */

#include "file_table.h"
#include "string.h"
#include "babyos.h"
#include "socket.h"

void file_table_t::init()
{
    m_lock.init();
    memset(m_file_table, 0, sizeof(file_t) * MAX_FILE_NUM);
}

file_t* file_table_t::alloc()
{
    file_t* file = NULL;
    m_lock.lock_irqsave();
    for (int i = 0; i < MAX_FILE_NUM; i++) {
        if (m_file_table[i].m_ref == 0) {
            file = &m_file_table[i];
            file->m_ref = 1;
            break;
        }
    }
    m_lock.unlock_irqrestore();

    return file;
}

int file_table_t::free(file_t* file)
{
    file_t f;

    m_lock.lock_irqsave();
    if (file->m_ref < 1) {
        os()->panic("ref < 1 when file free");
    }

    if (--file->m_ref > 0) {
        m_lock.unlock_irqrestore();
        return 0;
    }

    f = *file;
    file->m_type = file_t::TYPE_NONE;
    m_lock.unlock_irqrestore();

    if (f.m_type == file_t::TYPE_PIPE) {
        f.m_pipe->close(f.m_writeable);
    }
    else if (f.m_type == file_t::TYPE_SOCKET) {
        f.m_socket->release();
    }
    else if (f.m_type == file_t::TYPE_INODE) {
        os()->get_fs()->put_inode(f.m_inode);
    }

    return 0;
}

file_t* file_table_t::dup_file(file_t* file)
{
    m_lock.lock_irqsave();
    if (file->m_ref < 1) {
        os()->panic("ref < 1 when file dup");
    }
    file->m_ref++;
    m_lock.unlock_irqrestore();

    return file;
}

