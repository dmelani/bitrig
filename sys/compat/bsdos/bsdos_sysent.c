/*	$OpenBSD: bsdos_sysent.c,v 1.19 2007/11/27 18:05:59 art Exp $	*/

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	OpenBSD: syscalls.master,v 1.17 2006/09/22 15:29:57 pedro Exp 
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/bsdos/bsdos_syscallargs.h>

#define	s(type)	sizeof(type)

struct sysent bsdos_sysent[] = {
	{ 0, 0, 0,
	    sys_nosys },			/* 0 = syscall */
	{ 1, s(struct sys_exit_args), 0,
	    sys_exit },				/* 1 = exit */
	{ 0, 0, 0,
	    sys_fork },				/* 2 = fork */
	{ 3, s(struct sys_read_args), 0,
	    sys_read },				/* 3 = read */
	{ 3, s(struct sys_write_args), 0,
	    sys_write },			/* 4 = write */
	{ 3, s(struct sys_open_args), 0,
	    sys_open },				/* 5 = open */
	{ 1, s(struct sys_close_args), 0,
	    sys_close },			/* 6 = close */
	{ 4, s(struct sys_wait4_args), 0,
	    sys_wait4 },			/* 7 = wait4 */
	{ 2, s(struct compat_43_sys_creat_args), 0,
	    compat_43_sys_creat },		/* 8 = ocreat */
	{ 2, s(struct sys_link_args), 0,
	    sys_link },				/* 9 = link */
	{ 1, s(struct sys_unlink_args), 0,
	    sys_unlink },			/* 10 = unlink */
	{ 0, 0, 0,
	    sys_nosys },			/* 11 = obsolete execv */
	{ 1, s(struct sys_chdir_args), 0,
	    sys_chdir },			/* 12 = chdir */
	{ 1, s(struct sys_fchdir_args), 0,
	    sys_fchdir },			/* 13 = fchdir */
	{ 3, s(struct sys_mknod_args), 0,
	    sys_mknod },			/* 14 = mknod */
	{ 2, s(struct sys_chmod_args), 0,
	    sys_chmod },			/* 15 = chmod */
	{ 3, s(struct sys_chown_args), 0,
	    sys_chown },			/* 16 = chown */
	{ 1, s(struct sys_obreak_args), 0,
	    sys_obreak },			/* 17 = break */
	{ 3, s(struct compat_25_sys_getfsstat_args), 0,
	    compat_25_sys_getfsstat },		/* 18 = getfsstat */
	{ 3, s(struct compat_43_sys_lseek_args), 0,
	    compat_43_sys_lseek },		/* 19 = olseek */
	{ 0, 0, 0,
	    sys_getpid },			/* 20 = getpid */
	{ 4, s(struct sys_mount_args), 0,
	    sys_mount },			/* 21 = mount */
	{ 2, s(struct sys_unmount_args), 0,
	    sys_unmount },			/* 22 = unmount */
	{ 1, s(struct sys_setuid_args), 0,
	    sys_setuid },			/* 23 = setuid */
	{ 0, 0, 0,
	    sys_getuid },			/* 24 = getuid */
	{ 0, 0, 0,
	    sys_geteuid },			/* 25 = geteuid */
#ifdef PTRACE
	{ 4, s(struct sys_ptrace_args), 0,
	    sys_ptrace },			/* 26 = ptrace */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 26 = unimplemented ptrace */
#endif
	{ 3, s(struct sys_recvmsg_args), 0,
	    sys_recvmsg },			/* 27 = recvmsg */
	{ 3, s(struct sys_sendmsg_args), 0,
	    sys_sendmsg },			/* 28 = sendmsg */
	{ 6, s(struct sys_recvfrom_args), 0,
	    sys_recvfrom },			/* 29 = recvfrom */
	{ 3, s(struct sys_accept_args), 0,
	    sys_accept },			/* 30 = accept */
	{ 3, s(struct sys_getpeername_args), 0,
	    sys_getpeername },			/* 31 = getpeername */
	{ 3, s(struct sys_getsockname_args), 0,
	    sys_getsockname },			/* 32 = getsockname */
	{ 2, s(struct sys_access_args), 0,
	    sys_access },			/* 33 = access */
	{ 2, s(struct sys_chflags_args), 0,
	    sys_chflags },			/* 34 = chflags */
	{ 2, s(struct sys_fchflags_args), 0,
	    sys_fchflags },			/* 35 = fchflags */
	{ 0, 0, 0,
	    sys_sync },				/* 36 = sync */
	{ 2, s(struct sys_kill_args), 0,
	    sys_kill },				/* 37 = kill */
	{ 2, s(struct compat_43_sys_stat_args), 0,
	    compat_43_sys_stat },		/* 38 = stat43 */
	{ 0, 0, 0,
	    sys_getppid },			/* 39 = getppid */
	{ 2, s(struct compat_43_sys_lstat_args), 0,
	    compat_43_sys_lstat },		/* 40 = lstat43 */
	{ 1, s(struct sys_dup_args), 0,
	    sys_dup },				/* 41 = dup */
	{ 0, 0, 0,
	    sys_opipe },			/* 42 = opipe */
	{ 0, 0, 0,
	    sys_getegid },			/* 43 = getegid */
	{ 4, s(struct sys_profil_args), 0,
	    sys_profil },			/* 44 = profil */
#ifdef KTRACE
	{ 4, s(struct sys_ktrace_args), 0,
	    sys_ktrace },			/* 45 = ktrace */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 45 = unimplemented ktrace */
#endif
	{ 3, s(struct sys_sigaction_args), 0,
	    sys_sigaction },			/* 46 = sigaction */
	{ 0, 0, 0,
	    sys_getgid },			/* 47 = getgid */
	{ 2, s(struct sys_sigprocmask_args), 0,
	    sys_sigprocmask },			/* 48 = sigprocmask */
	{ 2, s(struct sys_getlogin_args), 0,
	    sys_getlogin },			/* 49 = getlogin */
	{ 1, s(struct sys_setlogin_args), 0,
	    sys_setlogin },			/* 50 = setlogin */
#ifdef ACCOUNTING
	{ 1, s(struct sys_acct_args), 0,
	    sys_acct },				/* 51 = acct */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 51 = unimplemented acct */
#endif
	{ 0, 0, 0,
	    sys_sigpending },			/* 52 = sigpending */
	{ 2, s(struct sys_osigaltstack_args), 0,
	    sys_osigaltstack },			/* 53 = osigaltstack */
	{ 3, s(struct bsdos_sys_ioctl_args), 0,
	    bsdos_sys_ioctl },			/* 54 = ioctl */
	{ 1, s(struct sys_reboot_args), 0,
	    sys_reboot },			/* 55 = reboot */
	{ 1, s(struct sys_revoke_args), 0,
	    sys_revoke },			/* 56 = revoke */
	{ 2, s(struct sys_symlink_args), 0,
	    sys_symlink },			/* 57 = symlink */
	{ 3, s(struct sys_readlink_args), 0,
	    sys_readlink },			/* 58 = readlink */
	{ 3, s(struct sys_execve_args), 0,
	    sys_execve },			/* 59 = execve */
	{ 1, s(struct sys_umask_args), 0,
	    sys_umask },			/* 60 = umask */
	{ 1, s(struct sys_chroot_args), 0,
	    sys_chroot },			/* 61 = chroot */
	{ 2, s(struct compat_43_sys_fstat_args), 0,
	    compat_43_sys_fstat },		/* 62 = fstat43 */
	{ 4, s(struct compat_43_sys_getkerninfo_args), 0,
	    compat_43_sys_getkerninfo },	/* 63 = ogetkerninfo */
	{ 0, 0, 0,
	    compat_43_sys_getpagesize },	/* 64 = ogetpagesize */
	{ 3, s(struct sys_msync_args), 0,
	    sys_msync },			/* 65 = msync */
	{ 0, 0, 0,
	    sys_vfork },			/* 66 = vfork */
	{ 0, 0, 0,
	    sys_nosys },			/* 67 = obsolete vread */
	{ 0, 0, 0,
	    sys_nosys },			/* 68 = obsolete vwrite */
	{ 1, s(struct sys_sbrk_args), 0,
	    sys_sbrk },				/* 69 = sbrk */
	{ 1, s(struct sys_sstk_args), 0,
	    sys_sstk },				/* 70 = sstk */
	{ 6, s(struct compat_43_sys_mmap_args), 0,
	    compat_43_sys_mmap },		/* 71 = ommap */
	{ 1, s(struct sys_ovadvise_args), 0,
	    sys_ovadvise },			/* 72 = vadvise */
	{ 2, s(struct sys_munmap_args), 0,
	    sys_munmap },			/* 73 = munmap */
	{ 3, s(struct sys_mprotect_args), 0,
	    sys_mprotect },			/* 74 = mprotect */
	{ 3, s(struct sys_madvise_args), 0,
	    sys_madvise },			/* 75 = madvise */
	{ 0, 0, 0,
	    sys_nosys },			/* 76 = obsolete vhangup */
	{ 0, 0, 0,
	    sys_nosys },			/* 77 = obsolete vlimit */
	{ 3, s(struct sys_mincore_args), 0,
	    sys_mincore },			/* 78 = mincore */
	{ 2, s(struct sys_getgroups_args), 0,
	    sys_getgroups },			/* 79 = getgroups */
	{ 2, s(struct sys_setgroups_args), 0,
	    sys_setgroups },			/* 80 = setgroups */
	{ 0, 0, 0,
	    sys_getpgrp },			/* 81 = getpgrp */
	{ 2, s(struct sys_setpgid_args), 0,
	    sys_setpgid },			/* 82 = setpgid */
	{ 3, s(struct sys_setitimer_args), 0,
	    sys_setitimer },			/* 83 = setitimer */
	{ 0, 0, 0,
	    compat_43_sys_wait },		/* 84 = owait */
	{ 1, s(struct compat_25_sys_swapon_args), 0,
	    compat_25_sys_swapon },		/* 85 = swapon */
	{ 2, s(struct sys_getitimer_args), 0,
	    sys_getitimer },			/* 86 = getitimer */
	{ 2, s(struct compat_43_sys_gethostname_args), 0,
	    compat_43_sys_gethostname },	/* 87 = ogethostname */
	{ 2, s(struct compat_43_sys_sethostname_args), 0,
	    compat_43_sys_sethostname },	/* 88 = osethostname */
	{ 0, 0, 0,
	    compat_43_sys_getdtablesize },	/* 89 = ogetdtablesize */
	{ 2, s(struct sys_dup2_args), 0,
	    sys_dup2 },				/* 90 = dup2 */
	{ 0, 0, 0,
	    sys_nosys },			/* 91 = unimplemented getdopt */
	{ 3, s(struct sys_fcntl_args), 0,
	    sys_fcntl },			/* 92 = fcntl */
	{ 5, s(struct sys_select_args), 0,
	    sys_select },			/* 93 = select */
	{ 0, 0, 0,
	    sys_nosys },			/* 94 = unimplemented setdopt */
	{ 1, s(struct sys_fsync_args), 0,
	    sys_fsync },			/* 95 = fsync */
	{ 3, s(struct sys_setpriority_args), 0,
	    sys_setpriority },			/* 96 = setpriority */
	{ 3, s(struct sys_socket_args), 0,
	    sys_socket },			/* 97 = socket */
	{ 3, s(struct sys_connect_args), 0,
	    sys_connect },			/* 98 = connect */
	{ 3, s(struct compat_43_sys_accept_args), 0,
	    compat_43_sys_accept },		/* 99 = oaccept */
	{ 2, s(struct sys_getpriority_args), 0,
	    sys_getpriority },			/* 100 = getpriority */
	{ 4, s(struct compat_43_sys_send_args), 0,
	    compat_43_sys_send },		/* 101 = osend */
	{ 4, s(struct compat_43_sys_recv_args), 0,
	    compat_43_sys_recv },		/* 102 = orecv */
	{ 1, s(struct sys_sigreturn_args), 0,
	    sys_sigreturn },			/* 103 = sigreturn */
	{ 3, s(struct sys_bind_args), 0,
	    sys_bind },				/* 104 = bind */
	{ 5, s(struct sys_setsockopt_args), 0,
	    sys_setsockopt },			/* 105 = setsockopt */
	{ 2, s(struct sys_listen_args), 0,
	    sys_listen },			/* 106 = listen */
	{ 0, 0, 0,
	    sys_nosys },			/* 107 = obsolete vtimes */
	{ 3, s(struct compat_43_sys_sigvec_args), 0,
	    compat_43_sys_sigvec },		/* 108 = osigvec */
	{ 1, s(struct compat_43_sys_sigblock_args), 0,
	    compat_43_sys_sigblock },		/* 109 = osigblock */
	{ 1, s(struct compat_43_sys_sigsetmask_args), 0,
	    compat_43_sys_sigsetmask },		/* 110 = osigsetmask */
	{ 1, s(struct sys_sigsuspend_args), 0,
	    sys_sigsuspend },			/* 111 = sigsuspend */
	{ 2, s(struct compat_43_sys_sigstack_args), 0,
	    compat_43_sys_sigstack },		/* 112 = osigstack */
	{ 3, s(struct compat_43_sys_recvmsg_args), 0,
	    compat_43_sys_recvmsg },		/* 113 = orecvmsg */
	{ 3, s(struct compat_43_sys_sendmsg_args), 0,
	    compat_43_sys_sendmsg },		/* 114 = osendmsg */
#ifdef TRACE
	{ 2, s(struct sys_vtrace_args), 0,
	    sys_vtrace },			/* 115 = vtrace */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 115 = obsolete vtrace */
#endif
	{ 2, s(struct sys_gettimeofday_args), 0,
	    sys_gettimeofday },			/* 116 = gettimeofday */
	{ 2, s(struct sys_getrusage_args), 0,
	    sys_getrusage },			/* 117 = getrusage */
	{ 5, s(struct sys_getsockopt_args), 0,
	    sys_getsockopt },			/* 118 = getsockopt */
	{ 0, 0, 0,
	    sys_nosys },			/* 119 = obsolete resuba */
	{ 3, s(struct sys_readv_args), 0,
	    sys_readv },			/* 120 = readv */
	{ 3, s(struct sys_writev_args), 0,
	    sys_writev },			/* 121 = writev */
	{ 2, s(struct sys_settimeofday_args), 0,
	    sys_settimeofday },			/* 122 = settimeofday */
	{ 3, s(struct sys_fchown_args), 0,
	    sys_fchown },			/* 123 = fchown */
	{ 2, s(struct sys_fchmod_args), 0,
	    sys_fchmod },			/* 124 = fchmod */
	{ 6, s(struct compat_43_sys_recvfrom_args), 0,
	    compat_43_sys_recvfrom },		/* 125 = orecvfrom */
	{ 2, s(struct sys_setreuid_args), 0,
	    sys_setreuid },			/* 126 = setreuid */
	{ 2, s(struct sys_setregid_args), 0,
	    sys_setregid },			/* 127 = setregid */
	{ 2, s(struct sys_rename_args), 0,
	    sys_rename },			/* 128 = rename */
	{ 2, s(struct compat_43_sys_truncate_args), 0,
	    compat_43_sys_truncate },		/* 129 = otruncate */
	{ 2, s(struct compat_43_sys_ftruncate_args), 0,
	    compat_43_sys_ftruncate },		/* 130 = oftruncate */
	{ 2, s(struct sys_flock_args), 0,
	    sys_flock },			/* 131 = flock */
	{ 2, s(struct sys_mkfifo_args), 0,
	    sys_mkfifo },			/* 132 = mkfifo */
	{ 6, s(struct sys_sendto_args), 0,
	    sys_sendto },			/* 133 = sendto */
	{ 2, s(struct sys_shutdown_args), 0,
	    sys_shutdown },			/* 134 = shutdown */
	{ 4, s(struct sys_socketpair_args), 0,
	    sys_socketpair },			/* 135 = socketpair */
	{ 2, s(struct sys_mkdir_args), 0,
	    sys_mkdir },			/* 136 = mkdir */
	{ 1, s(struct sys_rmdir_args), 0,
	    sys_rmdir },			/* 137 = rmdir */
	{ 2, s(struct sys_utimes_args), 0,
	    sys_utimes },			/* 138 = utimes */
	{ 0, 0, 0,
	    sys_nosys },			/* 139 = obsolete 4.2 sigreturn */
	{ 2, s(struct sys_adjtime_args), 0,
	    sys_adjtime },			/* 140 = adjtime */
	{ 3, s(struct compat_43_sys_getpeername_args), 0,
	    compat_43_sys_getpeername },	/* 141 = ogetpeername */
	{ 0, 0, 0,
	    compat_43_sys_gethostid },		/* 142 = ogethostid */
	{ 1, s(struct compat_43_sys_sethostid_args), 0,
	    compat_43_sys_sethostid },		/* 143 = osethostid */
	{ 2, s(struct compat_43_sys_getrlimit_args), 0,
	    compat_43_sys_getrlimit },		/* 144 = ogetrlimit */
	{ 2, s(struct compat_43_sys_setrlimit_args), 0,
	    compat_43_sys_setrlimit },		/* 145 = osetrlimit */
	{ 2, s(struct compat_43_sys_killpg_args), 0,
	    compat_43_sys_killpg },		/* 146 = okillpg */
	{ 0, 0, 0,
	    sys_setsid },			/* 147 = setsid */
	{ 4, s(struct sys_quotactl_args), 0,
	    sys_quotactl },			/* 148 = quotactl */
	{ 0, 0, 0,
	    compat_43_sys_quota },		/* 149 = oquota */
	{ 3, s(struct compat_43_sys_getsockname_args), 0,
	    compat_43_sys_getsockname },	/* 150 = ogetsockname */
	{ 0, 0, 0,
	    sys_nosys },			/* 151 = unimplemented sem_lock */
	{ 0, 0, 0,
	    sys_nosys },			/* 152 = unimplemented sem_wakeup */
	{ 0, 0, 0,
	    sys_nosys },			/* 153 = unimplemented asyncdaemon */
	{ 0, 0, 0,
	    sys_nosys },			/* 154 = unimplemented */
#if defined(NFSCLIENT) || defined(NFSSERVER)
	{ 2, s(struct sys_nfssvc_args), 0,
	    sys_nfssvc },			/* 155 = nfssvc */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 155 = unimplemented */
#endif
	{ 4, s(struct compat_43_sys_getdirentries_args), 0,
	    compat_43_sys_getdirentries },	/* 156 = ogetdirentries */
	{ 2, s(struct compat_25_sys_statfs_args), 0,
	    compat_25_sys_statfs },		/* 157 = statfs */
	{ 2, s(struct compat_25_sys_fstatfs_args), 0,
	    compat_25_sys_fstatfs },		/* 158 = fstatfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 159 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 160 = unimplemented */
#ifdef NFSCLIENT
	{ 2, s(struct sys_getfh_args), 0,
	    sys_getfh },			/* 161 = getfh */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 161 = unimplemented getfh */
#endif
	{ 0, 0, 0,
	    sys_nosys },			/* 162 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 163 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 164 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 165 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 166 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 167 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 168 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 169 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 170 = unimplemented */
#if defined(SYSVSHM) && !defined(alpha)
	{ 4, s(struct compat_10_sys_shmsys_args), 0,
	    compat_10_sys_shmsys },		/* 171 = shmsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 171 = unimplemented shmsys */
#endif
	{ 0, 0, 0,
	    sys_nosys },			/* 172 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 173 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 174 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 175 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 176 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 177 = unimplemented sfork */
	{ 0, 0, 0,
	    sys_nosys },			/* 178 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 179 = unimplemented getdescriptor */
	{ 0, 0, 0,
	    sys_nosys },			/* 180 = unimplemented setdescriptor */
	{ 1, s(struct sys_setgid_args), 0,
	    sys_setgid },			/* 181 = setgid */
	{ 1, s(struct sys_setegid_args), 0,
	    sys_setegid },			/* 182 = setegid */
	{ 1, s(struct sys_seteuid_args), 0,
	    sys_seteuid },			/* 183 = seteuid */
	{ 0, 0, 0,
	    sys_nosys },			/* 184 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 185 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 186 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 187 = unimplemented */
	{ 2, s(struct compat_35_sys_stat_args), 0,
	    compat_35_sys_stat },		/* 188 = stat35 */
	{ 2, s(struct compat_35_sys_fstat_args), 0,
	    compat_35_sys_fstat },		/* 189 = fstat35 */
	{ 2, s(struct compat_35_sys_lstat_args), 0,
	    compat_35_sys_lstat },		/* 190 = lstat35 */
	{ 2, s(struct sys_pathconf_args), 0,
	    sys_pathconf },			/* 191 = pathconf */
	{ 2, s(struct sys_fpathconf_args), 0,
	    sys_fpathconf },			/* 192 = fpathconf */
	{ 0, 0, 0,
	    sys_nosys },			/* 193 = unimplemented */
	{ 2, s(struct sys_getrlimit_args), 0,
	    sys_getrlimit },			/* 194 = getrlimit */
	{ 2, s(struct sys_setrlimit_args), 0,
	    sys_setrlimit },			/* 195 = setrlimit */
	{ 4, s(struct sys_getdirentries_args), 0,
	    sys_getdirentries },		/* 196 = getdirentries */
	{ 7, s(struct sys_mmap_args), 0,
	    sys_mmap },				/* 197 = mmap */
	{ 0, 0, 0,
	    sys_nosys },			/* 198 = __syscall */
	{ 4, s(struct sys_lseek_args), 0,
	    sys_lseek },			/* 199 = lseek */
	{ 3, s(struct sys_truncate_args), 0,
	    sys_truncate },			/* 200 = truncate */
	{ 3, s(struct sys_ftruncate_args), 0,
	    sys_ftruncate },			/* 201 = ftruncate */
	{ 6, s(struct sys___sysctl_args), 0,
	    sys___sysctl },			/* 202 = __sysctl */
	{ 2, s(struct sys_mlock_args), 0,
	    sys_mlock },			/* 203 = mlock */
	{ 2, s(struct sys_munlock_args), 0,
	    sys_munlock },			/* 204 = munlock */
	{ 0, 0, 0,
	    sys_nosys },			/* 205 = unimplemented sys_undelete */
	{ 0, 0, 0,
	    sys_nosys },			/* 206 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 207 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 208 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 209 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 210 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 211 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 212 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 213 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 214 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 215 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 216 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 217 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 218 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 219 = unimplemented */
#ifdef SYSVSEM
	{ 4, s(struct sys___semctl_args), 0,
	    sys___semctl },			/* 220 = __semctl */
	{ 3, s(struct sys_semget_args), 0,
	    sys_semget },			/* 221 = semget */
	{ 3, s(struct sys_semop_args), 0,
	    sys_semop },			/* 222 = semop */
	{ 0, 0, 0,
	    sys_nosys },			/* 223 = obsolete sys_semconfig */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 220 = unimplemented semctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 221 = unimplemented semget */
	{ 0, 0, 0,
	    sys_nosys },			/* 222 = unimplemented semop */
	{ 0, 0, 0,
	    sys_nosys },			/* 223 = unimplemented semconfig */
#endif
#ifdef SYSVMSG
	{ 3, s(struct sys_msgctl_args), 0,
	    sys_msgctl },			/* 224 = msgctl */
	{ 2, s(struct sys_msgget_args), 0,
	    sys_msgget },			/* 225 = msgget */
	{ 4, s(struct sys_msgsnd_args), 0,
	    sys_msgsnd },			/* 226 = msgsnd */
	{ 5, s(struct sys_msgrcv_args), 0,
	    sys_msgrcv },			/* 227 = msgrcv */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 224 = unimplemented msgctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 225 = unimplemented msgget */
	{ 0, 0, 0,
	    sys_nosys },			/* 226 = unimplemented msgsnd */
	{ 0, 0, 0,
	    sys_nosys },			/* 227 = unimplemented msgrcv */
#endif
#ifdef SYSVSHM
	{ 3, s(struct sys_shmat_args), 0,
	    sys_shmat },			/* 228 = shmat */
	{ 3, s(struct sys_shmctl_args), 0,
	    sys_shmctl },			/* 229 = shmctl */
	{ 1, s(struct sys_shmdt_args), 0,
	    sys_shmdt },			/* 230 = shmdt */
	{ 3, s(struct sys_shmget_args), 0,
	    sys_shmget },			/* 231 = shmget */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 228 = unimplemented shmat */
	{ 0, 0, 0,
	    sys_nosys },			/* 229 = unimplemented shmctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 230 = unimplemented shmdt */
	{ 0, 0, 0,
	    sys_nosys },			/* 231 = unimplemented shmget */
#endif
};

