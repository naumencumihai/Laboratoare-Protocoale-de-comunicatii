N_HOSTS = 4
LOGFILE = "/tmp/debug.txt"

BASE_FORMATS = {
        "host_name": "host{}",
        "router_if_name": "r-{}",
        "host_if_name": "h-{}",
        "router_ip": "192.168.{}.1",
        "host_ip": "192.168.{}.2",
        "router_ip6": "fec0::{:x}:1",
        "router_llip6": "fe80::{:x}:1",
        "host_ip6": "fd00::{:x}:2",
        "host_llip6": "fe80::{:x}:2",
        "router_mac": "DE:FE:C8:ED:00:{:02x}",
        "host_mac": "DE:AD:BE:EF:00:{:02x}",
}


def get(value, host):
    return BASE_FORMATS[value].format(host)
