#include "RFEthernet.h"

RFEthernet::RFEthernet(std::unordered_map<std::string, std::any>& sample_config)
    : Sensor(sample_config), _timeout(std::any_cast<int>(sample_config["Timeout"]))
{
	// launch cmd file to configure radar parameters
	std::string command = "C:\\Users\\Adnan\\Documents\\Github\\syndicate\\config\\run_multi_4rx3tx_mmwavestudio.cmd";
	WinExec(command.c_str(), SW_HIDE);
	std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(60))); // wait 60 seconds for lua script to execute

	int interface_id;
	int total_interfaces=0;
	char errbuf[PCAP_ERRBUF_SIZE];
	
#ifdef _WIN32
	/* Load Npcap and its functions. */
	if (!LoadNpcapDlls())
	{
		fprintf(stderr, "Couldn't load Npcap\n");
		exit(1);
	}
#endif
    
	/* Retrieve the device list on the local machine */
	if (pcap_findalldevs(&all_devs, errbuf) == -1)
	{
		fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}
    
    /* Print the list */
    for(curr_dev=all_devs; curr_dev; curr_dev=curr_dev->next)
    {
        std::cout << ++total_interfaces << "." << curr_dev->name;
        if (curr_dev->description)
            std::cout << " (" <<  curr_dev->description << ")" << std::endl;
        else
            std::cout << " (No description available)" << std::endl;
    }

    if(total_interfaces==0)
    {
        std::cout << std::endl << "No interfaces found! Make sure Npcap is installed." << std::endl;
        exit(1);
    }
    
    // std::cout << "Enter the interface number (1-" << total_interfaces << "): ";
    // std::cin >> interface_id;

	interface_id = 8;
    
    if(interface_id < 1 || interface_id > total_interfaces)
    {
        std::cout << std::endl << "Interface number out of range." << std::endl;
        /* Free the device list */
        pcap_freealldevs(all_devs);
        exit(1);
    }
		
	/* Jump to the selected adapter */
    for(curr_dev=all_devs, total_interfaces=0 ; total_interfaces < interface_id-1 ; curr_dev=curr_dev->next, total_interfaces++);
}

#ifdef _WIN32
bool RFEthernet::LoadNpcapDlls()
{
	_TCHAR npcap_dir[512];
	UINT len;
	len = GetSystemDirectory(npcap_dir, 480);
	if (!len) {
		fprintf(stderr, "Error in GetSystemDirectory: %x", GetLastError());
		return false;
	}
	_tcscat_s(npcap_dir, 512, _T("\\Npcap"));
	if (SetDllDirectory(npcap_dir) == 0) {
		fprintf(stderr, "Error in SetDllDirectory: %x", GetLastError());
		return false;
	}
	return true;
}
#endif

void RFEthernet::AcquireSave(double seconds, boost::barrier& startBarrier)
{
	seconds = seconds + 15; // TODO Bug fix for radar prematurely ending ;)
	pcap_dumper_t *dumpfile;
	char errbuf[PCAP_ERRBUF_SIZE];
    /* Open the adapter */
	if ((adhandle = pcap_open_live(curr_dev->name,	// name of the device
							 65536,			// portion of the packet to capture. 
											// 65536 grants that the whole packet will be captured on all the MACs.
							 1,				// promiscuous mode (nonzero means promiscuous)
							 _timeout,			// read timeout
							 errbuf			// error buffer
							 )) == NULL)
	{
		fprintf(stderr,"\nUnable to open the adapter. %s is not supported by Npcap\n", curr_dev->name);
		/* Free the device list */
		pcap_freealldevs(all_devs);
		exit(1);
	}

	/* Open the dump file */
    std::ostringstream filename;
    filename << rootPath << sensorName << ".raw";
	dumpfile = pcap_dump_open(adhandle, filename.str().c_str());

	if(dumpfile==NULL)
	{
		fprintf(stderr,"\nError opening output file\n");
		exit(1);
	}
	
    /* At this point, we no longer need the device list. Free it */
    pcap_freealldevs(all_devs);
    
    /* start the capture */
	// startBarrier.wait();
	std::cout << "radar starting \n";
	boost::thread interrupt_thread(boost::bind(&RFEthernet::interrupt_pcap_loop, this, seconds));
    pcap_loop(adhandle, 0, packet_handler, (unsigned char *)dumpfile);
	std::cout << "RF Execution Complete\n\n";

	interrupt_thread.join();
    pcap_close(adhandle);
	// launch cmd file to configure radar parameters
	// std::string command = "C://Users//Adnan//Documents//Github//syndicate//config//close_mmwavestudio.cmd";
	// WinExec(command.c_str(), SW_HIDE);
}

static void packet_handler(u_char *dumpfile, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	/* save the packet on the dump file */
	pcap_dump(dumpfile, header, pkt_data);
}

void RFEthernet::interrupt_pcap_loop(double seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(seconds)));
	pcap_breakloop(adhandle);
}

void RFEthernet::AcquireSaveBarrier(double seconds, boost::barrier& frameBarrier)
{
    std::cout << "I am not defined yet.\n\n";
}

void RFEthernet::ConcurrentAcquire(double seconds, boost::barrier& frameBarrier)
{
    std::cout << "I am not defined yet.\n\n";
}

void RFEthernet::ConcurrentSave()
{
    std::cout << "I am not defined yet.\n\n";
}


RFEthernet::~RFEthernet()
{
}
