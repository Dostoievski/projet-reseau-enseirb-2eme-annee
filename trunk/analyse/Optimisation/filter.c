/*
 * Expression			Description
 * ----------			-----------
 * ip					Capture all IP packets.
 * tcp					Capture only TCP packets.
 * udp					Capture only UDP packets.
 * arp					Capture only ARP packets.
 * icmp					Capture only ICMP packets.
 * tcp port 80			Capture only TCP packets with a port equal to 80.
 * ip host 10.1.2.3		Capture all IP packets to or from host 10.1.2.3.
 *
 ****************************************************************************
 *
 */

#include "process.h"

char filter_exp[] = "ip";		/* filter expression [3] */


int main(int argc, char **argv)
{
  /* we init the protocoles occurences*/
  count_protocoles_init(protocoles,PROTOCOLES_NUM);

	char errbuf[PCAP_ERRBUF_SIZE];		/* error buffer */
	pcap_t *handle;				/* packet capture handle */





	struct bpf_program fp;			/* compiled filter program (expression) */
	//bpf_u_int32 mask;			/* subnet mask */
	bpf_u_int32 net;			/* ip */
	int num_packets = -1;			/* number of packets to capture */
	char *file_name = NULL;                   /* the .pcap file to analyze */
       	
	/* check for file  name on command-line */
	 if (argc < 2 || argc > 3) {
		fprintf(stderr, "error: unrecognized command-line options\n\n");
		print_app_usage();
		exit(EXIT_FAILURE); 
	 }
	 
	 else {
	   if  (argc == 3) {   num_packets = atoi(argv[2]); }
	   
	 file_name = argv[1];
	 
	 //open capture file for offline processing
	handle = pcap_open_offline(file_name, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open .pcap file \n");
		exit(EXIT_FAILURE);
	}
	printf("opening .pcap file \n \n  \n");

	/* make sure we're capturing on an Ethernet device [2] */
	if (pcap_datalink(handle) != DLT_EN10MB) {
	  fprintf(stderr, " pcap_datalink() failed : %s\n",pcap_geterr(handle) );
		exit(EXIT_FAILURE);
	}

	/* compile the filter expression */
	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n",
		    filter_exp, pcap_geterr(handle));
		exit(EXIT_FAILURE);
	}

	/* apply the compiled filter */
	if (pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n",
		    filter_exp, pcap_geterr(handle));
		exit(EXIT_FAILURE);
	}

	/* now we can set our callback function */
	if(pcap_loop(handle, num_packets, got_packet, NULL) < 0){
	  printf("pcap_loop() failed : %s\n",pcap_geterr(handle));
	}

	/* cleanup */
	//pcap_freecode(&fp);
	pcap_close(handle);

	printf("\nCapture complete.\n");
	/* we print the protocoles stats */
	printf(" \n******************************************************************\nstatisitiques : \n");

	count_protocoles_stats();
	 }
return 0;
}

