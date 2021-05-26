/*
 * @Descripttion: 
 * @version: 
 * @Author: sueRimn
 * @Date: 2021-05-06 09:35:21
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2021-05-18 20:55:17
 */
#ifndef LOG_COPY_H
#define LOG_COPY_H

#define TOTAL_SIZE 1024*8
#define BUFFER_SIZE (TOTAL_SIZE - 1)

#include "dhmp_transport.h"



enum log_read_state {
	MID_READ_WAIT,
    MID_READ_KEY,
    MID_READ_VALUE,
    MID_READ_FAIL
};


// #define GET_BUFF_MR_ADDR(PTR) ( ((Ringbuff*)(PTR->addr))->buff )
// #define GET_BUFF_ADDR(PTR)  ( (Ringbuff*)(PTR->addr )) 

/*
 设定总空间为size-1，始终保留1个字节，如果剩余一个字节空间，就表示buff已满。

    先写：再移动写指针，
    后读，再移动读指针
    无锁？
*/

typedef struct Ring_buff
{
    unsigned int wr_pointer;              // 写指针cache
    unsigned int rd_pointer;               // 读指针cache
    int size;                      // 字节大小
}Ringbuff;

typedef struct Ring_buff_remote
{
    unsigned int wr_pointer;              // 写指针cache
    unsigned int rd_pointer;               // 读指针cache
    unsigned int size;                      // 字节大小
    
    int node_id;
    void* buff_mate;              // 远端buff的元数据地址
    void* buff;                  //  buff的远端地址 
}RemoteRingbuff;


typedef struct Ring_buff_local_mate
{
    struct dhmp_mr buff_mate_mr; 
}LocalMateRingbuff;

typedef struct ring_buff_local
{
    unsigned int wr_pointer;               // 写偏移量, 指向可写位置或者1字节的终点
    unsigned int rd_pointer;               // 读偏移量，指向可读位置或者1字节的终点
    unsigned int size;                     
    
    void * buff_addr;
    struct dhmp_mr buff_mr;
}LocalRingbuff;



#define SIZE_INT (sizeof(int))
#define REMOTE_RD_OFFSET (SIZE_INT )

// #define REMOTE_WR_ADDR(PTR) (PTR->buff_mate)
// #define REMOTE_RD_ADDR(PTR) (PTR->buff_mate + SIZE_INT )
// #define REMOTE_SIZE(PTR) (PTR->buff_mate + 2 * SIZE_INT )


// #define PUSH_ADDR (PTR) (PTR->buff + START_REMOTE_WR_PTR(PTR) )
// #define POP_ADDR (PTR) (PTR->buff + START_REMOTE_RD_PTR(PTR) )

// typedef struct remote_buff
// {
//     void* buff_mate;              
//     void* buff;
// }RemoteRingbuff;

#define KEY_LEN(l) ((l).mateData.key_length)
#define VALUE_LEN(l) ((l).mateData.value_length)

#define PTR_KEY_LEN(l) ( (l)->mateData.key_length)
#define PTR_VALUE_LEN(l) ((l)->mateData.value_length)


#define PTR_LOG_DATA_ADDR(l) ( (void*)(l) + sizeof(logEntry))
#define PTR_LOG_VALUE_ADDR(l) ( (void*)(l) + sizeof(logEntry) + PTR_KEY_LEN(l))
#define PTR_LOG_VALUE_TAG_ADDR(l) ( (void*)(l) + sizeof(logEntry) + PTR_KEY_LEN(l) + PTR_VALUE_LEN(l) - 1)


typedef struct logMateData
{
    unsigned int key_length;            //  key部分长度
    unsigned int value_length;          //  data部分长度
    // char data[];                        //  key + value
}logMateData;


typedef struct logEntry
{
    logMateData mateData;
    unsigned int     dataPos;       // 写者需要，读者不需要
    void*            dataAddr;      // 写者需要，读者不需要
    /* data */
}logEntry;




extern LocalRingbuff *local_recv_buff;
extern LocalMateRingbuff * local_recv_buff_mate;
extern RemoteRingbuff * remote_buff;

// 以字节为单位进行读写
bool rb_write (void *upper_api_buf, int len);
bool rb_read (void *buf, int len, bool isCopy);
void buff_init();

static inline int rb_data_size (Ringbuff *rb)    //计算数据空间大小
{
    return ( (rb->wr_pointer - rb->rd_pointer) & (rb->size -1));   
}

static inline int rb_free_size (Ringbuff *rb)   //计算空闲空间大小
{
    return ( rb->size - 1 - rb_data_size(rb));
}

static inline void update_wr_local(int new_ptr)
{
    // get_wr_local() = pos + len;
    remote_buff->wr_pointer= new_ptr;
}

static inline int get_wr_local()
{
    // get_wr_local() = pos + len;
    return remote_buff->wr_pointer;
}

static inline void* get_wr_addr_local()
{
    // get_wr_local() = pos + len;
    return &(remote_buff->wr_pointer);
}

static inline bool test_done(void * addr)
{
    char    test = 1;
    return ( *(char*)addr ^ test);
}


#endif