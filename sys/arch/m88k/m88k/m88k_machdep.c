/*	$OpenBSD: m88k_machdep.c,v 1.1 2004/09/30 14:55:54 miod Exp $	*/
/*
 * Copyright (c) 1998, 1999, 2000, 2001 Steve Murphree, Jr.
 * Copyright (c) 1996 Nivas Madhur
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Nivas Madhur.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * Mach Operating System
 * Copyright (c) 1993-1991 Carnegie Mellon University
 * Copyright (c) 1991 OMRON Corporation
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/msgbuf.h>
#include <sys/exec.h>
#include <sys/errno.h>

#include <machine/cmmu.h>
#include <machine/cpu.h>
#include <machine/cpu_number.h>
/*
#include <machine/locore.h>
*/
#include <machine/reg.h>
#ifdef M88100
#include <machine/m88100.h>
#endif

#include <uvm/uvm_extern.h>

#if DDB
#include <machine/db_machdep.h>
#include <ddb/db_extern.h>
#include <ddb/db_interface.h>
#endif /* DDB */

/* prototypes */
void regdump(struct trapframe *f);
void dumpsys(void);
void save_u_area(struct proc *, vaddr_t);
void load_u_area(struct proc *);
void dumpconf(void);

int longformat = 1;  /* for regdump() */

/*
 * safepri is a safe priority for sleep to set for a spin-wait
 * during autoconfiguration or after a panic.
 */
int   safepri = IPL_NONE;

/*
 * Setup u area ptes for u area double mapping.
 */

void
save_u_area(struct proc *p, vaddr_t va)
{
	int i;

	for (i = 0; i < UPAGES; i++) {
		p->p_md.md_upte[i] = *((pt_entry_t *)kvtopte(va));
		va += PAGE_SIZE;
	}
}

void
load_u_area(struct proc *p)
{
	int i;
	vaddr_t va;
	pt_entry_t *t;

	for (i = 0, va = UADDR; i < UPAGES; i++) {
		t = kvtopte(va);
		*t = p->p_md.md_upte[i];
		va += PAGE_SIZE;
	}
	cmmu_flush_tlb(cpu_number(), 1, UADDR, USPACE);
}

/*
 * Set registers on exec.
 * Clear all except sp and pc.
 */
void
setregs(p, pack, stack, retval)
	struct proc *p;
	struct exec_package *pack;
	u_long stack;
	int retval[2];
{
	struct trapframe *tf = (struct trapframe *)USER_REGS(p);

	/*
	 * The syscall will ``return'' to snip; set it.
	 * argc, argv, envp are placed on the stack by copyregs.
	 * Point r2 to the stack. crt0 should extract envp from
	 * argc & argv before calling user's main.
	 */
#if 0
	/*
	 * I don't think I need to mess with fpstate on 88k because
	 * we make sure the floating point pipeline is drained in
	 * the trap handlers. Should check on this later. XXX Nivas.
	 */
	if ((fs = p->p_md.md_fpstate) != NULL) {
		/*
		 * We hold an FPU state.  If we own *the* FPU chip state
		 * we must get rid of it, and the only way to do that is
		 * to save it.  In any case, get rid of our FPU state.
		 */
		if (p == fpproc) {
			savefpstate(fs);
			fpproc = NULL;
		}
		free((void *)fs, M_SUBPROC);
		p->p_md.md_fpstate = NULL;
	}
#endif

	bzero((caddr_t)tf, sizeof *tf);

	if (cputyp == CPU_88110) {
#ifdef M88110
		/*
		 * user mode, serialize mem, interrupts enabled,
		 * graphics unit, fp enabled
		 */
		tf->tf_epsr = PSR_SRM | PSR_SFD;
		/*
		 * XXX disable OoO for now...
		 */
		tf->tf_epsr |= PSR_SER;
#endif
	} else {
#ifdef M88100
		/*
		 * user mode, interrupts enabled,
		 * no graphics unit, fp enabled
		 */
		tf->tf_epsr = PSR_SFD | PSR_SFD2;
#endif
	}

	/*
	 * We want to start executing at pack->ep_entry. The way to
	 * do this is force the processor to fetch from ep_entry. Set
	 * NIP to something bogus and invalid so that it will be a NOOP.
	 * And set sfip to ep_entry with valid bit on so that it will be
	 * fetched.  mc88110 - just set exip to pack->ep_entry.
	 */
	if (cputyp == CPU_88110) {
#ifdef M88110
		tf->tf_exip = pack->ep_entry & ~3;
#endif
	} else {
#ifdef M88100
		tf->tf_snip = pack->ep_entry & ~3;
		tf->tf_sfip = (pack->ep_entry & ~3) | FIP_V;
#endif
	}
	tf->tf_r[2] = stack;
	tf->tf_r[31] = stack;
	retval[1] = 0;
}

