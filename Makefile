CC = gcc


all: npsi clean

print_utils.o:
	$(CC) -c src/print_utils.c
globals.o:
	$(CC) -c globals.c 
main.o:
	$(CC) -c main.c -l pcap
ip_addr_stat.o:
	$(CC) -c src/statistics/ip_addr_stat.c -l pthread
igmp_dsct.o:
	$(CC) -c src/capture/protocols/igmp_dsct.c
icmpdsct.o:
	$(CC) -c src/capture/icmpdsct.c 
l2pktmgr.o:
	$(CC) -c src/capture/l2pktmgr.c
l3pktmgr.o:
	$(CC) -c src/capture/l3pktmgr.c
pktmgr.o:
	$(CC) -c src/capture/pktmgr.c -l pcap
rule_parser.o:
	$(CC) -c src/filter/parsing/rule_parser.c
utils.o:
	$(CC) -c src/utils.c
tcpmgr.o:
	$(CC) -c src/capture/tcpmgr.c
udpmgr.o:
	$(CC) -c src/capture/udpmgr.c

http_disect.o:
	$(CC) -c src/capture/protocols/http_disect.c

dns_disect.o:
	$(CC) -c src/capture/protocols/dns_disect.c

alerts.o:
	$(CC) -c src/filter/actions/alerts.c

packet_parser.o:
	$(CC) -c src/filter/parsing/packet_parser.c
#rule_parser.o:
#	$(CC) -c src/filter/parsing/rule_parser.c
rule.o:
	$(CC) -c src/filter/parsing/rule.c
nsh_server.o:
	$(CC) -c src/rpc/nsh_server.c -l pthread -l ssl -l crypto
interpreter.o:
	$(CC) -c src/rpc/cmd/interpreter.c

nsh_commands.o:
	$(CC) -c src/rpc/cmd/nsh_commands.c

watchlist.o:
	$(CC) -c src/statistics/watchlist.c
wclean.o:
	$(CC) -c src/statistics/wclean.c -l pthread

arpcache.o:
	$(CC) -c src/statistics/arpcache.c

update_db.o:
	$(CC) -c src/database/update_db.c -l sqlite3

ftp-disect.o:
	$(CC) -c src/capture/protocols/ftp-disect.c
spi.o:
	$(CC) -c src/engine/spi.c 

debug.o:
	$(CC) -c src/debug.c
npsi: spi.o globals.o pktmgr.o l2pktmgr.o utils.o l3pktmgr.o icmpdsct.o\
			tcpmgr.o udpmgr.o igmp_dsct.o ip_addr_stat.o print_utils.o\
			alerts.o packet_parser.o rule_parser.o rule.o main.o nsh_server.o\
			interpreter.o nsh_commands.o watchlist.o wclean.o arpcache.o \
			update_db.o http_disect.o dns_disect.o ftp-disect.o  debug.o\
			
	$(CC) -o npsi.exe $^ -l pcap -l pthread -l ssl -l crypto -l sqlite3
	strip npsi.exe

clean:
	rm *.o