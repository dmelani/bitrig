/*	$OpenBSD: rf_reconstruct.c,v 1.1 1999/01/11 14:29:46 niklas Exp $	*/
/*	$NetBSD: rf_reconstruct.c,v 1.1 1998/11/13 04:20:33 oster Exp $	*/
/*
 * Copyright (c) 1995 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Mark Holland
 *
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */

/************************************************************
 *
 * rf_reconstruct.c -- code to perform on-line reconstruction
 *
 ************************************************************/

/*
 * :  
 * Log: rf_reconstruct.c,v 
 * Revision 1.65  1996/08/06 22:24:56  jimz
 * get rid of sys/buf.h on linux
 *
 * Revision 1.64  1996/07/30  04:28:53  jimz
 * include rf_types.h first
 *
 * Revision 1.63  1996/07/27  23:36:08  jimz
 * Solaris port of simulator
 *
 * Revision 1.62  1996/07/17  21:00:58  jimz
 * clean up timer interface, tracing
 *
 * Revision 1.61  1996/07/15  05:40:41  jimz
 * some recon datastructure cleanup
 * better handling of multiple failures
 * added undocumented double-recon test
 *
 * Revision 1.60  1996/07/15  02:57:18  jimz
 * added debugging (peek at first couple bytes of recon buffers
 * as they go by)
 *
 * Revision 1.59  1996/07/13  00:00:59  jimz
 * sanitized generalized reconstruction architecture
 * cleaned up head sep, rbuf problems
 *
 * Revision 1.58  1996/07/11  19:08:00  jimz
 * generalize reconstruction mechanism
 * allow raid1 reconstructs via copyback (done with array
 * quiesced, not online, therefore not disk-directed)
 *
 * Revision 1.57  1996/06/17  14:38:33  jimz
 * properly #if out RF_DEMO code
 * fix bug in MakeConfig that was causing weird behavior
 * in configuration routines (config was not zeroed at start)
 * clean up genplot handling of stacks
 *
 * Revision 1.56  1996/06/17  03:24:59  jimz
 * include shutdown.h for define of now-macroized ShutdownCreate
 *
 * Revision 1.55  1996/06/11  10:58:36  jimz
 * get rid of simulator-testcode artifacts
 * add generic ReconDoneProc mechanism instead
 *
 * Revision 1.54  1996/06/10  14:18:58  jimz
 * move user, throughput stats into per-array structure
 *
 * Revision 1.53  1996/06/10  11:55:47  jimz
 * Straightened out some per-array/not-per-array distinctions, fixed
 * a couple bugs related to confusion. Added shutdown lists. Removed
 * layout shutdown function (now subsumed by shutdown lists).
 *
 * Revision 1.52  1996/06/09  02:36:46  jimz
 * lots of little crufty cleanup- fixup whitespace
 * issues, comment #ifdefs, improve typing in some
 * places (esp size-related)
 *
 * Revision 1.51  1996/06/07  22:26:27  jimz
 * type-ify which_ru (RF_ReconUnitNum_t)
 *
 * Revision 1.50  1996/06/07  21:33:04  jimz
 * begin using consistent types for sector numbers,
 * stripe numbers, row+col numbers, recon unit numbers
 *
 * Revision 1.49  1996/06/06  01:24:36  jimz
 * don't get rid of reconCtrlPtr until we're done with it
 *
 * Revision 1.48  1996/06/05  18:06:02  jimz
 * Major code cleanup. The Great Renaming is now done.
 * Better modularity. Better typing. Fixed a bunch of
 * synchronization bugs. Made a lot of global stuff
 * per-desc or per-array. Removed dead code.
 *
 * Revision 1.47  1996/06/03  23:28:26  jimz
 * more bugfixes
 * check in tree to sync for IPDS runs with current bugfixes
 * there still may be a problem with threads in the script test
 * getting I/Os stuck- not trivially reproducible (runs ~50 times
 * in a row without getting stuck)
 *
 * Revision 1.46  1996/06/02  17:31:48  jimz
 * Moved a lot of global stuff into array structure, where it belongs.
 * Fixed up paritylogging, pss modules in this manner. Some general
 * code cleanup. Removed lots of dead code, some dead files.
 *
 * Revision 1.45  1996/05/31  22:26:54  jimz
 * fix a lot of mapping problems, memory allocation problems
 * found some weird lock issues, fixed 'em
 * more code cleanup
 *
 * Revision 1.44  1996/05/30  23:22:16  jimz
 * bugfixes of serialization, timing problems
 * more cleanup
 *
 * Revision 1.43  1996/05/30  11:29:41  jimz
 * Numerous bug fixes. Stripe lock release code disagreed with the taking code
 * about when stripes should be locked (I made it consistent: no parity, no lock)
 * There was a lot of extra serialization of I/Os which I've removed- a lot of
 * it was to calculate values for the cache code, which is no longer with us.
 * More types, function, macro cleanup. Added code to properly quiesce the array
 * on shutdown. Made a lot of stuff array-specific which was (bogusly) general
 * before. Fixed memory allocation, freeing bugs.
 *
 * Revision 1.42  1996/05/27  18:56:37  jimz
 * more code cleanup
 * better typing
 * compiles in all 3 environments
 *
 * Revision 1.41  1996/05/24  22:17:04  jimz
 * continue code + namespace cleanup
 * typed a bunch of flags
 *
 * Revision 1.40  1996/05/24  04:40:40  jimz
 * don't do demoMode stuff in kernel
 *
 * Revision 1.39  1996/05/24  01:59:45  jimz
 * another checkpoint in code cleanup for release
 * time to sync kernel tree
 *
 * Revision 1.38  1996/05/23  21:46:35  jimz
 * checkpoint in code cleanup (release prep)
 * lots of types, function names have been fixed
 *
 * Revision 1.37  1996/05/23  00:33:23  jimz
 * code cleanup: move all debug decls to rf_options.c, all extern
 * debug decls to rf_options.h, all debug vars preceded by rf_
 *
 * Revision 1.36  1996/05/18  19:51:34  jimz
 * major code cleanup- fix syntax, make some types consistent,
 * add prototypes, clean out dead code, et cetera
 *
 * Revision 1.35  1996/05/01  16:28:16  jimz
 * don't include ccmn.h
 *
 * Revision 1.34  1995/12/12  18:10:06  jimz
 * MIN -> RF_MIN, MAX -> RF_MAX, ASSERT -> RF_ASSERT
 * fix 80-column brain damage in comments
 *
 * Revision 1.33  1995/12/06  15:05:09  root
 * added copyright info
 *
 * Revision 1.32  1995/11/17  19:04:11  wvcii
 * added prototyping to ComputePSDiskOffsets
 * prow and pcol now type int (were u_int)
 *
 * Revision 1.31  1995/11/17  01:39:35  amiri
 * isolated some demo related stuff
 *
 * Revision 1.30  1995/10/18  19:33:14  amiri
 * removed fflush (stdin/stdout) calls from ReconstructFailedDisk
 *
 * Revision 1.29  1995/10/11  10:20:33  jimz
 * #if 0'd problem code for sigmetrics
 *
 * Revision 1.28  1995/10/10  23:18:15  amiri
 * added fflushes to stdin/stdout before requesting
 * input in demo mode.
 *
 * Revision 1.27  1995/10/10  19:24:47  amiri
 * took out update_mode (for demo) from
 * KERNEL source.
 *
 * Revision 1.26  1995/10/09  23:35:48  amiri
 * added support for more meters in recon. demo
 *
 * Revision 1.25  1995/07/03  18:14:30  holland
 * changed the way the number of floating recon bufs &
 * the head sep limit get set
 *
 * Revision 1.24  1995/07/02  15:07:42  holland
 * bug fixes related to getting distributed sparing numbers
 *
 * Revision 1.23  1995/06/23  13:36:36  robby
 * updeated to prototypes in rf_layout.h
 *
*/

#ifdef _KERNEL
#define KERNEL
#endif

#include "rf_types.h"
#include <sys/time.h>
#ifndef LINUX
#include <sys/buf.h>
#endif /* !LINUX */
#include <sys/errno.h>
#include "rf_raid.h"
#include "rf_reconutil.h"
#include "rf_revent.h"
#include "rf_reconbuffer.h"
#include "rf_threadid.h"
#include "rf_acctrace.h"
#include "rf_etimer.h"
#include "rf_dag.h"
#include "rf_desc.h"
#include "rf_general.h"
#include "rf_freelist.h"
#include "rf_debugprint.h"
#include "rf_driver.h"
#include "rf_utils.h"
#include "rf_cpuutil.h"
#include "rf_shutdown.h"
#include "rf_sys.h"

#if RF_DEMO > 0
#include "rf_demo.h"
#endif /* RF_DEMO > 0 */

#ifdef KERNEL
#include "rf_kintf.h"
#endif /* KERNEL */

/* setting these to -1 causes them to be set to their default values if not set by debug options */