int
copystr(fromaddr, toaddr, maxlength, lencopied)
	const void *fromaddr;
	void *toaddr;
	size_t maxlength;
	size_t *lencopied;
{
	u_int tally;

	tally = 0;

	while (maxlength--) {
		*(u_char *)toaddr = *(u_char *)fromaddr++;
		tally++;
		if (*(u_char *)toaddr++ == 0) {
			if (lencopied) *lencopied = tally;
			return (0);
		}
	}

	if (lencopied)
		*lencopied = tally;

	return (ENAMETOOLONG);
}

void
setrunqueue(p)
	struct proc *p;
{
	struct prochd *q;
	struct proc *oldlast;
	int which = p->p_priority >> 2;

	if (p->p_back != NULL)
		panic("setrunqueue %p", p);
	q = &qs[which];
	whichqs |= 1 << which;
	p->p_forw = (struct proc *)q;
	p->p_back = oldlast = q->ph_rlink;
	q->ph_rlink = p;
	oldlast->p_forw = p;
}

/*
 * Remove process p from its run queue, which should be the one
 * indicated by its priority.  Calls should be made at splstatclock().
 */
void
remrunqueue(vp)
	struct proc *vp;
{
	struct proc *p = vp;
	int which = p->p_priority >> 2;
	struct prochd *q;

	if ((whichqs & (1 << which)) == 0)
		panic("remrq %p", p);
	p->p_forw->p_back = p->p_back;
	p->p_back->p_forw = p->p_forw;
	p->p_back = NULL;
	q = &qs[which];
	if (q->ph_link == (struct proc *)q)
		whichqs &= ~(1 << which);
}

void
nmihand(void *framep)
{
#if 0
	struct trapframe *frame = framep;
#endif

#if DDB
	printf("Abort Pressed\n");
	Debugger();
#else
	printf("Spurious NMI?\n");
#endif /* DDB */
}

void
regdump(struct trapframe *f)
{
#define R(i) f->tf_r[i]
	printf("R00-05: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       R(0),R(1),R(2),R(3),R(4),R(5));
	printf("R06-11: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       R(6),R(7),R(8),R(9),R(10),R(11));
	printf("R12-17: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       R(12),R(13),R(14),R(15),R(16),R(17));
	printf("R18-23: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       R(18),R(19),R(20),R(21),R(22),R(23));
	printf("R24-29: 0x%08x  0x%08x  0x%08x  0x%08x  0x%08x  0x%08x\n",
	       R(24),R(25),R(26),R(27),R(28),R(29));
	printf("R30-31: 0x%08x  0x%08x\n",R(30),R(31));
	if (cputyp == CPU_88110) {
		printf("exip %x enip %x\n", f->tf_exip, f->tf_enip);
	} else {
		printf("sxip %x snip %x sfip %x\n",
		    f->tf_sxip, f->tf_snip, f->tf_sfip);
	}
#ifdef M88100
	if (f->tf_vector == 0x3 && cputyp != CPU_88110) {
		/* print dmt stuff for data access fault */
		printf("dmt0 %x dmd0 %x dma0 %x\n",
		    f->tf_dmt0, f->tf_dmd0, f->tf_dma0);
		printf("dmt1 %x dmd1 %x dma1 %x\n",
		    f->tf_dmt1, f->tf_dmd1, f->tf_dma1);
		printf("dmt2 %x dmd2 %x dma2 %x\n",
		    f->tf_dmt2, f->tf_dmd2, f->tf_dma2);
		printf("fault type %d\n", (f->tf_dpfsr >> 16) & 0x7);
		dae_print((unsigned *)f);
	}
	if (longformat && cputyp != CPU_88110) {
		printf("fpsr %x fpcr %x epsr %x ssbr %x\n",
		    f->tf_fpsr, f->tf_fpcr, f->tf_epsr, f->tf_ssbr);
		printf("fpecr %x fphs1 %x fpls1 %x fphs2 %x fpls2 %x\n",
		    f->tf_fpecr, f->tf_fphs1, f->tf_fpls1,
		    f->tf_fphs2, f->tf_fpls2);
		printf("fppt %x fprh %x fprl %x fpit %x\n",
		    f->tf_fppt, f->tf_fprh, f->tf_fprl, f->tf_fpit);
		printf("vector %d mask %x mode %x scratch1 %x cpu %x\n",
		    f->tf_vector, f->tf_mask, f->tf_mode,
		    f->tf_scratch1, f->tf_cpu);
	}
#endif
#ifdef M88110
	if (longformat && cputyp == CPU_88110) {
		printf("fpsr %x fpcr %x fpecr %x epsr %x\n",
		    f->tf_fpsr, f->tf_fpcr, f->tf_fpecr, f->tf_epsr);
		printf("dsap %x duap %x dsr %x dlar %x dpar %x\n",
		    f->tf_dsap, f->tf_duap, f->tf_dsr, f->tf_dlar, f->tf_dpar);
		printf("isap %x iuap %x isr %x ilar %x ipar %x\n",
		    f->tf_isap, f->tf_iuap, f->tf_isr, f->tf_ilar, f->tf_ipar);
		printf("vector %d mask %x mode %x scratch1 %x cpu %x\n",
		    f->tf_vector, f->tf_mask, f->tf_mode,
		    f->tf_scratch1, f->tf_cpu);
	}
#endif
}
