/*	$OpenBSD: kern_bufq.c,v 1.28 2014/07/12 18:43:32 tedu Exp $	*/
/*
 * Copyright (c) 2010 Thordur I. Bjornsson <thib@secnorth.net>
 * Copyright (c) 2010 David Gwynne <dlg@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mount.h>
#include <sys/mutex.h>
#include <sys/buf.h>
#include <sys/errno.h>
#include <sys/queue.h>

#include <sys/disklabel.h>

SLIST_HEAD(, bufq)	bufqs = SLIST_HEAD_INITIALIZER(bufqs);
struct mutex		bufqs_mtx = MUTEX_INITIALIZER(IPL_NONE);
int			bufqs_stop;

struct bufq_impl {
	void		*(*impl_create)(void);
	void		 (*impl_destroy)(void *);

	void		 (*impl_queue)(void *, struct buf *);
	struct buf	*(*impl_dequeue)(void *);
	void		 (*impl_requeue)(void *, struct buf *);
	int		 (*impl_peek)(void *);
};

void		*bufq_fifo_create(void);
void		 bufq_fifo_destroy(void *);
void		 bufq_fifo_queue(void *, struct buf *);
struct buf	*bufq_fifo_dequeue(void *);
void		 bufq_fifo_requeue(void *, struct buf *);
int		 bufq_fifo_peek(void *);

const struct bufq_impl bufq_impls[BUFQ_HOWMANY] = {
	{
		bufq_fifo_create,
		bufq_fifo_destroy,
		bufq_fifo_queue,
		bufq_fifo_dequeue,
		bufq_fifo_requeue,
		bufq_fifo_peek
	}
};

int
bufq_init(struct bufq *bq, int type)
{
	u_int hi = BUFQ_HI, low = BUFQ_LOW;

	if (type > BUFQ_HOWMANY)
		panic("bufq_init: type %i unknown", type);

	/*
	 * Ensure that writes can't consume the entire amount of kva
	 * available the buffer cache if we only have a limited amount
	 * of kva available to us.
	 */
	if (hi >= (bcstats.kvaslots / 16)) {
		hi = bcstats.kvaslots / 16;
		if (hi < 2)
			hi = 2;
		low = hi / 2;
	}

	bzero(bq, sizeof(struct bufq));
	mtx_init(&bq->bufq_mtx, IPL_BIO);
	bq->bufq_hi = hi;
	bq->bufq_low = low;
	bq->bufq_type = type;
	bq->bufq_stop = 0;
	bq->bufq_impl = &bufq_impls[type];
	bq->bufq_data = bq->bufq_impl->impl_create();
	if (bq->bufq_data == NULL) {
		/*
		 * we should actually return failure so disks attaching after
		 * boot in low memory situations dont panic the system.
		 */
		panic("bufq init fail");
	}

	mtx_enter(&bufqs_mtx);
	while (bufqs_stop) {
		msleep(&bufqs_stop, &bufqs_mtx, PRIBIO, "bqinit", 0);
	}
	SLIST_INSERT_HEAD(&bufqs, bq, bufq_entries);
	mtx_leave(&bufqs_mtx);

	return (0);
}

int
bufq_switch(struct bufq *bq, int type)
{
	void		*data;
	void		*odata;
	int		otype;
	struct buf	*bp;
	int		ret;

	mtx_enter(&bq->bufq_mtx);
	ret = (bq->bufq_type == type);
	mtx_leave(&bq->bufq_mtx);
	if (ret)
		return (0);

	data = bufq_impls[type].impl_create();
	if (data == NULL)
		return (ENOMEM);

	mtx_enter(&bq->bufq_mtx);
	if (bq->bufq_type != type) { /* might have changed during create */
		odata = bq->bufq_data;
		otype = bq->bufq_type;

		while ((bp = bufq_impls[otype].impl_dequeue(odata)) != NULL)
			bufq_impls[type].impl_queue(data, bp);

		bq->bufq_data = data;
		bq->bufq_type = type;
		bq->bufq_impl = &bufq_impls[type];
	} else {
		otype = type;
		odata = data;
	}
	mtx_leave(&bq->bufq_mtx);

	bufq_impls[otype].impl_destroy(odata);

	return (0);
}

void
bufq_destroy(struct bufq *bq)
{
	bufq_drain(bq);

	bq->bufq_impl->impl_destroy(bq->bufq_data);
	bq->bufq_data = NULL;

	mtx_enter(&bufqs_mtx);
	while (bufqs_stop) {
		msleep(&bufqs_stop, &bufqs_mtx, PRIBIO, "bqdest", 0);
	}
	SLIST_REMOVE(&bufqs, bq, bufq, bufq_entries);
	mtx_leave(&bufqs_mtx);
}


void
bufq_queue(struct bufq *bq, struct buf *bp, int *bqwait)
{
	mtx_enter(&bq->bufq_mtx);
	while (bq->bufq_stop) {
		msleep(&bq->bufq_stop, &bq->bufq_mtx, PRIBIO, "bqqueue", 0);
	}

	bp->b_bq = bq;
	bq->bufq_outstanding++;
	bq->bufq_impl->impl_queue(bq->bufq_data, bp);
	mtx_leave(&bq->bufq_mtx);

	/* If it's an asynchronous write, the caller should bufq_wait(). */
	*bqwait = (bp->b_flags & B_READ) == 0 && (bp->b_flags & B_ASYNC);
}