#define Dprintf(s)         if (rf_reconDebug) rf_debug_printf(s,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL)
#define Dprintf1(s,a)         if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),NULL,NULL,NULL,NULL,NULL,NULL,NULL)
#define Dprintf2(s,a,b)       if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),NULL,NULL,NULL,NULL,NULL,NULL)
#define Dprintf3(s,a,b,c)     if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),NULL,NULL,NULL,NULL,NULL)
#define Dprintf4(s,a,b,c,d)   if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),(void *)((unsigned long)d),NULL,NULL,NULL,NULL)
#define Dprintf5(s,a,b,c,d,e) if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),(void *)((unsigned long)d),(void *)((unsigned long)e),NULL,NULL,NULL)
#define Dprintf6(s,a,b,c,d,e,f) if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),(void *)((unsigned long)d),(void *)((unsigned long)e),(void *)((unsigned long)f),NULL,NULL)
#define Dprintf7(s,a,b,c,d,e,f,g) if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),(void *)((unsigned long)d),(void *)((unsigned long)e),(void *)((unsigned long)f),(void *)((unsigned long)g),NULL)
#define Dprintf8(s,a,b,c,d,e,f,g,h) if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),(void *)((unsigned long)d),(void *)((unsigned long)e),(void *)((unsigned long)f),(void *)((unsigned long)g),(void *)((unsigned long)h))

#define DDprintf1(s,a)         if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),NULL,NULL,NULL,NULL,NULL,NULL,NULL)
#define DDprintf2(s,a,b)       if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),NULL,NULL,NULL,NULL,NULL,NULL)
#define DDprintf3(s,a,b,c)     if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),NULL,NULL,NULL,NULL,NULL)
#define DDprintf4(s,a,b,c,d)   if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),(void *)((unsigned long)d),NULL,NULL,NULL,NULL)
#define DDprintf5(s,a,b,c,d,e) if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),(void *)((unsigned long)d),(void *)((unsigned long)e),NULL,NULL,NULL)
#define DDprintf6(s,a,b,c,d,e,f) if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),(void *)((unsigned long)d),(void *)((unsigned long)e),(void *)((unsigned long)f),NULL,NULL)
#define DDprintf7(s,a,b,c,d,e,f,g) if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),(void *)((unsigned long)d),(void *)((unsigned long)e),(void *)((unsigned long)f),(void *)((unsigned long)g),NULL)
#define DDprintf8(s,a,b,c,d,e,f,g,h) if (rf_reconDebug) rf_debug_printf(s,(void *)((unsigned long)a),(void *)((unsigned long)b),(void *)((unsigned long)c),(void *)((unsigned long)d),(void *)((unsigned long)e),(void *)((unsigned long)f),(void *)((unsigned long)g),(void *)((unsigned long)h))

#ifdef KERNEL
static RF_Thread_t      recon_thr_handle;
static int              recon_thread_initialized = 0;
#endif /* KERNEL */

static RF_FreeList_t *rf_recond_freelist;
#define RF_MAX_FREE_RECOND  4
#define RF_RECOND_INC       1

static RF_RaidReconDesc_t *AllocRaidReconDesc(RF_Raid_t *raidPtr,
	RF_RowCol_t row, RF_RowCol_t col, RF_RaidDisk_t *spareDiskPtr,
	int numDisksDone, RF_RowCol_t srow, RF_RowCol_t scol);
static void FreeReconDesc(RF_RaidReconDesc_t *reconDesc);
static int ProcessReconEvent(RF_Raid_t *raidPtr, RF_RowCol_t frow,
	RF_ReconEvent_t *event);
static int IssueNextReadRequest(RF_Raid_t *raidPtr, RF_RowCol_t row,
	RF_RowCol_t col);
static int TryToRead(RF_Raid_t *raidPtr, RF_RowCol_t row, RF_RowCol_t col);
static int ComputePSDiskOffsets(RF_Raid_t *raidPtr, RF_StripeNum_t psid,
	RF_RowCol_t row, RF_RowCol_t col, RF_SectorNum_t *outDiskOffset,
	RF_SectorNum_t *outFailedDiskSectorOffset, RF_RowCol_t *spRow,
	RF_RowCol_t *spCol, RF_SectorNum_t *spOffset);
static int IssueNextWriteRequest(RF_Raid_t *raidPtr, RF_RowCol_t row);
static int ReconReadDoneProc(void *arg, int status);
static int ReconWriteDoneProc(void *arg, int status);
static void CheckForNewMinHeadSep(RF_Raid_t *raidPtr, RF_RowCol_t row,
	RF_HeadSepLimit_t hsCtr);
static int CheckHeadSeparation(RF_Raid_t *raidPtr, RF_PerDiskReconCtrl_t *ctrl,
	RF_RowCol_t row, RF_RowCol_t col, RF_HeadSepLimit_t hsCtr,
	RF_ReconUnitNum_t which_ru);
static int CheckForcedOrBlockedReconstruction(RF_Raid_t *raidPtr,
	RF_ReconParityStripeStatus_t *pssPtr, RF_PerDiskReconCtrl_t *ctrl,
	RF_RowCol_t row, RF_RowCol_t col, RF_StripeNum_t psid,
	RF_ReconUnitNum_t which_ru);
static void ForceReconReadDoneProc(void *arg, int status);

static void rf_ShutdownReconstruction(void *);


struct RF_ReconDoneProc_s {
  void                (*proc)(RF_Raid_t *, void *);
  void                 *arg;
  RF_ReconDoneProc_t   *next;
};

static RF_FreeList_t *rf_rdp_freelist;
#define RF_MAX_FREE_RDP 4
#define RF_RDP_INC      1

static void SignalReconDone(RF_Raid_t *raidPtr)
{
  RF_ReconDoneProc_t *p;

  RF_LOCK_MUTEX(raidPtr->recon_done_proc_mutex);
  for(p=raidPtr->recon_done_procs;p;p=p->next) {
    p->proc(raidPtr, p->arg);
  }
  RF_UNLOCK_MUTEX(raidPtr->recon_done_proc_mutex);
}

int rf_RegisterReconDoneProc(
  RF_Raid_t            *raidPtr,
  void                (*proc)(RF_Raid_t *, void *),
  void                 *arg,
  RF_ReconDoneProc_t  **handlep)
{
  RF_ReconDoneProc_t *p;

  RF_FREELIST_GET(rf_rdp_freelist,p,next,(RF_ReconDoneProc_t *));
  if (p == NULL)
    return(ENOMEM);
  p->proc = proc;
  p->arg = arg;
  RF_LOCK_MUTEX(raidPtr->recon_done_proc_mutex);
  p->next = raidPtr->recon_done_procs;
  raidPtr->recon_done_procs = p;
  RF_UNLOCK_MUTEX(raidPtr->recon_done_proc_mutex);
  if (handlep)
    *handlep = p;
  return(0);
}

/*****************************************************************************************
 *
 * sets up the parameters that will be used by the reconstruction process
 * currently there are none, except for those that the layout-specific
 * configuration (e.g. rf_ConfigureDeclustered) routine sets up.
 *
 * in the kernel, we fire off the recon thread.
 *
 ****************************************************************************************/
static void rf_ShutdownReconstruction(ignored)
  void  *ignored;
{
  RF_FREELIST_DESTROY(rf_recond_freelist,next,(RF_RaidReconDesc_t *));
  RF_FREELIST_DESTROY(rf_rdp_freelist,next,(RF_ReconDoneProc_t *));
}

int rf_ConfigureReconstruction(listp)
  RF_ShutdownList_t  **listp;
{
  int rc;

  RF_FREELIST_CREATE(rf_recond_freelist, RF_MAX_FREE_RECOND,
    RF_RECOND_INC, sizeof(RF_RaidReconDesc_t));
  if (rf_recond_freelist == NULL)
    return(ENOMEM);
  RF_FREELIST_CREATE(rf_rdp_freelist, RF_MAX_FREE_RDP,
    RF_RDP_INC, sizeof(RF_ReconDoneProc_t));
  if (rf_rdp_freelist == NULL) {
    RF_FREELIST_DESTROY(rf_recond_freelist,next,(RF_RaidReconDesc_t *));
    return(ENOMEM);
  }
  rc = rf_ShutdownCreate(listp, rf_ShutdownReconstruction, NULL);
  if (rc) {
    RF_ERRORMSG3("Unable to add to shutdown list file %s line %d rc=%d\n",
      __FILE__, __LINE__, rc);
    rf_ShutdownReconstruction(NULL);
    return(rc);
  }

#ifdef KERNEL
  if (!recon_thread_initialized) {
	  RF_CREATE_THREAD(recon_thr_handle, rf_ReconKernelThread, NULL); 
	  recon_thread_initialized = 1;
  }
#endif /* KERNEL */

  return(0);
}

static RF_RaidReconDesc_t *AllocRaidReconDesc(raidPtr, row, col, spareDiskPtr, numDisksDone, srow, scol)
  RF_Raid_t      *raidPtr;
  RF_RowCol_t     row;
  RF_RowCol_t     col;
  RF_RaidDisk_t  *spareDiskPtr;
  int             numDisksDone;
  RF_RowCol_t     srow;
  RF_RowCol_t     scol;
{
  
  RF_RaidReconDesc_t *reconDesc;  

  RF_FREELIST_GET(rf_recond_freelist,reconDesc,next,(RF_RaidReconDesc_t *));

  reconDesc->raidPtr     = raidPtr;
  reconDesc->row         = row;
  reconDesc->col         = col;
  reconDesc->spareDiskPtr=spareDiskPtr;
  reconDesc->numDisksDone=numDisksDone; 
  reconDesc->srow=srow; 
  reconDesc->scol=scol;
  reconDesc->state      = 0;
  reconDesc->next       = NULL;

  return(reconDesc);
}

