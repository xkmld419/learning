/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    INET_Addr.h
 *
 *  INET_Addr.h,v 4.57 2006/01/17 18:49:37 jwillemsen Exp
 *
 *  @author Douglas C. Schmidt <schmidt@cs.wustl.edu>
 */
//=============================================================================

#ifndef ACE_INET_ADDR_H
#define ACE_INET_ADDR_H

#include <string.h>
#include "Log_Msg.h"
#include "Trace.h"
#include "Sock_Connect.h"
#include <netinet/in.h>
#include <arpa/inet.h>


#include "Addr.h"

/**
 * @class ACE_INET_Addr
 *
 * @brief Defines a C++ wrapper facade for the Internet domain address
 * family format.
 */
class  ACE_INET_Addr : public ACE_Addr
{
public:
  // = Initialization methods.

  /// Default constructor.
  ACE_INET_Addr (void);

  /// Copy constructor.
  ACE_INET_Addr (const ACE_INET_Addr &);

  /// Creates an ACE_INET_Addr from a sockaddr_in structure.
  ACE_INET_Addr (const sockaddr_in *, int len);

  /// Creates an ACE_INET_Addr from a <port_number> and the remote
  /// <host_name>. The port number is assumed to be in host byte order.
  /// To set a port already in network byte order, please @see set().
  /// Use address_family to select IPv6 (PF_INET6) vs. IPv4 (PF_INET).
  ACE_INET_Addr (u_short port_number,
                 const char host_name[],
                 int address_family = AF_UNSPEC);

  /**
   * Initializes an ACE_INET_Addr from the <address>, which can be
   * "ip-number:port-number" (e.g., "tango.cs.wustl.edu:1234" or
   * "128.252.166.57:1234").  If there is no ':' in the <address> it
   * is assumed to be a port number, with the IP address being
   * INADDR_ANY.
   */
  explicit ACE_INET_Addr (const char address[]);

  /**
   * Creates an ACE_INET_Addr from a <port_number> and an Internet
   * <ip_addr>.  This method assumes that <port_number> and <ip_addr>
   * are in host byte order. If you have addressing information in
   * network byte order, @see set().
   */
  explicit ACE_INET_Addr (u_short port_number,
                          ACE_UINT32 ip_addr = INADDR_ANY);

  /// Uses <getservbyname> to create an ACE_INET_Addr from a
  /// <port_name>, the remote <host_name>, and the <protocol>.
  ACE_INET_Addr (const char port_name[],
                 const char host_name[],
                 const char protocol[] = "tcp");

  /**
   * Uses <getservbyname> to create an ACE_INET_Addr from a
   * <port_name>, an Internet <ip_addr>, and the <protocol>.  This
   * method assumes that <ip_addr> is in host byte order.
   */
  ACE_INET_Addr (const char port_name[],
                 ACE_UINT32 ip_addr,
                 const char protocol[] = "tcp");

#if defined (ACE_HAS_WCHAR)
  ACE_INET_Addr (u_short port_number,
                 const wchar_t host_name[],
                 int address_family = AF_UNSPEC);

  explicit ACE_INET_Addr (const wchar_t address[]);

  ACE_INET_Addr (const wchar_t port_name[],
                 const wchar_t host_name[],
                 const wchar_t protocol[] = ACE_TEXT_WIDE ("tcp"));

  ACE_INET_Addr (const wchar_t port_name[],
                 ACE_UINT32 ip_addr,
                 const wchar_t protocol[] = ACE_TEXT_WIDE ("tcp"));
#endif /* ACE_HAS_WCHAR */

  /// Default dtor.
  ~ACE_INET_Addr (void);

  inline void reset (void)
  {
    memset (&this->inet_addr_, 0, sizeof (this->inet_addr_));
    if (this->get_type() == AF_INET)
      {
  #ifdef ACE_HAS_SOCKADDR_IN_SIN_LEN
        this->inet_addr_.in4_.sin_len = sizeof (this->inet_addr_.in4_);
  #endif
        this->inet_addr_.in4_.sin_family = AF_INET;
      }
  #if defined (ACE_HAS_IPV6)
    else if (this->get_type() == AF_INET6)
      {
  #ifdef ACE_HAS_SOCKADDR_IN6_SIN6_LEN
        this->inet_addr_.in6_.sin6_len = sizeof (this->inet_addr_.in6_);
  #endif
        this->inet_addr_.in6_.sin6_family = AF_INET6;
      }
  #endif  /* ACE_HAS_IPV6 */
  }
  
  
  
