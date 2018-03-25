/*
 * guzhoudiaoke@126.com
 * 2018-03-10
 */

#include "net.h"
#include "string.h"
#include "babyos.h"
#include "net.h"
#include "math.h"

uint16 net_t::htons(uint16 n)
{
  return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8);
}

uint16 net_t::ntohs(uint16 n)
{
  return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8);
}

uint32 net_t::htonl(uint32 n)
{
  return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
}

uint32 net_t::ntohl(uint32 n)
{
  return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
}

uint32 net_t::make_ipaddr(uint8 ip0, uint8 ip1, uint8 ip2, uint8 ip3)
{
    return (uint32) ((uint32) ip0 << 24) | ((uint32) ip1 << 16) | ((uint32) ip2 << 8) | ip3;
}

void net_t::init()
{
    m_buffer_lock.init();
    m_net_buffers.init(os()->get_obj_pool_of_size());

    uint32 total_buf_num = PAGE_SIZE * NET_BUF_PAGES / NET_BUF_SIZE;
    uint32 n = 0;
    while (n < total_buf_num) {
        uint8* bufs = (uint8 *) os()->get_mm()->alloc_pages(MAX_ORDER);
        uint32 num = PAGE_SIZE * math_t::pow(2, MAX_ORDER) / NET_BUF_SIZE;
        for (int i = 0; i < num; i++, n++) {
            m_net_buffers.push_back((net_buf_t *) (bufs));
            bufs += NET_BUF_SIZE;
        }
    }

    m_arp.init();
    m_ip.init();
}

void net_t::set_eth_addr(uint8 eth_addr[ETH_ADDR_LEN])
{
    m_ethernet.set_eth_addr(eth_addr);

    /* FIXME: only test */
    if (eth_addr[ETH_ADDR_LEN-1] == 0xbc) {
        m_ipaddr = make_ipaddr(192, 168, 1, 104);
        m_gateway = make_ipaddr(192, 168, 1, 1);
        m_subnet_mask = make_ipaddr(255, 255, 255, 0);
    }
    else if (eth_addr[ETH_ADDR_LEN-1] == 0xbe) {
        m_ipaddr = make_ipaddr(192, 168, 1, 105);
        m_gateway = make_ipaddr(192, 168, 1, 1);
        m_subnet_mask = make_ipaddr(255, 255, 255, 0);
    }

    uint8* ip = (uint8 *) (&m_ipaddr);
    console()->kprintf(GREEN, "IP: %d.%d.%d.%d\n", ip[3], ip[2], ip[1], ip[0]);
}

void net_t::transmit(uint8 eth_addr[ETH_ADDR_LEN], uint8* data, uint32 len)
{
    m_ethernet.transmit(eth_addr, PROTO_ARP, data, len);
}

void net_t::receive(uint8* data, uint32 len)
{
    m_ethernet.receive(data, len);
}

net_buf_t* net_t::alloc_net_buffer(uint32 len)
{
    if (len > NET_BUF_DATA_SIZE) {
        return NULL;
    }

    net_buf_t* buf = NULL;
    uint32 flags;
    m_buffer_lock.lock_irqsave(flags);
    buf = *(m_net_buffers.begin());
    m_net_buffers.pop_front();
    m_buffer_lock.unlock_irqrestore(flags);

    return buf;
}

void net_t::free_net_buffer(net_buf_t* buf)
{
    uint32 flags;
    m_buffer_lock.lock_irqsave(flags);
    m_net_buffers.push_front(buf);
    m_buffer_lock.unlock_irqrestore(flags);
}

ethernet_t* net_t::get_ethernet()
{
    return &m_ethernet;
}

uint32 net_t::get_ipaddr()
{
    return m_ipaddr;
}

uint32 net_t::get_subnet_mask()
{
    return m_subnet_mask;
}

uint32 net_t::get_gateway()
{
    return m_gateway;
}

void net_t::arp_request(uint32 ip)
{
    m_arp.request(ip);
}

arp_t* net_t::get_arp()
{
    return &m_arp;
}

ip_t* net_t::get_ip()
{
    return &m_ip;
}

icmp_t* net_t::get_icmp()
{
    return &m_icmp;
}

uint16 net_t::check_sum(uint8* data, uint32 len)
{
    uint32 acc = 0;
    uint16* p = (uint16 *) data;

    int i = len;
    for (; i > 1; i -= 2, p++) {
        acc += *p;
    }

    if (i == 1) {
        acc += *((uint8 *) p);
    }

    while (acc >> 16) {
        acc = (acc & 0xffff) + (acc >> 16);
    }

    return (uint16) ~acc;
}

void net_t::dump_ip_addr(uint32 ip)
{
    uint8* ip_bytes = (uint8 *) (&ip);
    console()->kprintf(YELLOW, "%d.%d.%d.%d", ip_bytes[3], ip_bytes[2], ip_bytes[1], ip_bytes[0]);
}

void net_t::dump_eth_addr(uint8 eth_addr[ETH_ADDR_LEN])
{
    console()->kprintf(YELLOW, "%2x:%2x:%2x:%2x:%2x:%2x",
        eth_addr[0], eth_addr[1], eth_addr[2], eth_addr[3], eth_addr[4], eth_addr[5]);
}