static void FreeReconDesc(reconDesc)
  RF_RaidReconDesc_t  *reconDesc;
{
#if RF_RECON_STATS > 0
  printf("RAIDframe: %lu recon event waits, %lu recon delays\n",
    (long)reconDesc->numReconEventWaits, (long)reconDesc->numReconExecDelays);
#endif /* RF_RECON_STATS > 0 */
#ifdef KERNEL
  printf("RAIDframe: %lu max exec ticks\n", 
	 (long)reconDesc->maxReconExecTicks);
#endif /* KERNEL */
#if (RF_RECON_STATS > 0) || defined(KERNEL)
  printf("\n");
#endif /* (RF_RECON_STATS > 0) || KERNEL */
  RF_FREELIST_FREE(rf_recond_freelist,reconDesc,next);
}


/*****************************************************************************************
 *
 * primary routine to reconstruct a failed disk.  This should be called from
 * within its own thread.  It won't return until reconstruction completes,
 * fails, or is aborted.
 ****************************************************************************************/
int rf_ReconstructFailedDisk(raidPtr, row, col)
  RF_Raid_t    *raidPtr;
  RF_RowCol_t   row;
  RF_RowCol_t   col;
{
#ifdef SIMULATE
  RF_PendingRecon_t *pend;
  RF_RowCol_t r, c;
#endif /* SIMULATE */
  RF_LayoutSW_t *lp;
  int rc;

  lp = raidPtr->Layout.map;
  if (lp->SubmitReconBuffer) {
    /*
     * The current infrastructure only supports reconstructing one
     * disk at a time for each array.
     */
#ifdef SIMULATE
    if (raidPtr->reconInProgress) {
      RF_Malloc(pend, sizeof(RF_PendingRecon_t), (RF_PendingRecon_t *));
      pend->row = row;
      pend->col = col;
      pend->next = raidPtr->pendingRecon;
      raidPtr->pendingRecon = pend;
      /* defer until current recon completes */
      return(0);
    }
    raidPtr->reconInProgress++;
#else /* SIMULATE */
    RF_LOCK_MUTEX(raidPtr->mutex);
    while (raidPtr->reconInProgress) {
      RF_WAIT_COND(raidPtr->waitForReconCond, raidPtr->mutex);
    }
    raidPtr->reconInProgress++;
    RF_UNLOCK_MUTEX(raidPtr->mutex);
#endif /* SIMULATE */
    rc = rf_ReconstructFailedDiskBasic(raidPtr, row, col);
  }
  else {
    RF_ERRORMSG1("RECON: no way to reconstruct failed disk for arch %c\n",
      lp->parityConfig);
    rc = EIO;
  }
#ifdef SIMULATE
  pend = raidPtr->pendingRecon;
  if (pend) {
    /* launch next recon */
    raidPtr->pendingRecon = pend->next;
    r = pend->row;
    c = pend->col;
    RF_Free(pend, sizeof(RF_PendingRecon_t));
    return(rf_ReconstructFailedDisk(raidPtr, r, c));
  }
#else /* SIMULATE */
  RF_LOCK_MUTEX(raidPtr->mutex);
  raidPtr->reconInProgress--;
  RF_UNLOCK_MUTEX(raidPtr->mutex);
  RF_SIGNAL_COND(raidPtr->waitForReconCond);
#if 1
#if (defined(__NetBSD__) || defined(__OpenBSD__)) && defined(_KERNEL)
  wakeup(&raidPtr->waitForReconCond); /* XXX Methinks this will be needed
					at some point... GO*/
#endif
#endif
#endif /* SIMULATE */
  return(rc);
}

int rf_ReconstructFailedDiskBasic(raidPtr, row, col)
  RF_Raid_t    *raidPtr;
  RF_RowCol_t   row;
  RF_RowCol_t   col;
{
  RF_RaidDisk_t *spareDiskPtr = NULL;
  RF_RaidReconDesc_t *reconDesc;
  RF_RowCol_t srow, scol;
  int numDisksDone=0, rc;

  /* first look for a spare drive onto which to reconstruct the data */
  /* spare disk descriptors are stored in row 0.  This may have to change eventually */

  RF_LOCK_MUTEX(raidPtr->mutex);
  RF_ASSERT (raidPtr->Disks[row][col].status == rf_ds_failed);

  if (raidPtr->Layout.map->flags & RF_DISTRIBUTE_SPARE) {
    if (raidPtr->status[row] != rf_rs_degraded) {
      RF_ERRORMSG2("Unable to reconstruct disk at row %d col %d because status not degraded\n",row,col);
      RF_UNLOCK_MUTEX(raidPtr->mutex);
      return(EINVAL);
    }
    srow = row;
    scol = (-1);
  }
  else {
    srow = 0;
    for (scol=raidPtr->numCol; scol<raidPtr->numCol + raidPtr->numSpare; scol++) {
      if (raidPtr->Disks[srow][scol].status == rf_ds_spare) {
        spareDiskPtr = &raidPtr->Disks[srow][scol];
        spareDiskPtr->status = rf_ds_used_spare;
        break;
      }
    }
    if (!spareDiskPtr) {
      RF_ERRORMSG2("Unable to reconstruct disk at row %d col %d because no spares are available\n",row,col);
      RF_UNLOCK_MUTEX(raidPtr->mutex);
      return(ENOSPC);
    }

#if RF_DEMO > 0
    if (!rf_demoMode) {
#endif /* RF_DEMO > 0 */
      printf("RECON: initiating reconstruction on row %d col %d -> spare at row %d col %d\n",row, col, srow, scol);
#if RF_DEMO > 0
    }
#endif /* RF_DEMO > 0 */
  }
  RF_UNLOCK_MUTEX(raidPtr->mutex);

  reconDesc = AllocRaidReconDesc((void *) raidPtr, row, col,spareDiskPtr, numDisksDone, srow , scol);
  raidPtr->reconDesc = (void *) reconDesc;
#if RF_RECON_STATS > 0
  reconDesc->hsStallCount = 0;
  reconDesc->numReconExecDelays = 0;
  reconDesc->numReconEventWaits = 0;
#endif /* RF_RECON_STATS > 0 */
#ifdef KERNEL
  reconDesc->reconExecTimerRunning = 0;
  reconDesc->reconExecTicks = 0;
  reconDesc->maxReconExecTicks = 0;
#endif /* KERNEL */
#if RF_DEMO > 0 && !defined(SIMULATE)
  if (rf_demoMode) {
    char cbuf[10]; 
    printf("About to start reconstruction, hit return to continue:");
    gets(cbuf);
  }
#endif /* RF_DEMO > 0 && !SIMULATE */
  rc = rf_ContinueReconstructFailedDisk(reconDesc);
  return(rc);
}


