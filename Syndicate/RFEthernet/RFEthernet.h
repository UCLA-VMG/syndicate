#pragma once

#ifdef _MSC_VER
/*
 * we do not want the warnings about the old deprecated and unsecure CRT functions
 * since these examples can be compiled under *nix as well
 */
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "sensor.h"

#include <pcap.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <string>
#include <memory>
#include <any>

struct RFEthernet : public Sensor 
{
    RFEthernet(std::unordered_map<std::string, std::any>& sample_config);
    ~RFEthernet();

    void AcquireSave(double seconds);
    void AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier);
    void ConcurrentAcquire(double seconds, boost::barrier& frameBarrier);
    void ConcurrentSave();

    pcap_if_t *all_devs;
	pcap_if_t *curr_dev;
    int _timeout;
	                    pcap_t *adhandle;
	                    pcap_dumper_t *dumpfile;

#ifdef _WIN32
    bool LoadNpcapDlls();
#endif
};

/* Callback function invoked by libpcap for every incoming packet */
static void packet_handler(u_char *dumpfile, const struct pcap_pkthdr *header, const u_char *pkt_data);