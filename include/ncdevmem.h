#ifndef NCDEVMEM_H__
#define NCDEVMEM_H__

struct ncdevmem;

struct ncdevmem *ncdevmem_setup(int ifindex, int nqueues, int dmabuf_fd);

int ncdevmem_get_dmabuf_id(struct ncdevmem *);

void ncdevmem_cleanup(struct ncdevmem *);

#endif
