 #ifndef __LWIPOPTS_H__
 #define __LWIPOPTS_H__
 
 /**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
 #define SYS_LIGHTWEIGHT_PROT    1     //* SYS_LIGHTWEIGHT_PROT==1:�������Ҫȷ���Ŀ����񱣻� *�ڻ��������䣬���պ��ڴ�Ĺؼ����� *����ͻ��ա�
 
 /**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
 #define NO_SYS                  0     //NO_SYS��ʾ�޲���ϵͳģ��㣬�����ǳ���Ҫ�� ��Ϊ�޲���ϵͳ���в���ϵͳ����ֲ�ͱ�д����ȫ��һ���ģ� 
                                        //�����������޲���ϵͳ��ֲ�����Խ�����궨��Ϊ1��               
 /**
 * NO_SYS_NO_TIMERS==1: Drop support for sys_timeout when NO_SYS==1
 * Mainly for compatibility to old versions.
 */
 #define NO_SYS_NO_TIMERS        0    // NO_SYS_NO_TIMERS==1:��NO_SYS==1ʱ��ȡ����sys_timeout��֧�� *��Ҫ��Ϊ�˼��ݾɰ汾��
 
 /* ---------- Memory options ---------- */
 /* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
     lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
     byte alignment -> define MEM_ALIGNMENT to 2. */
 #define MEM_ALIGNMENT           4    //ʹ��4�ֽڶ���ģʽ                
 
 /* MEM_SIZE: the size of the heap memory. If the application will send
 a lot of data that needs to be copied, this should be set high. */
 #define MEM_SIZE                (10 * 1024)        //���ڴ�Ĵ�С�����Ӧ�ó��򽫷��ͺܶ���Ҫ���Ƶ�����Ӧ�����õô�һ�㡣     
 
 /* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
     sends a lot of data out of ROM (or other static memory), this
     should be set high. */
 #define MEMP_NUM_PBUF           16    // MEMP_NUM_PBUF: memp�ṹpbuf�����������Ӧ�ó����ROM(��������̬�ڴ�)���ʹ������ݣ���Ӧ����ø�һЩ��
 /* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
     per active UDP "connection". */
 #define MEMP_NUM_UDP_PCB        4    //MEMP_NUM_UDP_PCB: UDPЭ����ƿ��������һ��ÿ����Ծ��UDP�����ӡ���
 /* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
     connections. */
 #define MEMP_NUM_TCP_PCB        6   //TCPЭ����ƿ��������һ��ÿ����Ծ��tcp�����ӡ���
 /* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
     connections. */
 #define MEMP_NUM_TCP_PCB_LISTEN 6   //���ڼ�����TCP�������ӡ�
 /* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
     segments. */
 #define MEMP_NUM_TCP_SEG        32   //ͬʱ�Ŷӵ�TCP������
 /* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
     timeouts. */
 #define MEMP_NUM_SYS_TIMEOUT    10   //ͬʱ��ĳ�ʱ������
 
 /* ---------- Pbuf options ---------- */
 /* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
 #define PBUF_POOL_SIZE          4      //PBUF_POOL�ڴ�����ڴ��������            
 
 /* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
 #define PBUF_POOL_BUFSIZE       LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_ENCAPSULATION_HLEN+PBUF_LINK_HLEN)     //PBUF_POOL�ڴ����ÿ���ڴ���С��            
 
 /* ---------- TCP options ---------- */
 #define LWIP_TCP                1    //����TCP
 #define TCP_TTL                 255  //TCP����ʱ��
 
 /* Controls if TCP should queue segments that arrive out of
     order. Define to 0 if your device is low on memory. */
 #define TCP_QUEUE_OOSEQ         0   //����TCP�Ƿ�Ӧ�öԵ���Ķν����Ŷ���������豸�ڴ治�㣬����Ϊ0��
 
 /* TCP Maximum segment size. */
 #define TCP_MSS                 (1500 - 40)    //TCPЭ�鱨����󳤶ȡ�             
 
 /* TCP sender buffer space (bytes). */
 #define TCP_SND_BUF             (4*TCP_MSS)    //����TCPЭ��ʹ�õ�����ͻ������ռ䣨�ֽڣ���     
 
 /*  TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
 as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */
 
 #define TCP_SND_QUEUELEN        (8* TCP_SND_BUF/TCP_MSS)  //TCP���Ͷ˻������ռ�(pbufs)����һ�����������Ϊ(2 * TCP_SND_BUF/TCP_MSS)��
 
 /* TCP receive window. */
 #define TCP_WND                 (4*TCP_MSS)   //TCP���մ��ڴ�С��      
 
 /* ---------- ICMP options ---------- */
 #define LWIP_ICMP                       1
 
 /* ---------- DHCP options ---------- */
 /* Define LWIP_DHCP to 1 if you want DHCP configuration of
 interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
 turning this on does currently not work. */
 #define LWIP_DHCP               1
 
 /* ---------- UDP options ---------- */
 #define LWIP_UDP                1
 #define UDP_TTL                 255
 
 /* ---------- Statistics options ---------- */  //ͳ�ƹ���
 #define LWIP_STATS 0  
 #define LWIP_PROVIDE_ERRNO 1
 
 /* ---------- link callback options ---------- */
 /* LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
 #define LWIP_NETIF_LINK_CALLBACK        0  //֧�����ԽӿڵĻص����������Ӹı�ʱ(����������)
 /*
     --------------------------------------
     ---------- Checksum options ----------
     --------------------------------------
 */
 
 /*The STM32F4x7 allows comput
 ing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature let the following define uncommented.
 - To disable it and process by CPU comment the  the checksum. 
 */