int rf_ContinueReconstructFailedDisk(reconDesc)
  RF_RaidReconDesc_t  *reconDesc;
{
  RF_Raid_t             *raidPtr=reconDesc->raidPtr;   
  RF_RowCol_t            row=reconDesc->row;
  RF_RowCol_t            col=reconDesc->col;
  RF_RowCol_t            srow=reconDesc->srow; 
  RF_RowCol_t            scol=reconDesc->scol;
  RF_ReconMap_t         *mapPtr;

  RF_ReconEvent_t *event;
  struct timeval etime, elpsd;
  unsigned long xor_s, xor_resid_us;
  int retcode,i, ds;

  switch (reconDesc->state)
    {
      
      
    case 0:     

      raidPtr->accumXorTimeUs = 0;

      /* create one trace record per physical disk */
      RF_Malloc(raidPtr->recon_tracerecs, raidPtr->numCol * sizeof(RF_AccTraceEntry_t), (RF_AccTraceEntry_t *));
      
      /* quiesce the array prior to starting recon.  this is needed to assure no nasty interactions
       * with pending user writes.  We need to do this before we change the disk or row status.
       */
      reconDesc->state=1;

      Dprintf("RECON: begin request suspend\n");      
      retcode =  rf_SuspendNewRequestsAndWait(raidPtr);
      Dprintf("RECON: end request suspend\n");      
      rf_StartUserStats(raidPtr);              /* zero out the stats kept on user accs */

#ifdef SIMULATE
      if (retcode) return(0);
#endif /* SIMULATE */

      /* fall through to state 1 */

    case 1:

      RF_LOCK_MUTEX(raidPtr->mutex);
      
      /* create the reconstruction control pointer and install it in the right slot */
      raidPtr->reconControl[row] = rf_MakeReconControl(reconDesc, row, col, srow, scol);
      mapPtr=raidPtr->reconControl[row]->reconMap;
      raidPtr->status[row] = rf_rs_reconstructing;
      raidPtr->Disks[row][col].status = rf_ds_reconstructing;
      raidPtr->Disks[row][col].spareRow = srow;
      raidPtr->Disks[row][col].spareCol = scol;
      
      RF_UNLOCK_MUTEX(raidPtr->mutex);

      RF_GETTIME(raidPtr->reconControl[row]->starttime);
#if RF_DEMO > 0
      if (rf_demoMode) {
         rf_demo_update_mode(RF_DEMO_RECON);
         rf_startup_recon_demo(rf_demoMeterVpos, raidPtr->numCol,
           raidPtr->Layout.numDataCol+raidPtr->Layout.numParityCol, 0);
      }
#endif /* RF_DEMO > 0 */

      /* now start up the actual reconstruction: issue a read for each surviving disk */
      rf_start_cpu_monitor();
      reconDesc->numDisksDone = 0;
      for (i=0; i<raidPtr->numCol; i++) {
        if (i != col) {
          /* find and issue the next I/O on the indicated disk */
	  if (IssueNextReadRequest(raidPtr, row, i)) {
            Dprintf2("RECON: done issuing for r%d c%d\n", row, i);
            reconDesc->numDisksDone++;
          }
        }
      }

    case 2:
      Dprintf("RECON: resume requests\n");
      rf_ResumeNewRequests(raidPtr);


      reconDesc->state=3;

    case 3:

      /* process reconstruction events until all disks report that they've completed all work */
      mapPtr=raidPtr->reconControl[row]->reconMap;


      
      while (reconDesc->numDisksDone < raidPtr->numCol-1) {

	event = rf_GetNextReconEvent(reconDesc, row, (void (*)(void *))rf_ContinueReconstructFailedDisk,reconDesc);
#ifdef SIMULATE
	if (event==NULL) {return(0);}
#else /* SIMULATE */
	RF_ASSERT(event);
#endif /* SIMULATE */

	if (ProcessReconEvent(raidPtr, row, event)) reconDesc->numDisksDone++;
	raidPtr->reconControl[row]->percentComplete = 100 - (rf_UnitsLeftToReconstruct(mapPtr) * 100 / mapPtr->totalRUs);
#if RF_DEMO > 0
	if (rf_prReconSched || rf_demoMode)
#else /* RF_DEMO > 0 */
	if (rf_prReconSched)
#endif /* RF_DEMO > 0 */
	{
	  rf_PrintReconSchedule(raidPtr->reconControl[row]->reconMap, &(raidPtr->reconControl[row]->starttime));
	}
      }
      


      reconDesc->state=4;
      

    case 4:
      mapPtr=raidPtr->reconControl[row]->reconMap;
      if (rf_reconDebug) {
	printf("RECON: all reads completed\n");
      }
      

      
      /* at this point all the reads have completed.  We now wait for any pending writes
       * to complete, and then we're done
       */

      while (rf_UnitsLeftToReconstruct(raidPtr->reconControl[row]->reconMap) > 0) {
	
	event = rf_GetNextReconEvent(reconDesc, row, (void (*)(void *))rf_ContinueReconstructFailedDisk,reconDesc);
#ifdef SIMULATE
	if (event==NULL) {return(0);}
#else /* SIMULATE */
	RF_ASSERT(event);
#endif /* SIMULATE */
	
	(void) ProcessReconEvent(raidPtr, row, event);         /* ignore return code */
	raidPtr->reconControl[row]->percentComplete = 100 - (rf_UnitsLeftToReconstruct(mapPtr) * 100 / mapPtr->totalRUs);
#if RF_DEMO > 0
	if (rf_prReconSched || rf_demoMode)
#else /* RF_DEMO > 0 */
	if (rf_prReconSched)
#endif /* RF_DEMO > 0 */
	{
	  rf_PrintReconSchedule(raidPtr->reconControl[row]->reconMap, &(raidPtr->reconControl[row]->starttime));
	}
      }
      reconDesc->state=5;

    case 5:
      rf_stop_cpu_monitor();

      /* Success:  mark the dead disk as reconstructed.  We quiesce the array here to assure no
       * nasty interactions with pending user accesses when we free up the psstatus structure
       * as part of FreeReconControl()
       */



      reconDesc->state=6;

      retcode =  rf_SuspendNewRequestsAndWait(raidPtr);
      rf_StopUserStats(raidPtr);
      rf_PrintUserStats(raidPtr);               /* print out the stats on user accs accumulated during recon */

#ifdef SIMULATE
      if (retcode) return(0);
#endif /* SIMULATE */

      /* fall through to state 6 */
    case 6:
      

      
      RF_LOCK_MUTEX(raidPtr->mutex);
      raidPtr->numFailures--;
      ds = (raidPtr->Layout.map->flags & RF_DISTRIBUTE_SPARE);
      raidPtr->Disks[row][col].status = (ds) ? rf_ds_dist_spared : rf_ds_spared;
      raidPtr->status[row] = (ds) ? rf_rs_reconfigured : rf_rs_optimal;
      RF_UNLOCK_MUTEX(raidPtr->mutex);
      RF_GETTIME(etime);
      RF_TIMEVAL_DIFF(&(raidPtr->reconControl[row]->starttime), &etime, &elpsd);

      /* XXX -- why is state 7 different from state 6 if there is no return() here? -- XXX
       *        Note that I set elpsd above & use it below, so if you put a return
       *        here you'll have to fix this. (also, FreeReconControl is called below)
       */
      
    case 7:

      rf_ResumeNewRequests(raidPtr);

#if RF_DEMO > 0
      if (rf_demoMode) {
        rf_finish_recon_demo(&elpsd);
      }
      else {
#endif /* RF_DEMO > 0 */
	printf("Reconstruction of disk at row %d col %d completed and spare disk reassigned\n", row, col);
	xor_s = raidPtr->accumXorTimeUs/1000000;
	xor_resid_us = raidPtr->accumXorTimeUs%1000000;
	printf("Recon time was %d.%06d seconds, accumulated XOR time was %ld us (%ld.%06ld)\n",
		(int)elpsd.tv_sec,(int)elpsd.tv_usec,raidPtr->accumXorTimeUs,xor_s,xor_resid_us);
	printf("  (start time %d sec %d usec, end time %d sec %d usec)\n",
	       (int)raidPtr->reconControl[row]->starttime.tv_sec,
	       (int)raidPtr->reconControl[row]->starttime.tv_usec,
	       (int)etime.tv_sec, (int)etime.tv_usec);
	rf_print_cpu_util("reconstruction");
#if RF_RECON_STATS > 0
	printf("Total head-sep stall count was %d\n", 
	       (int)reconDesc->hsStallCount);
#endif /* RF_RECON_STATS > 0 */
#if RF_DEMO > 0
      }
#endif /* RF_DEMO > 0 */
      rf_FreeReconControl(raidPtr, row);
      RF_Free(raidPtr->recon_tracerecs, raidPtr->numCol * sizeof(RF_AccTraceEntry_t));
      FreeReconDesc(reconDesc);
      
    }

  SignalReconDone(raidPtr);
  return (0);
}

/*****************************************************************************************
 * do the right thing upon each reconstruction event.
 * returns nonzero if and only if there is nothing left unread on the indicated disk
 ****************************************************************************************/
static int ProcessReconEvent(raidPtr, frow, event)
  RF_Raid_t        *raidPtr;
  RF_RowCol_t       frow;
  RF_ReconEvent_t  *event;
{
  int retcode = 0, submitblocked;
  RF_ReconBuffer_t *rbuf;
  RF_SectorCount_t sectorsPerRU;

  Dprintf1("RECON: ProcessReconEvent type %d\n", event->type);  
  switch(event->type) {

  /* a read I/O has completed */
  case RF_REVENT_READDONE:
    rbuf = raidPtr->reconControl[frow]->perDiskInfo[event->col].rbuf;
    Dprintf3("RECON: READDONE EVENT: row %d col %d psid %ld\n",
      frow, event->col, rbuf->parityStripeID);
    Dprintf7("RECON: done read  psid %ld buf %lx  %02x %02x %02x %02x %02x\n",
      rbuf->parityStripeID, rbuf->buffer, rbuf->buffer[0]&0xff, rbuf->buffer[1]&0xff,
      rbuf->buffer[2]&0xff, rbuf->buffer[3]&0xff, rbuf->buffer[4]&0xff);
    rf_FreeDiskQueueData((RF_DiskQueueData_t *) rbuf->arg);
    submitblocked =  rf_SubmitReconBuffer(rbuf, 0, 0);
    Dprintf1("RECON: submitblocked=%d\n", submitblocked);
    if (!submitblocked) retcode = IssueNextReadRequest(raidPtr, frow, event->col);
    break;

  /* a write I/O has completed */
  case RF_REVENT_WRITEDONE:
    if (rf_floatingRbufDebug) {
      rf_CheckFloatingRbufCount(raidPtr, 1);
    }
    sectorsPerRU = raidPtr->Layout.sectorsPerStripeUnit * raidPtr->Layout.SUsPerRU;
    rbuf = (RF_ReconBuffer_t *) event->arg;
    rf_FreeDiskQueueData((RF_DiskQueueData_t *) rbuf->arg);
    Dprintf3("RECON: WRITEDONE EVENT: psid %d ru %d (%d %% complete)\n", 
	    rbuf->parityStripeID, rbuf->which_ru, raidPtr->reconControl[frow]->percentComplete);
    rf_ReconMapUpdate(raidPtr, raidPtr->reconControl[frow]->reconMap,
		   rbuf->failedDiskSectorOffset, rbuf->failedDiskSectorOffset + sectorsPerRU -1);
    rf_RemoveFromActiveReconTable(raidPtr, frow, rbuf->parityStripeID, rbuf->which_ru);
    
    if (rbuf->type == RF_RBUF_TYPE_FLOATING) {
      RF_LOCK_MUTEX(raidPtr->reconControl[frow]->rb_mutex);
      raidPtr->numFullReconBuffers--;
      rf_ReleaseFloatingReconBuffer(raidPtr, frow, rbuf);
      RF_UNLOCK_MUTEX(raidPtr->reconControl[frow]->rb_mutex);
    } else if (rbuf->type == RF_RBUF_TYPE_FORCED) rf_FreeReconBuffer(rbuf);
    else RF_ASSERT(0);
    break;

  case RF_REVENT_BUFCLEAR:                  /* A buffer-stall condition has been cleared */
    Dprintf2("RECON: BUFCLEAR EVENT: row %d col %d\n",frow, event->col);
    submitblocked = rf_SubmitReconBuffer(raidPtr->reconControl[frow]->perDiskInfo[event->col].rbuf, 0, (int) (long)event->arg);
    RF_ASSERT(!submitblocked);              /* we wouldn't have gotten the BUFCLEAR event if we couldn't submit */
    retcode = IssueNextReadRequest(raidPtr, frow, event->col);
    break;

  case RF_REVENT_BLOCKCLEAR:                /* A user-write reconstruction blockage has been cleared */
    DDprintf2("RECON: BLOCKCLEAR EVENT: row %d col %d\n",frow, event->col);
    retcode = TryToRead(raidPtr, frow, event->col);
    break;

  case RF_REVENT_HEADSEPCLEAR:              /* A max-head-separation reconstruction blockage has been cleared */
    Dprintf2("RECON: HEADSEPCLEAR EVENT: row %d col %d\n",frow, event->col);
    retcode = TryToRead(raidPtr, frow, event->col);
    break;

  /* a buffer has become ready to write */
  case RF_REVENT_BUFREADY:
    Dprintf2("RECON: BUFREADY EVENT: row %d col %d\n",frow, event->col);
    retcode = IssueNextWriteRequest(raidPtr, frow);
    if (rf_floatingRbufDebug) {
      rf_CheckFloatingRbufCount(raidPtr, 1);
    }
    break;

  /* we need to skip the current RU entirely because it got recon'd while we were waiting for something else to happen */
  case RF_REVENT_SKIP:
    DDprintf2("RECON: SKIP EVENT: row %d col %d\n",frow, event->col);
    retcode = IssueNextReadRequest(raidPtr, frow, event->col);
    break;

  /* a forced-reconstruction read access has completed.  Just submit the buffer */
  case RF_REVENT_FORCEDREADDONE:
    rbuf = (RF_ReconBuffer_t *) event->arg;
    rf_FreeDiskQueueData((RF_DiskQueueData_t *) rbuf->arg);
    DDprintf2("RECON: FORCEDREADDONE EVENT: row %d col %d\n",frow, event->col);
    submitblocked = rf_SubmitReconBuffer(rbuf, 1, 0);
    RF_ASSERT(!submitblocked);
    break;
    
  default:
    RF_PANIC();
  }
  rf_FreeReconEventDesc(event);
  return(retcode);
}

