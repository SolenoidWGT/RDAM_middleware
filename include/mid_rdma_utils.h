#ifndef MID_RDMA_UTILS_H
#define MID_RDMA_UTILS_H

#include "dhmp.h"
#include "dhmp_transport.h"


struct ibv_mr * dhmp_memory_malloc_register(struct ibv_pd *pd,  size_t length, int nvm_node);

int dhmp_memory_register(struct ibv_pd *pd, 
									struct dhmp_mr *dmr, size_t length);


struct dhmp_transport* dhmp_node_select_by_id(int node_id);

int client_find_server_id();

#endif