/*	$OpenBSD: rf_reconutil.h,v 1.1 1999/01/11 14:29:48 niklas Exp $	*/
/*	$NetBSD: rf_reconutil.h,v 1.1 1998/11/13 04:20:34 oster Exp $	*/
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
 * rf_reconutil.h -- header file for reconstruction utilities
 ************************************************************/

/* :  
 * Log: rf_reconutil.h,v 
 * Revision 1.10  1996/07/15 05:40:41  jimz
 * some recon datastructure cleanup
 * better handling of multiple failures
 * added undocumented double-recon test
 *
 * Revision 1.9  1996/07/13  00:00:59  jimz
 * sanitized generalized reconstruction architecture
 * cleaned up head sep, rbuf problems
 *
 * Revision 1.8  1996/06/07  21:33:04  jimz
 * begin using consistent types for sector numbers,
 * stripe numbers, row+col numbers, recon unit numbers
 *
 * Revision 1.7  1996/06/05  18:06:02  jimz
 * Major code cleanup. The Great Renaming is now done.
 * Better modularity. Better typing. Fixed a bunch of
 * synchronization bugs. Made a lot of global stuff
 * per-desc or per-array. Removed dead code.
 *
 * Revision 1.6  1996/06/03  23:28:26  jimz
 * more bugfixes
 * check in tree to sync for IPDS runs with current bugfixes
 * there still may be a problem with threads in the script test
 * getting I/Os stuck- not trivially reproducible (runs ~50 times
 * in a row without getting stuck)
 *
 * Revision 1.5  1996/05/27  18:56:37  jimz
 * more code cleanup
 * better typing
 * compiles in all 3 environments
 *
 * Revision 1.4  1996/05/23  21:46:35  jimz
 * checkpoint in code cleanup (release prep)
 * lots of types, function names have been fixed
 *
 * Revision 1.3  1996/05/18  19:51:34  jimz
 * major code cleanup- fix syntax, make some types consistent,
 * add prototypes, clean out dead code, et cetera
 *
 * Revision 1.2  1995/12/06  15:06:47  root
 * added copyright info
 *
 */

#ifndef _RF__RF_RECONUTIL_H_
#define _RF__RF_RECONUTIL_H_

#include "rf_types.h"
#include "rf_reconstruct.h"

RF_ReconCtrl_t *rf_MakeReconControl(RF_RaidReconDesc_t *reconDesc,
	RF_RowCol_t frow, RF_RowCol_t fcol, RF_RowCol_t srow, RF_RowCol_t scol);
void rf_FreeReconControl(RF_Raid_t *raidPtr, RF_RowCol_t row);
RF_HeadSepLimit_t rf_GetDefaultHeadSepLimit(RF_Raid_t *raidPtr);
int rf_GetDefaultNumFloatingReconBuffers(RF_Raid_t *raidPtr);
RF_ReconBuffer_t *rf_MakeReconBuffer(RF_Raid_t *raidPtr, RF_RowCol_t row,
	RF_RowCol_t col, RF_RbufType_t type);
void rf_FreeReconBuffer(RF_ReconBuffer_t *rbuf);
void rf_CheckFloatingRbufCount(RF_Raid_t *raidPtr, int dolock);

#endif /* !_RF__RF_RECONUTIL_H_ */