/*****************************************************************************************
 *
 * find the next thing that's needed on the indicated disk, and issue a read
 * request for it.  We assume that the reconstruction buffer associated with this
 * process is free to receive the data.  If reconstruction is blocked on the
 * indicated RU, we issue a blockage-release request instead of a physical disk
 * read request.  If the current disk gets too far ahead of the others, we issue
 * a head-separation wait request and return.
 *
 * ctrl->{ru_count, curPSID, diskOffset} and rbuf->failedDiskSectorOffset are
 * maintained to point the the unit we're currently accessing.  Note that this deviates
 * from the standard C idiom of having counters point to the next thing to be 
 * accessed.  This allows us to easily retry when we're blocked by head separation
 * or reconstruction-blockage events.
 *
 * returns nonzero if and only if there is nothing left unread on the indicated disk
 ****************************************************************************************/
static int IssueNextReadRequest(raidPtr, row, col)
  RF_Raid_t    *raidPtr;
  RF_RowCol_t   row;
  RF_RowCol_t   col;
{
  RF_PerDiskReconCtrl_t *ctrl = &raidPtr->reconControl[row]->perDiskInfo[col];
  RF_RaidLayout_t *layoutPtr = &raidPtr->Layout;
  RF_ReconBuffer_t *rbuf = ctrl->rbuf;
  RF_ReconUnitCount_t RUsPerPU = layoutPtr->SUsPerPU / layoutPtr->SUsPerRU;
  RF_SectorCount_t sectorsPerRU = layoutPtr->sectorsPerStripeUnit * layoutPtr->SUsPerRU;
  int do_new_check = 0, retcode = 0, status;

  /* if we are currently the slowest disk, mark that we have to do a new check */
  if (ctrl->headSepCounter <= raidPtr->reconControl[row]->minHeadSepCounter) do_new_check = 1;

  while (1) {

    ctrl->ru_count++;
    if (ctrl->ru_count < RUsPerPU) {
      ctrl->diskOffset             += sectorsPerRU;
      rbuf->failedDiskSectorOffset += sectorsPerRU;
    } else {
      ctrl->curPSID++;
      ctrl->ru_count = 0;
      /* code left over from when head-sep was based on parity stripe id */
      if (ctrl->curPSID >= raidPtr->reconControl[row]->lastPSID) {
	CheckForNewMinHeadSep(raidPtr, row, ++(ctrl->headSepCounter));
	return(1);                           /* finito! */
      }
      
      /* find the disk offsets of the start of the parity stripe on both the current disk and the failed disk.
       * skip this entire parity stripe if either disk does not appear in the indicated PS
       */
      status = ComputePSDiskOffsets(raidPtr, ctrl->curPSID, row, col, &ctrl->diskOffset, &rbuf->failedDiskSectorOffset,
				    &rbuf->spRow, &rbuf->spCol, &rbuf->spOffset);
      if (status) {
	ctrl->ru_count = RUsPerPU-1; continue;
      }
    }
    rbuf->which_ru = ctrl->ru_count;
    
    /* skip this RU if it's already been reconstructed */
    if (rf_CheckRUReconstructed(raidPtr->reconControl[row]->reconMap, rbuf->failedDiskSectorOffset)) {
      Dprintf2("Skipping psid %ld ru %d: already reconstructed\n",ctrl->curPSID,ctrl->ru_count);
      continue;
    }
    break;
  }
  ctrl->headSepCounter++;
  if (do_new_check) CheckForNewMinHeadSep(raidPtr, row, ctrl->headSepCounter);  /* update min if needed */

  
  /* at this point, we have definitely decided what to do, and we have only to see if we can actually do it now */
  rbuf->parityStripeID = ctrl->curPSID;
  rbuf->which_ru       = ctrl->ru_count;
  bzero((char *)&raidPtr->recon_tracerecs[col], sizeof(raidPtr->recon_tracerecs[col]));
  raidPtr->recon_tracerecs[col].reconacc = 1;
  RF_ETIMER_START(raidPtr->recon_tracerecs[col].recon_timer);
  retcode = TryToRead(raidPtr, row, col);
  return(retcode);
}

/* tries to issue the next read on the indicated disk.  We may be blocked by (a) the heads being too
 * far apart, or (b) recon on the indicated RU being blocked due to a write by a user thread.
 * In this case, we issue a head-sep or blockage wait request, which will cause this same routine
 * to be invoked again later when the blockage has cleared.
 */
static int TryToRead(raidPtr, row, col)
  RF_Raid_t    *raidPtr;
  RF_RowCol_t   row;
  RF_RowCol_t   col;
{
  RF_PerDiskReconCtrl_t *ctrl = &raidPtr->reconControl[row]->perDiskInfo[col];
  RF_SectorCount_t sectorsPerRU = raidPtr->Layout.sectorsPerStripeUnit * raidPtr->Layout.SUsPerRU;
  RF_StripeNum_t psid = ctrl->curPSID;
  RF_ReconUnitNum_t which_ru = ctrl->ru_count;
  RF_DiskQueueData_t *req;
  int status, created = 0;
  RF_ReconParityStripeStatus_t *pssPtr;
  
  /* if the current disk is too far ahead of the others, issue a head-separation wait and return */
  if (CheckHeadSeparation(raidPtr, ctrl, row, col, ctrl->headSepCounter, which_ru)) return(0);
  RF_LOCK_PSS_MUTEX(raidPtr, row, psid);
  pssPtr = rf_LookupRUStatus(raidPtr, raidPtr->reconControl[row]->pssTable, psid, which_ru, RF_PSS_CREATE, &created);
  
  /* if recon is blocked on the indicated parity stripe, issue a block-wait request and return.
   * this also must mark the indicated RU in the stripe as under reconstruction if not blocked.
   */
  status = CheckForcedOrBlockedReconstruction(raidPtr, pssPtr, ctrl, row, col, psid, which_ru);
  if (status == RF_PSS_RECON_BLOCKED) {
    Dprintf2("RECON: Stalling psid %ld ru %d: recon blocked\n",psid,which_ru);
    goto out;
  } else if (status == RF_PSS_FORCED_ON_WRITE) {
    rf_CauseReconEvent(raidPtr, row, col, NULL, RF_REVENT_SKIP);
    goto out;
  }

  /* make one last check to be sure that the indicated RU didn't get reconstructed while
   * we were waiting for something else to happen.  This is unfortunate in that it causes
   * us to make this check twice in the normal case.  Might want to make some attempt to
   * re-work this so that we only do this check if we've definitely blocked on one of the
   * above checks.  When this condition is detected, we may have just created a bogus
   * status entry, which we need to delete.
   */
  if (rf_CheckRUReconstructed(raidPtr->reconControl[row]->reconMap, ctrl->rbuf->failedDiskSectorOffset)) {
    Dprintf2("RECON: Skipping psid %ld ru %d: prior recon after stall\n",psid,which_ru);
    if (created) rf_PSStatusDelete(raidPtr, raidPtr->reconControl[row]->pssTable, pssPtr);
    rf_CauseReconEvent(raidPtr, row, col, NULL, RF_REVENT_SKIP);
    goto out;
  }

  /* found something to read.  issue the I/O */
  Dprintf5("RECON: Read for psid %ld on row %d col %d offset %ld buf %lx\n",
    psid, row, col, ctrl->diskOffset, ctrl->rbuf->buffer);
  RF_ETIMER_STOP(raidPtr->recon_tracerecs[col].recon_timer);
  RF_ETIMER_EVAL(raidPtr->recon_tracerecs[col].recon_timer);
  raidPtr->recon_tracerecs[col].specific.recon.recon_start_to_fetch_us =
    RF_ETIMER_VAL_US(raidPtr->recon_tracerecs[col].recon_timer);
  RF_ETIMER_START(raidPtr->recon_tracerecs[col].recon_timer);
  
  /* should be ok to use a NULL proc pointer here, all the bufs we use should be in kernel space */
  req = rf_CreateDiskQueueData(RF_IO_TYPE_READ, ctrl->diskOffset, sectorsPerRU, ctrl->rbuf->buffer, psid, which_ru,
			    ReconReadDoneProc, (void *) ctrl, NULL, &raidPtr->recon_tracerecs[col], (void *)raidPtr, 0, NULL);
  
  RF_ASSERT(req);          /* XXX -- fix this -- XXX */
  
  ctrl->rbuf->arg = (void *) req;
  rf_DiskIOEnqueue(&raidPtr->Queues[row][col], req, RF_IO_RECON_PRIORITY);
  pssPtr->issued[col] = 1;

out:
  RF_UNLOCK_PSS_MUTEX(raidPtr, row, psid);
  return(0);
}