  // Return the port number, converting it into host byte order...
  
  inline u_short get_port_number (void) const
  {
    ACE_TRACE ("get_port_number");
  #if defined (ACE_HAS_IPV6)
    if (this->get_type () == PF_INET)
      return ntohs (this->inet_addr_.in4_.sin_port);
    else
      return ntohs (this->inet_addr_.in6_.sin6_port);
  #else
    return ntohs (this->inet_addr_.in4_.sin_port);
  #endif /* ACE_HAS_IPV6 */
  }
  
  inline int get_addr_size (void) const
  {
    ACE_TRACE ("get_addr_size");
  #if defined (ACE_HAS_IPV6)
    if (this->get_type () == PF_INET)
      return sizeof this->inet_addr_.in4_;
    else
      return sizeof this->inet_addr_.in6_;
  #else
    return sizeof this->inet_addr_.in4_;
  #endif /* ACE_HAS_IPV6 */
  }
  
  inline bool operator < (const ACE_INET_Addr &rhs) const
  {
    return this->get_ip_address () < rhs.get_ip_address ()
      || (this->get_ip_address () == rhs.get_ip_address ()
          && this->get_port_number () < rhs.get_port_number ());
  }
  
  
  // Return @c true if the IP address is INADDR_ANY or IN6ADDR_ANY.
  inline bool is_any (void) const
  {
  #if defined (ACE_HAS_IPV6)
    if (this->get_type () == AF_INET6)
        return IN6_IS_ADDR_UNSPECIFIED (&this->inet_addr_.in6_.sin6_addr);
  #endif /* ACE_HAS_IPV6 */
  
    return (this->inet_addr_.in4_.sin_addr.s_addr == INADDR_ANY);
  }
  
  // Return @c true if the IP address is IPv4/IPv6 loopback address.
  inline bool is_loopback (void) const
  {
  #if defined (ACE_HAS_IPV6)
    if (this->get_type () == AF_INET6)
        return IN6_IS_ADDR_LOOPBACK (&this->inet_addr_.in6_.sin6_addr);
  #endif /* ACE_HAS_IPV6 */
  
    return (this->get_ip_address () == INADDR_LOOPBACK);
  }
  
  #if defined (ACE_HAS_IPV6)
  // Return @c true if the IP address is IPv6 linklocal address.
  inline bool is_linklocal (void) const
  {
    if (this->get_type () == AF_INET6)
        return IN6_IS_ADDR_LINKLOCAL (&this->inet_addr_.in6_.sin6_addr);
  
    return false;
  }
  
  // Return @c true if the IP address is IPv4 mapped IPv6 address.
  inline bool is_ipv4_mapped_ipv6 (void) const
  {
    if (this->get_type () == AF_INET6)
        return IN6_IS_ADDR_V4MAPPED (&this->inet_addr_.in6_.sin6_addr);
  
    return false;
  }
  
  // Return @c true if the IP address is IPv4-compatible IPv6 address.
  inline bool is_ipv4_compat_ipv6 (void) const
  {
    if (this->get_type () == AF_INET6)
        return IN6_IS_ADDR_V4COMPAT (&this->inet_addr_.in6_.sin6_addr);
  
    return false;
  }
  #endif /* ACE_HAS_IPV6 */

  // = Direct initialization methods.

  // These methods are useful after the object has been constructed.

  /// Initializes from another ACE_INET_Addr.
  int set (const ACE_INET_Addr &);

  /**
   * Initializes an ACE_INET_Addr from a <port_number> and the
   * remote <host_name>.  If <encode> is non-zero then <port_number> is
   * converted into network byte order, otherwise it is assumed to be
   * in network byte order already and are passed straight through.
   * address_family can be used to select IPv4/IPv6 if the OS has
   * IPv6 capability (ACE_HAS_IPV6 is defined). To specify IPv6, use
   * the value AF_INET6. To specify IPv4, use AF_INET.
   */
  int set (u_short port_number,
           const char host_name[],
           int encode = 1,
           int address_family = AF_UNSPEC);

  /**
   * Initializes an ACE_INET_Addr from a @a port_number and an Internet
   * @a ip_addr.  If @a encode is non-zero then the port number and IP address
   * are converted into network byte order, otherwise they are assumed to be
   * in network byte order already and are passed straight through.
   *
   * If <map> is non-zero and IPv6 support has been compiled in,
   * then this address will be set to the IPv4-mapped IPv6 address of it.
   */
  int set (u_short port_number,
           ACE_UINT32 ip_addr = INADDR_ANY,
           int encode = 1,
           int map = 0);