struct buf *
bufq_dequeue(struct bufq *bq)
{
	struct buf	*bp;

	mtx_enter(&bq->bufq_mtx);
	bp = bq->bufq_impl->impl_dequeue(bq->bufq_data);
	mtx_leave(&bq->bufq_mtx);

	return (bp);
}

void
bufq_requeue(struct bufq *bq, struct buf *bp)
{
	mtx_enter(&bq->bufq_mtx);
	bq->bufq_impl->impl_requeue(bq->bufq_data, bp);
	mtx_leave(&bq->bufq_mtx);
}

int
bufq_peek(struct bufq *bq)
{
	int		rv;

	mtx_enter(&bq->bufq_mtx);
	rv = bq->bufq_impl->impl_peek(bq->bufq_data);
	mtx_leave(&bq->bufq_mtx);

	return (rv);
}

void
bufq_drain(struct bufq *bq)
{
	struct buf	*bp;
	int		 s;

	while ((bp = bufq_dequeue(bq)) != NULL) {
		bp->b_error = ENXIO;
		bp->b_flags |= B_ERROR;
		s = splbio();
		biodone(bp);
		splx(s);
	}
}

void
bufq_wait(struct bufq *bq)
{
	if (bq->bufq_hi) {
		assertwaitok();
		mtx_enter(&bq->bufq_mtx);
		while (bq->bufq_outstanding >= bq->bufq_hi) {
			bq->bufq_waiting++;
			msleep(&bq->bufq_waiting, &bq->bufq_mtx,
			    PRIBIO, "bqwait", 0);
			bq->bufq_waiting--;
		}
		mtx_leave(&bq->bufq_mtx);
	}
}

void
bufq_done(struct bufq *bq, struct buf *bp)
{
	mtx_enter(&bq->bufq_mtx);
	KASSERT(bq->bufq_outstanding > 0);
	bq->bufq_outstanding--;
	if (bq->bufq_stop && bq->bufq_outstanding == 0)
		wakeup(&bq->bufq_outstanding);
	if (bq->bufq_waiting && bq->bufq_outstanding < bq->bufq_low)
		wakeup(&bq->bufq_waiting);
	mtx_leave(&bq->bufq_mtx);
	bp->b_bq = NULL;
}

void
bufq_quiesce(void)
{
	struct bufq		*bq;

	mtx_enter(&bufqs_mtx);
	bufqs_stop = 1;
	mtx_leave(&bufqs_mtx);
	/*
	 * We can safely walk the list since it can't be modified as
	 * long as bufqs_stop is non-zero.
	 */
	SLIST_FOREACH(bq, &bufqs, bufq_entries) {
		mtx_enter(&bq->bufq_mtx);
		bq->bufq_stop = 1;
		while (bq->bufq_outstanding) {
			msleep(&bq->bufq_outstanding, &bq->bufq_mtx,
			    PRIBIO, "bqquies", 0);
		}
		mtx_leave(&bq->bufq_mtx);
	}
}

void
bufq_restart(void)
{
	struct bufq		*bq;

	mtx_enter(&bufqs_mtx);
	SLIST_FOREACH(bq, &bufqs, bufq_entries) {
		mtx_enter(&bq->bufq_mtx);
		bq->bufq_stop = 0;
		wakeup(&bq->bufq_stop);
		mtx_leave(&bq->bufq_mtx);
	}
	bufqs_stop = 0;
	wakeup(&bufqs_stop);
	mtx_leave(&bufqs_mtx);
}

/* F I F O   I M P L M E N T A T I O N */

void *
bufq_fifo_create(void)
{
	struct bufq_fifo_head	*head;

	head = malloc(sizeof(*head), M_DEVBUF, M_NOWAIT | M_ZERO);
	if (head == NULL)
		return (NULL);

	SIMPLEQ_INIT(head);

	return (head);
}

void
bufq_fifo_destroy(void *data)
{
	free(data, M_DEVBUF, 0);
}

void
bufq_fifo_queue(void *data, struct buf *bp)
{
	struct bufq_fifo_head	*head = data;

	SIMPLEQ_INSERT_TAIL(head, bp, b_bufq.bufq_data_fifo.bqf_entries);
}

struct buf *
bufq_fifo_dequeue(void *data)
{
	struct bufq_fifo_head	*head = data;
	struct buf		*bp;

	bp = SIMPLEQ_FIRST(head);
	if (bp != NULL)
		SIMPLEQ_REMOVE_HEAD(head, b_bufq.bufq_data_fifo.bqf_entries);

	return (bp);
}

void
bufq_fifo_requeue(void *data, struct buf *bp)
{
	struct bufq_fifo_head	*head = data;

	SIMPLEQ_INSERT_HEAD(head, bp, b_bufq.bufq_data_fifo.bqf_entries);
}

int
bufq_fifo_peek(void *data)
{
	struct bufq_fifo_head	*head = data;

	return (SIMPLEQ_FIRST(head) != NULL);
}