/* given a parity stripe ID, we want to find out whether both the current disk and the
 * failed disk exist in that parity stripe.  If not, we want to skip this whole PS.
 * If so, we want to find the disk offset of the start of the PS on both the current
 * disk and the failed disk.
 *
 * this works by getting a list of disks comprising the indicated parity stripe, and
 * searching the list for the current and failed disks.  Once we've decided they both
 * exist in the parity stripe, we need to decide whether each is data or parity,
 * so that we'll know which mapping function to call to get the corresponding disk
 * offsets.
 *
 * this is kind of unpleasant, but doing it this way allows the reconstruction code
 * to use parity stripe IDs rather than physical disks address to march through the
 * failed disk, which greatly simplifies a lot of code, as well as eliminating the
 * need for a reverse-mapping function.  I also think it will execute faster, since
 * the calls to the mapping module are kept to a minimum.
 *
 * ASSUMES THAT THE STRIPE IDENTIFIER IDENTIFIES THE DISKS COMPRISING THE STRIPE
 * IN THE CORRECT ORDER
 */
static int ComputePSDiskOffsets(
  RF_Raid_t       *raidPtr,  /* raid descriptor */
  RF_StripeNum_t   psid,     /* parity stripe identifier */
  RF_RowCol_t      row,      /* row and column of disk to find the offsets for */
  RF_RowCol_t      col,
  RF_SectorNum_t  *outDiskOffset,
  RF_SectorNum_t  *outFailedDiskSectorOffset,
  RF_RowCol_t     *spRow,    /* OUT: row,col of spare unit for failed unit */
  RF_RowCol_t     *spCol,
  RF_SectorNum_t  *spOffset) /* OUT: offset into disk containing spare unit */
{
  RF_RaidLayout_t *layoutPtr  = &raidPtr->Layout;
  RF_RowCol_t fcol = raidPtr->reconControl[row]->fcol;
  RF_RaidAddr_t sosRaidAddress;                   /* start-of-stripe */
  RF_RowCol_t *diskids;
  u_int i, j, k, i_offset, j_offset;
  RF_RowCol_t prow, pcol;
  int testcol, testrow;
  RF_RowCol_t stripe;
  RF_SectorNum_t poffset;
  char i_is_parity=0, j_is_parity=0;
  RF_RowCol_t stripeWidth = layoutPtr->numDataCol + layoutPtr->numParityCol;

  /* get a listing of the disks comprising that stripe */
  sosRaidAddress = rf_ParityStripeIDToRaidAddress(layoutPtr, psid);
  (layoutPtr->map->IdentifyStripe)(raidPtr, sosRaidAddress, &diskids, &stripe);
  RF_ASSERT(diskids);

  /* reject this entire parity stripe if it does not contain the indicated disk or it does not contain the failed disk */
  if (row != stripe)
    goto skipit;
  for (i=0; i<stripeWidth; i++) {
    if (col == diskids[i])
      break;
  }
  if (i == stripeWidth)
    goto skipit;
  for (j=0; j<stripeWidth; j++) {
    if (fcol == diskids[j])
      break;
  }
  if (j == stripeWidth) {
    goto skipit;
  }

  /* find out which disk the parity is on */
  (layoutPtr->map->MapParity)(raidPtr, sosRaidAddress, &prow, &pcol, &poffset, RF_DONT_REMAP);

  /* find out if either the current RU or the failed RU is parity */
  /* also, if the parity occurs in this stripe prior to the data and/or failed col, we need to decrement i and/or j */
  for (k=0; k<stripeWidth; k++)
    if (diskids[k] == pcol)
      break;
  RF_ASSERT(k < stripeWidth);
  i_offset = i; j_offset=j;
  if (k < i) i_offset--; else if (k==i) {i_is_parity = 1; i_offset = 0;} /* set offsets to zero to disable multiply below */
  if (k < j) j_offset--; else if (k==j) {j_is_parity = 1; j_offset = 0;}
      
  /* at this point, [ij]_is_parity tells us whether the [current,failed] disk is parity at
   * the start of this RU, and, if data, "[ij]_offset" tells us how far into the stripe
   * the [current,failed] disk is.
   */

  /* call the mapping routine to get the offset into the current disk, repeat for failed disk. */
  if (i_is_parity)
    layoutPtr->map->MapParity(raidPtr, sosRaidAddress + i_offset * layoutPtr->sectorsPerStripeUnit, &testrow, &testcol, outDiskOffset, RF_DONT_REMAP);
  else
    layoutPtr->map->MapSector(raidPtr, sosRaidAddress + i_offset * layoutPtr->sectorsPerStripeUnit, &testrow, &testcol, outDiskOffset, RF_DONT_REMAP);

  RF_ASSERT(row == testrow && col == testcol);
  
  if (j_is_parity)
    layoutPtr->map->MapParity(raidPtr, sosRaidAddress + j_offset * layoutPtr->sectorsPerStripeUnit, &testrow, &testcol, outFailedDiskSectorOffset, RF_DONT_REMAP);
  else
    layoutPtr->map->MapSector(raidPtr, sosRaidAddress + j_offset * layoutPtr->sectorsPerStripeUnit, &testrow, &testcol, outFailedDiskSectorOffset, RF_DONT_REMAP);
  RF_ASSERT(row == testrow && fcol == testcol);

  /* now locate the spare unit for the failed unit */
  if (layoutPtr->map->flags & RF_DISTRIBUTE_SPARE) {
    if (j_is_parity)
      layoutPtr->map->MapParity(raidPtr, sosRaidAddress + j_offset * layoutPtr->sectorsPerStripeUnit, spRow, spCol, spOffset, RF_REMAP);
    else
      layoutPtr->map->MapSector(raidPtr, sosRaidAddress + j_offset * layoutPtr->sectorsPerStripeUnit, spRow, spCol, spOffset, RF_REMAP);
  } else {
    *spRow    = raidPtr->reconControl[row]->spareRow;
    *spCol    = raidPtr->reconControl[row]->spareCol;
    *spOffset = *outFailedDiskSectorOffset;
  }
  
  return(0);

skipit:
  Dprintf3("RECON: Skipping psid %ld: nothing needed from r%d c%d\n",
    psid, row, col);
  return(1);
}

/* this is called when a buffer has become ready to write to the replacement disk */
static int IssueNextWriteRequest(raidPtr, row)
  RF_Raid_t    *raidPtr;
  RF_RowCol_t   row;
{
  RF_RaidLayout_t *layoutPtr = &raidPtr->Layout;
  RF_SectorCount_t sectorsPerRU = layoutPtr->sectorsPerStripeUnit * layoutPtr->SUsPerRU;
  RF_RowCol_t fcol = raidPtr->reconControl[row]->fcol;
  RF_ReconBuffer_t *rbuf;
  RF_DiskQueueData_t *req;

  rbuf = rf_GetFullReconBuffer(raidPtr->reconControl[row]);
  RF_ASSERT(rbuf);                  /* there must be one available, or we wouldn't have gotten the event that sent us here */
  RF_ASSERT(rbuf->pssPtr);

  rbuf->pssPtr->writeRbuf = rbuf;
  rbuf->pssPtr = NULL;

  Dprintf7("RECON: New write (r %d c %d offs %d) for psid %ld ru %d (failed disk offset %ld) buf %lx\n",
    rbuf->spRow, rbuf->spCol, rbuf->spOffset, rbuf->parityStripeID,
    rbuf->which_ru, rbuf->failedDiskSectorOffset, rbuf->buffer);
  Dprintf6("RECON: new write psid %ld   %02x %02x %02x %02x %02x\n",
   rbuf->parityStripeID, rbuf->buffer[0]&0xff, rbuf->buffer[1]&0xff,
   rbuf->buffer[2]&0xff, rbuf->buffer[3]&0xff, rbuf->buffer[4]&0xff);

  /* should be ok to use a NULL b_proc here b/c all addrs should be in kernel space */
  req = rf_CreateDiskQueueData(RF_IO_TYPE_WRITE, rbuf->spOffset, 
			       sectorsPerRU, rbuf->buffer, 
			       rbuf->parityStripeID, rbuf->which_ru,
			       ReconWriteDoneProc, (void *) rbuf, NULL, 
			       &raidPtr->recon_tracerecs[fcol], 
			       (void *)raidPtr, 0, NULL);

  RF_ASSERT(req);          /* XXX -- fix this -- XXX */
  
  rbuf->arg = (void *) req;
  rf_DiskIOEnqueue(&raidPtr->Queues[rbuf->spRow][rbuf->spCol], req, RF_IO_RECON_PRIORITY);

  return(0);
}