  /// Uses <getservbyname> to initialize an ACE_INET_Addr from a
  /// <port_name>, the remote <host_name>, and the <protocol>.
  int set (const char port_name[],
           const char host_name[],
           const char protocol[] = "tcp");

  /**
   * Uses <getservbyname> to initialize an ACE_INET_Addr from a
   * <port_name>, an <ip_addr>, and the <protocol>.  This assumes that
   * <ip_addr> is already in network byte order.
   */
  int set (const char port_name[],
           ACE_UINT32 ip_addr,
           const char protocol[] = "tcp");

  /**
   * Initializes an ACE_INET_Addr from the <addr>, which can be
   * "ip-number:port-number" (e.g., "tango.cs.wustl.edu:1234" or
   * "128.252.166.57:1234").  If there is no ':' in the <address> it
   * is assumed to be a port number, with the IP address being
   * INADDR_ANY.
   */
  int set (const char addr[]);

  /// Creates an ACE_INET_Addr from a sockaddr_in structure.
  int set (const sockaddr_in *,
           int len);

#if defined (ACE_HAS_WCHAR)
  int set (u_short port_number,
           const wchar_t host_name[],
           int encode = 1,
           int address_family = AF_UNSPEC);

  int set (const wchar_t port_name[],
           const wchar_t host_name[],
           const wchar_t protocol[] = ACE_TEXT_WIDE ("tcp"));

  int set (const wchar_t port_name[],
           ACE_UINT32 ip_addr,
           const wchar_t protocol[] = ACE_TEXT_WIDE ("tcp"));

  int set (const wchar_t addr[]);
#endif /* ACE_HAS_WCHAR */

  /// Return a pointer to the underlying network address.
  virtual void *get_addr (void) const;

  /// Set a pointer to the address.
  virtual void set_addr (void *, int len);

  /// Set a pointer to the address.
  virtual void set_addr (void *, int len, int map);

  /**
   * Transform the current ACE_INET_Addr address into string format.
   * If <ipaddr_format> is non-0 this produces "ip-number:port-number"
   * (e.g., "128.252.166.57:1234"), whereas if <ipaddr_format> is 0
   * this produces "ip-name:port-number" (e.g.,
   * "tango.cs.wustl.edu:1234").  Returns -1 if the <size> of the
   * <buffer> is too small, else 0.
   */
  virtual int addr_to_string (ACE_TCHAR buffer[],
                              size_t size,
                              int ipaddr_format = 1) const;

  /**
   * Initializes an ACE_INET_Addr from the @a address, which can be
   * "ip-addr:port-number" (e.g., "tango.cs.wustl.edu:1234"),
   * "ip-addr:port-name" (e.g., "tango.cs.wustl.edu:telnet"),
   * "ip-number:port-number" (e.g., "128.252.166.57:1234"), or
   * "ip-number:port-name" (e.g., "128.252.166.57:telnet").  If there
   * is no ':' in the <address> it is assumed to be a port number,
   * with the IP address being INADDR_ANY.
   */
  virtual int string_to_addr (const char address[]);

#if defined (ACE_HAS_WCHAR)
  /*
  virtual int string_to_addr (const char address[]);
  */
#endif /* ACE_HAS_WCHAR */

  /**
   * Sets the port number without affecting the host name.  If
   * <encode> is enabled then <port_number> is converted into network
   * byte order, otherwise it is assumed to be in network byte order
   * already and are passed straight through.
   */
  void set_port_number (u_short,
                        int encode = 1);

  /**
   * Sets the address without affecting the port number.  If
   * <encode> is enabled then <ip_addr> is converted into network
   * byte order, otherwise it is assumed to be in network byte order
   * already and are passed straight through.  The size of the address
   * is specified in the <len> parameter.
   * If <map> is non-zero, IPv6 support has been compiled in, and
   * <ip_addr> is an IPv4 address, then this address is set to the IPv4-mapped
   * IPv6 address of it.
   */
  int set_address (const char *ip_addr,
                   int len,
                   int encode = 1,
                   int map = 0);

#if (defined (__linux__) || defined (ACE_WIN32)) && defined (ACE_HAS_IPV6)
  /**
   * Sets the interface that should be used for this address. This only has
   * an effect when the address is link local, otherwise it does nothing.
   */
  int set_interface (const char *intf_name);
#endif /* (__linux__ || ACE_WIN32) && ACE_HAS_IPV6 */

  /// Return the port number, converting it into host byte-order.

