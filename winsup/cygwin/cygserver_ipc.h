/* cygserver_ipc.h

   Copyright 2002, 2003, 2004, 2012, 2013 Red Hat, Inc.

This file is part of Cygwin.

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#ifndef __CYGSERVER_IPC_H__
#define __CYGSERVER_IPC_H__

/*
 * Datastructure which is part of any IPC input parameter block.
 */
struct vmspace {
  void *vm_map;			/* UNUSED */
  struct shmmap_state *vm_shm;
};

struct proc {
  pid_t cygpid;
  DWORD winpid;
  uid_t uid;
  gid_t gid;
  int gidcnt;
  gid_t *gidlist;
  bool is_admin;
  struct vmspace *p_vmspace;
  HANDLE signal_arrived;
};

#ifdef __INSIDE_CYGWIN__
#include "sigproc.h"
extern inline void
ipc_set_proc_info (proc &blk)
{
  blk.cygpid = getpid ();
  blk.winpid = GetCurrentProcessId ();
  blk.uid = geteuid32 ();
  blk.gid = getegid32 ();
  blk.gidcnt = 0;
  blk.gidlist = NULL;
  blk.is_admin = false;
  _my_tls.set_signal_arrived (true, blk.signal_arrived);
}
#endif /* __INSIDE_CYGWIN__ */

#ifndef __INSIDE_CYGWIN__
class ipc_retval {
private:
  union {
    int i;
    ssize_t ssz;
    size_t sz;
    vm_offset_t off;
    vm_object_t obj;
  };

public:
  ipc_retval (ssize_t nssz) { ssz = nssz; }

  operator int () const { return i; }
  int operator = (int ni) { return i = ni; }

#ifndef __x86_64__
  /* On x86_64: size_t == vm_offset_t == unsigned long */
  operator size_t () const { return sz; }
  size_t operator = (size_t nsz) { return sz = nsz; }
#else
  /* On i686: ssize_t == long == int */
  operator ssize_t () const { return ssz; }
  ssize_t operator = (ssize_t nssz) { return ssz = nssz; }
#endif

  operator vm_offset_t () const { return off; }
  vm_offset_t operator = (vm_offset_t noff) { return off = noff; }

  operator vm_object_t () const { return obj; }
  vm_object_t operator = (vm_object_t nobj) { return obj = nobj; }
};

struct thread {
  class process *client;
  proc *ipcblk;
  ipc_retval td_retval[2];
};
#define td_proc ipcblk
#define p_pid cygpid
#endif

#endif /* __CYGSERVER_IPC_H__ */