/* this gets called upon the completion of a reconstruction read operation
 * the arg is a pointer to the per-disk reconstruction control structure
 * for the process that just finished a read.
 *
 * called at interrupt context in the kernel, so don't do anything illegal here.
 */
static int ReconReadDoneProc(arg, status)
  void  *arg;
  int    status;
{
  RF_PerDiskReconCtrl_t *ctrl = (RF_PerDiskReconCtrl_t *) arg;
  RF_Raid_t *raidPtr = ctrl->reconCtrl->reconDesc->raidPtr;

  if (status) {
    /*
     * XXX
     */
    printf("Recon read failed!\n");
    RF_PANIC();
  }

  RF_ETIMER_STOP(raidPtr->recon_tracerecs[ctrl->col].recon_timer);
  RF_ETIMER_EVAL(raidPtr->recon_tracerecs[ctrl->col].recon_timer);
  raidPtr->recon_tracerecs[ctrl->col].specific.recon.recon_fetch_to_return_us =
    RF_ETIMER_VAL_US(raidPtr->recon_tracerecs[ctrl->col].recon_timer);
  RF_ETIMER_START(raidPtr->recon_tracerecs[ctrl->col].recon_timer);
  
  rf_CauseReconEvent(raidPtr, ctrl->row, ctrl->col, NULL, RF_REVENT_READDONE);
  return(0);
}

/* this gets called upon the completion of a reconstruction write operation.
 * the arg is a pointer to the rbuf that was just written
 *
 * called at interrupt context in the kernel, so don't do anything illegal here.
 */
static int ReconWriteDoneProc(arg, status)
  void  *arg;
  int    status;
{
  RF_ReconBuffer_t *rbuf = (RF_ReconBuffer_t *) arg;

  Dprintf2("Reconstruction completed on psid %ld ru %d\n",rbuf->parityStripeID, rbuf->which_ru);
  if (status) {printf("Recon write failed!\n"); /*fprintf(stderr,"Recon write failed!\n");*/ RF_PANIC();}
  rf_CauseReconEvent((RF_Raid_t *) rbuf->raidPtr, rbuf->row, rbuf->col, arg, RF_REVENT_WRITEDONE);
  return(0);
}


/* computes a new minimum head sep, and wakes up anyone who needs to be woken as a result */
static void CheckForNewMinHeadSep(raidPtr, row, hsCtr)
  RF_Raid_t          *raidPtr;
  RF_RowCol_t         row;
  RF_HeadSepLimit_t   hsCtr;
{
  RF_ReconCtrl_t *reconCtrlPtr = raidPtr->reconControl[row];
  RF_HeadSepLimit_t new_min;
  RF_RowCol_t i;
  RF_CallbackDesc_t *p;
  RF_ASSERT(hsCtr >= reconCtrlPtr->minHeadSepCounter);           /* from the definition of a minimum */
  

  RF_LOCK_MUTEX(reconCtrlPtr->rb_mutex);
  
  new_min = ~ (1L<< (8*sizeof(long)-1));         /* 0x7FFF....FFF */
  for (i=0; i<raidPtr->numCol; i++) if (i != reconCtrlPtr->fcol) {
    if (reconCtrlPtr->perDiskInfo[i].headSepCounter < new_min) new_min = reconCtrlPtr->perDiskInfo[i].headSepCounter;
  }

  /* set the new minimum and wake up anyone who can now run again */
  if (new_min != reconCtrlPtr->minHeadSepCounter) {
    reconCtrlPtr->minHeadSepCounter = new_min;
    Dprintf1("RECON:  new min head pos counter val is %ld\n",new_min);
    while (reconCtrlPtr->headSepCBList) {
      if (reconCtrlPtr->headSepCBList->callbackArg.v > new_min) break;
      p = reconCtrlPtr->headSepCBList;
      reconCtrlPtr->headSepCBList = p->next;
      p->next = NULL;
      rf_CauseReconEvent(raidPtr, p->row, p->col, NULL, RF_REVENT_HEADSEPCLEAR);
      rf_FreeCallbackDesc(p);
    }
    
  }

  RF_UNLOCK_MUTEX(reconCtrlPtr->rb_mutex);
}

/* checks to see that the maximum head separation will not be violated
 * if we initiate a reconstruction I/O on the indicated disk.  Limiting the
 * maximum head separation between two disks eliminates the nasty buffer-stall
 * conditions that occur when one disk races ahead of the others and consumes
 * all of the floating recon buffers.  This code is complex and unpleasant
 * but it's necessary to avoid some very nasty, albeit fairly rare,
 * reconstruction behavior.
 *
 * returns non-zero if and only if we have to stop working on the indicated disk
 * due to a head-separation delay.
 */
static int CheckHeadSeparation(
  RF_Raid_t              *raidPtr,
  RF_PerDiskReconCtrl_t  *ctrl,
  RF_RowCol_t             row,
  RF_RowCol_t             col,
  RF_HeadSepLimit_t       hsCtr,
  RF_ReconUnitNum_t       which_ru)
{
  RF_ReconCtrl_t *reconCtrlPtr = raidPtr->reconControl[row];
  RF_CallbackDesc_t *cb, *p, *pt;
  int retval = 0, tid;
  
  /* if we're too far ahead of the slowest disk, stop working on this disk
   * until the slower ones catch up.  We do this by scheduling a wakeup callback
   * for the time when the slowest disk has caught up.  We define "caught up"
   * with 20% hysteresis, i.e. the head separation must have fallen to at most
   * 80% of the max allowable head separation before we'll wake up.
   * 
   */
  rf_get_threadid(tid);
  RF_LOCK_MUTEX(reconCtrlPtr->rb_mutex);
  if ((raidPtr->headSepLimit >= 0) &&
      ((ctrl->headSepCounter - reconCtrlPtr->minHeadSepCounter) > raidPtr->headSepLimit))
  {
    Dprintf6("[%d] RECON: head sep stall: row %d col %d hsCtr %ld minHSCtr %ld limit %ld\n",
			   tid,row,col,ctrl->headSepCounter, reconCtrlPtr->minHeadSepCounter, raidPtr->headSepLimit);
    cb = rf_AllocCallbackDesc();
    /* the minHeadSepCounter value we have to get to before we'll wake up.  build in 20% hysteresis. */
    cb->callbackArg.v  = (ctrl->headSepCounter - raidPtr->headSepLimit + raidPtr->headSepLimit/5);
    cb->row = row; cb->col = col;
    cb->next = NULL;

    /* insert this callback descriptor into the sorted list of pending head-sep callbacks */
    p = reconCtrlPtr->headSepCBList;
    if (!p) reconCtrlPtr->headSepCBList = cb;
    else if (cb->callbackArg.v < p->callbackArg.v) {
      cb->next = reconCtrlPtr->headSepCBList;
      reconCtrlPtr->headSepCBList = cb;
    }
    else {
      for (pt=p, p=p->next; p && (p->callbackArg.v < cb->callbackArg.v); pt=p,p=p->next);
      cb->next = p;
      pt->next = cb;
    }
    retval = 1;
#if RF_RECON_STATS > 0
    ctrl->reconCtrl->reconDesc->hsStallCount++;
#endif /* RF_RECON_STATS > 0 */
  }
  RF_UNLOCK_MUTEX(reconCtrlPtr->rb_mutex);

  return(retval);
}

/* checks to see if reconstruction has been either forced or blocked by a user operation.
 * if forced, we skip this RU entirely.
 * else if blocked, put ourselves on the wait list.
 * else return 0.
 *
 * ASSUMES THE PSS MUTEX IS LOCKED UPON ENTRY
 */
static int CheckForcedOrBlockedReconstruction(
  RF_Raid_t                     *raidPtr,
  RF_ReconParityStripeStatus_t  *pssPtr,
  RF_PerDiskReconCtrl_t         *ctrl,
  RF_RowCol_t                    row,
  RF_RowCol_t                    col,
  RF_StripeNum_t                 psid,
  RF_ReconUnitNum_t              which_ru)
{
  RF_CallbackDesc_t *cb;
  int retcode = 0;

  if ((pssPtr->flags & RF_PSS_FORCED_ON_READ) || (pssPtr->flags & RF_PSS_FORCED_ON_WRITE)) retcode = RF_PSS_FORCED_ON_WRITE;
  else if (pssPtr->flags & RF_PSS_RECON_BLOCKED) {
    Dprintf4("RECON: row %d col %d blocked at psid %ld ru %d\n",row, col, psid, which_ru);
    cb = rf_AllocCallbackDesc();   /* append ourselves to the blockage-wait list */
    cb->row = row; cb->col = col;
    cb->next = pssPtr->blockWaitList;
    pssPtr->blockWaitList = cb;
    retcode = RF_PSS_RECON_BLOCKED;
  }

  if (!retcode) pssPtr->flags |= RF_PSS_UNDER_RECON;    /* mark this RU as under reconstruction */
  
  return(retcode);
}

/* if reconstruction is currently ongoing for the indicated stripeID, reconstruction
 * is forced to completion and we return non-zero to indicate that the caller must
 * wait.  If not, then reconstruction is blocked on the indicated stripe and the
 * routine returns zero.  If and only if we return non-zero, we'll cause the cbFunc
 * to get invoked with the cbArg when the reconstruction has completed.
 */
