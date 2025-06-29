#include "ncdevmem.h"

#include <stdlib.h>

#include <ynl/netdev-user.h>
#include <ynl/ynl.h>

struct ncdevmem {
	struct ynl_sock *ys;
	int dmabuf_id;
};

struct ncdevmem *ncdevmem_setup(int ifindex, int nqueues, int dmabuf_fd)
{
	struct ncdevmem *ncdevmem;

	struct netdev_queue_id *queues;
	struct netdev_bind_rx_req *req;
	struct netdev_bind_rx_rsp *rsp;

	struct ynl_error *yerr;

	ncdevmem = (struct ncdevmem *) malloc(sizeof(struct ncdevmem));
	if (ncdevmem == NULL)
		goto RETURN_NULL;

	queues = (struct netdev_queue_id *)
		 malloc(sizeof(struct netdev_queue_id) * nqueues);
	if (queues == NULL)
		goto FREE_NCDEVMEM;

	for (int i = 0; i < nqueues; i++) {
		queues[i]._present.type = 1;
		queues[i]._present.id = 1;
		queues[i].type = NETDEV_QUEUE_TYPE_RX;
		queues[i].id = (i + 1);
	}

	ncdevmem->ys = ynl_sock_create(&ynl_netdev_family, yerr);
	if (ncdevmem->ys == NULL)
		goto FREE_QUEUES;

	req = netdev_bind_rx_req_alloc();
	if (req == NULL)
		goto DESTROY_YNL_SOCK;

	netdev_bind_rx_req_set_ifindex(req, ifindex);
	netdev_bind_rx_req_set_fd(req, dmabuf_fd);
	__netdev_bind_rx_req_set_queues(req, queues, nqueues);

	rsp = netdev_bind_rx(ncdevmem->ys, req);
	if (rsp == NULL)
		goto RX_REQ_FREE;

	if (rsp->_present.id == 0)
		goto RX_RSP_FREE;

	ncdevmem->dmabuf_id = rsp->id;

	netdev_bind_rx_rsp_free(rsp);
	netdev_bind_rx_req_free(req);

	return ncdevmem;

RX_RSP_FREE:		netdev_bind_rx_rsp_free(rsp);
RX_REQ_FREE:		netdev_bind_rx_req_free(req);
			queues = NULL;
DESTROY_YNL_SOCK:	ynl_sock_destroy(ncdevmem->ys);
FREE_QUEUES:		if (queues != NULL) free(queues);
FREE_NCDEVMEM:		free(ncdevmem);
RETURN_NULL:		return NULL;
}

int ncdevmem_get_dmabuf_id(struct ncdevmem *ncdevmem)
{
	return ncdevmem->dmabuf_id;
}

void ncdevmem_cleanup(struct ncdevmem *ncdevmem)
{
	ynl_sock_destroy(ncdevmem->ys);
	free(ncdevmem);
}