  /**
   * Return the character representation of the name of the host,
   * storing it in the <hostname> (which is assumed to be
   * <hostnamelen> bytes long).  This version is reentrant.  If
   * <hostnamelen> is greater than 0 then <hostname> will be
   * NUL-terminated even if -1 is returned.
   */
  int get_host_name (char hostname[],
                     size_t hostnamelen) const;

#if defined (ACE_HAS_WCHAR)
  int get_host_name (wchar_t hostname[],
                     size_t hostnamelen) const;
#endif /* ACE_HAS_WCHAR */

  /**
   * Return the character representation of the hostname (this version
   * is non-reentrant since it returns a pointer to a static data
   * area).
   */
  const char *get_host_name (void) const;

  /// Return the "dotted decimal" Internet address.
  const char *get_host_addr (void) const;
  const char *get_host_addr (char *dst, int size) const;

  /// Return the 4-byte IP address, converting it into host byte
  /// order.
  ACE_UINT32 get_ip_address (void) const;

  /// Return @c true if the IP address is INADDR_ANY or IN6ADDR_ANY.
  
  /// Return @c true if the IP address is IPv4/IPv6 loopback address.

  /**
   * Returns @c true if @c this is less than @a rhs.  In this context,
   * "less than" is defined in terms of IP address and TCP port
   * number.  This operator makes it possible to use @c ACE_INET_Addrs
   * in STL maps.
   */
  /// Compare two addresses for equality.  The addresses are considered
  /// equal if they contain the same IP address and port number.
  bool operator == (const ACE_INET_Addr &SAP) const;

  /// Compare two addresses for inequality.
  bool operator != (const ACE_INET_Addr &SAP) const;

  /// Computes and returns hash value.
  virtual u_long hash (void) const;

  /// Dump the state of an object.
  void dump (void) const;

  /// Declare the dynamic allocation hooks.
  ACE_ALLOC_HOOK_DECLARE;

private:
  /// Insure that @a hostname is properly null-terminated.
  int get_host_name_i (char hostname[], size_t hostnamelen) const;

  // Methods to gain access to the actual address of
  // the underlying internet address structure.
  inline void *
  ip_addr_pointer (void) const
  {
  #if defined (ACE_HAS_IPV6)
    if (this->get_type () == PF_INET)
      return (void*)&this->inet_addr_.in4_.sin_addr;
    else
      return (void*)&this->inet_addr_.in6_.sin6_addr;
  #else
    return (void*)&this->inet_addr_.in4_.sin_addr;
  #endif
  }

  inline int determine_type (void) const
  {
  #if defined (ACE_HAS_IPV6)
  #  if defined (ACE_USES_IPV4_IPV6_MIGRATION)
    return ACE::ipv6_enabled () ? AF_INET6 : AF_INET;
  #  else
    return AF_INET6;
  #  endif /* ACE_USES_IPV4_IPV6_MIGRATION */
  #else
    return AF_INET;
  #endif /* ACE_HAS_IPV6 */
  }

  inline int ip_addr_size (void) const
  {
    // Since this size value is used to pass to other host db-type
    // functions (gethostbyaddr, etc.) the length is of int type.
    // Thus, cast all these sizes back to int. They're all well
    // within the range of an int anyway.
  #if defined (ACE_HAS_IPV6)
    if (this->get_type () == PF_INET)
      return static_cast<int> (sizeof this->inet_addr_.in4_.sin_addr);
    else
      return static_cast<int> (sizeof this->inet_addr_.in6_.sin6_addr);
  #else
    // These _UNICOS changes were picked up from pre-IPv6 code in
    // get_host_name_i... the IPv6 section above may need something
    // similar, so keep an eye out for it.
  #  if !defined(_UNICOS)
    return static_cast<int> (sizeof this->inet_addr_.in4_.sin_addr.s_addr);
  #  else /* _UNICOS */
    return static_cast<int> (sizeof this->inet_addr_.in4_.sin_addr);
  #  endif /* ! _UNICOS */
  #endif /* ACE_HAS_IPV6 */
  }


  /// Underlying representation.
  /// This union uses the knowledge that the two structures share the
  /// first member, sa_family (as all sockaddr structures do).
  union
  {
    sockaddr_in  in4_;
#if defined (ACE_HAS_IPV6)
    sockaddr_in6 in6_;
#endif /* ACE_HAS_IPV6 */
  } inet_addr_;

#if defined (ACE_VXWORKS)
  char buf_[INET_ADDR_LEN];
#endif
};

#endif /* ACE_INET_ADDR_H */
