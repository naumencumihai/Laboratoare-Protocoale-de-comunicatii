#!/usr/bin/env python

import click
import info
import os

from scapy.all import *

@click.command()
@click.option('--testname')
@click.option('--hosts')
@click.option('--me')
def main(testname, me, hosts):
    if testname == 'ping':
        pkts = [IP(dst=host)/ICMP() for host in hosts.split(',')]

    elif testname == 'ping6':
        pkts = [IPv6(dst=host)/ICMPv6EchoRequest() for host in hosts.split(',')]

    else:
        return

    pkts = sr(pkts, timeout=4, verbose=False, iface=info.get('host_if_name', me))

    if len(pkts[0]) == 3:
        print('PASS')

    else:
        print('FAIL')

if __name__ == '__main__':
    main()
