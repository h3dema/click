#ifndef TOBPF_HH
#define TOBPF_HH
#include "element.hh"
#include "string.hh"
#include "elements/userlevel/frombpf.hh"

/*
 * =c
 * ToBPF(DEVNAME)
 * =d
 *
 * Pulls packets and sends them out the named device using
 * Berkeley Packet Filters (or Linux equivalent).
 *
 * Packets sent via ToBPF should already have a link-level
 * header prepended. This means that ARP processing,
 * for example, must already have been done.
 *
 * Under Linux, a FromBPF element will not receive packets sent by a ToBPF
 * element for the same device. Under other operating systems, your mileage
 * may vary.
 *
 * This element is only available at user level.
 * 
 * =a FromBPF
 * =a FromDump
 * =a ToDump */

#ifdef HAVE_PCAP
extern "C" {
# include <pcap.h>
}
#else
# include "fakepcap.h"
#endif

// what method do we use to write BPF?
#ifdef HAVE_PCAP
# if defined(__FreeBSD__) || defined(__OpenBSD__)
#  define TOBPF_BSD_DEV_BPF 1
#  define TOBPF_WRITE 1
# elif defined(__linux__)
#  define TOBPF_LINUX 1
#  define TOBPF_SEND 1
#  define TOBPF_SENDTO 1
# endif
#endif

/*
 * Write packets to the ethernet via the bpf.
 * Expects packets that already have an ether header.
 * Can push or pull.
 */

class ToBPF : public Element {

  String _ifname;
  int _fd; /* might be borrowed from FromBPF */
  pcap_t *_pcap;

#if TOBPF_SENDTO
  struct sockaddr _sendto_sa;
#endif
  
 public:
  
  ToBPF();
  ToBPF(const String &ifname);
  ~ToBPF();
  
  const char *class_name() const		{ return "ToBPF"; }
  const char *processing() const		{ return AGNOSTIC; }
  
  ToBPF *clone() const;
  int configure_phase() const	{ return FromBPF::CONFIGURE_PHASE_TOBPF; }
  int configure(const Vector<String> &, ErrorHandler *);
  int initialize(ErrorHandler *);
  void uninitialize();
  
  void push(int port, Packet *);
  void run_scheduled();
  
};

#endif