int rf_ForceOrBlockRecon(raidPtr, asmap, cbFunc, cbArg)
  RF_Raid_t              *raidPtr;
  RF_AccessStripeMap_t   *asmap;
  void                  (*cbFunc)(RF_Raid_t *,void *);
  void                   *cbArg;
{
  RF_RowCol_t row = asmap->physInfo->row;               /* which row of the array we're working on */
  RF_StripeNum_t stripeID = asmap->stripeID;                    /* the stripe ID we're forcing recon on */
  RF_SectorCount_t sectorsPerRU = raidPtr->Layout.sectorsPerStripeUnit * raidPtr->Layout.SUsPerRU;   /* num sects in one RU */
  RF_ReconParityStripeStatus_t *pssPtr;                /* a pointer to the parity stripe status structure */
  RF_StripeNum_t psid; /* parity stripe id */
  RF_SectorNum_t offset, fd_offset;                        /* disk offset, failed-disk offset */
  RF_RowCol_t *diskids;
  RF_RowCol_t stripe;
  int tid;
  RF_ReconUnitNum_t which_ru; /* RU within parity stripe */
  RF_RowCol_t fcol, diskno, i;
  RF_ReconBuffer_t *new_rbuf;                               /* ptr to newly allocated rbufs */
  RF_DiskQueueData_t *req;                                  /* disk I/O req to be enqueued */
  RF_CallbackDesc_t *cb;
  int created = 0, nPromoted;

  rf_get_threadid(tid);
  psid = rf_MapStripeIDToParityStripeID(&raidPtr->Layout, stripeID, &which_ru);

  RF_LOCK_PSS_MUTEX(raidPtr, row, psid);

  pssPtr = rf_LookupRUStatus(raidPtr, raidPtr->reconControl[row]->pssTable, psid, which_ru, RF_PSS_CREATE|RF_PSS_RECON_BLOCKED, &created);

  /* if recon is not ongoing on this PS, just return */
  if (!(pssPtr->flags & RF_PSS_UNDER_RECON)) {
    RF_UNLOCK_PSS_MUTEX(raidPtr, row, psid);
    return(0);
  }

  /* otherwise, we have to wait for reconstruction to complete on this RU. */
  /* In order to avoid waiting for a potentially large number of low-priority accesses to
   * complete, we force a normal-priority (i.e. not low-priority) reconstruction
   * on this RU.
   */
  if (!(pssPtr->flags & RF_PSS_FORCED_ON_WRITE) && !(pssPtr->flags & RF_PSS_FORCED_ON_READ)) {
    DDprintf1("Forcing recon on psid %ld\n",psid);
    pssPtr->flags |= RF_PSS_FORCED_ON_WRITE;     /* mark this RU as under forced recon */
    pssPtr->flags &= ~RF_PSS_RECON_BLOCKED;      /* clear the blockage that we just set */
    fcol = raidPtr->reconControl[row]->fcol;
    
    /* get a listing of the disks comprising the indicated stripe */
    (raidPtr->Layout.map->IdentifyStripe)(raidPtr, asmap->raidAddress, &diskids, &stripe);
    RF_ASSERT(row == stripe);
    
    /* For previously issued reads, elevate them to normal priority.  If the I/O has already completed,
     * it won't be found in the queue, and hence this will be a no-op.
     * For unissued reads, allocate buffers and issue new reads.  The fact that we've set the
     * FORCED bit means that the regular recon procs will not re-issue these reqs
     */
    for (i=0; i<raidPtr->Layout.numDataCol+raidPtr->Layout.numParityCol; i++) if ( (diskno = diskids[i]) != fcol) {
      if (pssPtr->issued[diskno]) {
	nPromoted = rf_DiskIOPromote(&raidPtr->Queues[row][diskno], psid, which_ru);
	if (rf_reconDebug && nPromoted) printf("[%d] promoted read from row %d col %d\n",tid,row,diskno);
      } else {
	new_rbuf = rf_MakeReconBuffer(raidPtr, row, diskno, RF_RBUF_TYPE_FORCED);              /* create new buf */
	ComputePSDiskOffsets(raidPtr, psid, row, diskno, &offset, &fd_offset,
			     &new_rbuf->spRow, &new_rbuf->spCol, &new_rbuf->spOffset);   /* find offsets & spare location */
	new_rbuf->parityStripeID = psid;                                                 /* fill in the buffer */
	new_rbuf->which_ru = which_ru;
	new_rbuf->failedDiskSectorOffset = fd_offset;
	new_rbuf->priority = RF_IO_NORMAL_PRIORITY;

	/* use NULL b_proc b/c all addrs should be in kernel space */
	req = rf_CreateDiskQueueData(RF_IO_TYPE_READ, offset + which_ru * sectorsPerRU, sectorsPerRU, new_rbuf->buffer,
				  psid, which_ru, (int (*)(void *, int))ForceReconReadDoneProc, (void *) new_rbuf, NULL,
				  NULL,(void *)raidPtr, 0, NULL);

	RF_ASSERT(req);          /* XXX -- fix this -- XXX */

	new_rbuf->arg = req;
	rf_DiskIOEnqueue(&raidPtr->Queues[row][diskno], req, RF_IO_NORMAL_PRIORITY);     /* enqueue the I/O */
	Dprintf3("[%d] Issued new read req on row %d col %d\n",tid,row,diskno);
      }
    }

    /* if the write is sitting in the disk queue, elevate its priority */
    if (rf_DiskIOPromote(&raidPtr->Queues[row][fcol], psid, which_ru)) printf("[%d] promoted write to row %d col %d\n",tid,row,fcol);
  }
  
  /* install a callback descriptor to be invoked when recon completes on this parity stripe. */
  cb = rf_AllocCallbackDesc();
  /* XXX the following is bogus.. These functions don't really match!!  GO */
  cb->callbackFunc = (void (*)(RF_CBParam_t))cbFunc;
  cb->callbackArg.p = (void *) cbArg;
  cb->next = pssPtr->procWaitList;
  pssPtr->procWaitList = cb;
  DDprintf2("[%d] Waiting for forced recon on psid %ld\n",tid,psid);
  
  RF_UNLOCK_PSS_MUTEX(raidPtr, row, psid);
  return(1);
}

/* called upon the completion of a forced reconstruction read.
 * all we do is schedule the FORCEDREADONE event.
 * called at interrupt context in the kernel, so don't do anything illegal here.
 */
static void ForceReconReadDoneProc(arg, status)
  void  *arg;
  int    status;
{
  RF_ReconBuffer_t *rbuf = arg;

  if (status) {printf("Forced recon read failed!\n"); /*fprintf(stderr,"Forced recon read failed!\n");*/ RF_PANIC();}
  rf_CauseReconEvent((RF_Raid_t *) rbuf->raidPtr, rbuf->row, rbuf->col, (void *) rbuf, RF_REVENT_FORCEDREADDONE);
}

/* releases a block on the reconstruction of the indicated stripe */
int rf_UnblockRecon(raidPtr, asmap)
  RF_Raid_t             *raidPtr;
  RF_AccessStripeMap_t  *asmap;
{
  RF_RowCol_t row = asmap->origRow;
  RF_StripeNum_t stripeID = asmap->stripeID;
  RF_ReconParityStripeStatus_t *pssPtr;
  RF_ReconUnitNum_t which_ru;
  RF_StripeNum_t psid;
  int tid, created = 0;
  RF_CallbackDesc_t *cb;

  rf_get_threadid(tid);
  psid = rf_MapStripeIDToParityStripeID(&raidPtr->Layout, stripeID, &which_ru);
  RF_LOCK_PSS_MUTEX( raidPtr, row, psid);
  pssPtr = rf_LookupRUStatus(raidPtr, raidPtr->reconControl[row]->pssTable, psid, which_ru, RF_PSS_NONE, &created);

  /* When recon is forced, the pss desc can get deleted before we get back to unblock recon.
   * But, this can _only_ happen when recon is forced.
   * It would be good to put some kind of sanity check here, but how to decide if recon
   * was just forced or not?
   */
  if (!pssPtr) {
    /*printf("Warning: no pss descriptor upon unblock on psid %ld RU %d\n",psid,which_ru);*/
    if (rf_reconDebug || rf_pssDebug) printf("Warning: no pss descriptor upon unblock on psid %ld RU %d\n",(long)psid,which_ru);
    goto out;
  }

  pssPtr->blockCount--;
  Dprintf3("[%d] unblocking recon on psid %ld: blockcount is %d\n",tid,psid,pssPtr->blockCount);
  if (pssPtr->blockCount == 0) {     /* if recon blockage has been released */

    /* unblock recon before calling CauseReconEvent in case CauseReconEvent causes us to
     * try to issue a new read before returning here.
     */
    pssPtr->flags &= ~RF_PSS_RECON_BLOCKED;

    
    while (pssPtr->blockWaitList) {  /* spin through the block-wait list and release all the waiters */
      cb = pssPtr->blockWaitList;
      pssPtr->blockWaitList = cb->next;
      cb->next = NULL;
      rf_CauseReconEvent(raidPtr, cb->row, cb->col, NULL, RF_REVENT_BLOCKCLEAR);
      rf_FreeCallbackDesc(cb);
    }
    if (!(pssPtr->flags & RF_PSS_UNDER_RECON)) {     /* if no recon was requested while recon was blocked */
      rf_PSStatusDelete(raidPtr, raidPtr->reconControl[row]->pssTable, pssPtr);
    }
  }
  
out:
  RF_UNLOCK_PSS_MUTEX( raidPtr, row, psid );
  return(0);
}