// #define CHECKSUM_BY_HARDWARE
 
 #ifdef CHECKSUM_BY_HARDWARE  //Ӳ��Ч��
 /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
 #define CHECKSUM_GEN_IP                 0
 /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
 #define CHECKSUM_GEN_UDP                0
 /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
 #define CHECKSUM_GEN_TCP                0
 /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
 #define CHECKSUM_CHECK_IP               0
 /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
 #define CHECKSUM_CHECK_UDP              0
 /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
 #define CHECKSUM_CHECK_TCP              0
 /*CHECKSUM_CHECK_ICMP==0: Check checksums by hardware for incoming ICMP packets.*/
 #define CHECKSUM_GEN_ICMP               0
 #else                      //���Ч��
 /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
 #define CHECKSUM_GEN_IP                 1
 /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
 #define CHECKSUM_GEN_UDP                1
 /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
 #define CHECKSUM_GEN_TCP                1
 /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
 #define CHECKSUM_CHECK_IP               1
 /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
 #define CHECKSUM_CHECK_UDP              1
 /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
 #define CHECKSUM_CHECK_TCP              1
 /*CHECKSUM_CHECK_ICMP==1: Check checksums by hardware for incoming ICMP packets.*/
 #define CHECKSUM_GEN_ICMP               1
 #endif
 
 /*
     ----------------------------------------------
     ---------- Sequential layer options ----------
     ----------------------------------------------
 */
 /**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
 #define LWIP_NETCONN                    1            //����/�ر�netconn����
 
 /*
     ------------------------------------
     ---------- Socket options ----------
     ------------------------------------
 */
 /**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
 #define LWIP_SOCKET                     1            //����/�ر�socket����
 #define DEFAULT_UDP_RECVMBOX_SIZE       10
#define DEFAULT_TCP_RECVMBOX_SIZE       10
#define DEFAULT_ACCEPTMBOX_SIZE         10
#define DEFAULT_THREAD_STACKSIZE        1024


#define TCPIP_THREAD_NAME              "lwip"
#define TCPIP_THREAD_STACKSIZE          512
#define TCPIP_MBOX_SIZE                 8
#define TCPIP_THREAD_PRIO               3

//#define IPERF_SERVER_THREAD_NAME            "iperf_server"
//#define IPERF_SERVER_THREAD_STACKSIZE        1024
//#define IPERF_SERVER_THREAD_PRIO             0

//#define BLOCK_TIME			                250
//#define BLOCK_TIME_WAITING_FOR_INPUT	( ( portTickType ) 100 )

/*
   ----------------------------------------
   ---------- Lwip Debug options ----------
   ----------------------------------------
*/
//#define LWIP_DEBUG                      1

#endif /* __LWIPOPTS_H__ */
 
 /************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

