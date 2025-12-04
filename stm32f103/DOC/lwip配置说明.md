- 内存与缓冲
  
  - MEM_ALIGNMENT=4 ：lwIP 堆内存按 4 字节对齐（Cortex-M）。
  - MEM_SIZE=10*1024 ：lwIP 堆大小。你的 W5500 接收路径使用 PBUF_RAM 分配整帧，10KB 可同时容纳约 6 个 1.6KB 帧，流量稍大可能不足。
  - MEMP_NUM_PBUF=16 ： memp pbuf 结构体数量，常用于 PBUF_REF 等引用型缓冲。
  - MEMP_NUM_SYS_TIMEOUT=10 ：并发超时事件数量上限。
  - PBUF_POOL_SIZE=4 、 PBUF_POOL_BUFSIZE≈TCP_MSS+头部 ：pbuf 池配置。当前驱动未使用 PBUF_POOL （使用 PBUF_RAM ），此项影响有限。
- TCP/IP 功能
  
  - LWIP_TCP=1 、 LWIP_UDP=1 、 LWIP_ICMP=1 ：启用 TCP、UDP、ICMP（IPv4 Ping）。
  - TCP_MSS=1460 ：以太网下最大 TCP 段长度（MTU 1500）。
  - TCP_SND_BUF=4*MSS 、 TCP_WND=4*MSS ：发送缓冲与接收窗口，各约 5840 字节，影响 RAM 占用与吞吐。
  - TCP_SND_QUEUELEN=8*(SND_BUF/MSS) ：发送队列长度（pbuf），需至少 ≥ 2*(SND_BUF/MSS) 。
  - TCP_QUEUE_OOSEQ=0 ：禁用乱序段排队，节省内存。
- DHCP 与校验和
  
  - LWIP_DHCP=0 ：静态 IP 模式（当前工程）。
  - 校验和：软件生成/校验（ CHECKSUM_GEN_* 、 CHECKSUM_CHECK_* 均为 1）；更稳妥适配 W5500 MACRAW。
- API 层与线程
  
  - LWIP_NETCONN=1 、 LWIP_SOCKET=1 ：启用 Netconn 与 Socket API，易用但增加堆/栈占用。
  - DEFAULT_*_MBOX_SIZE=10 ：各消息箱容量，影响突发能力与 RAM。
  - TCPIP_THREAD_NAME="lwip" 、 TCPIP_THREAD_STACKSIZE=1024 、 TCPIP_MBOX_SIZE=8 、 TCPIP_THREAD_PRIO=(32-1) ：TCP/IP 线程配置。你的驱动输入线程优先级同为 31。
- 统计与回调
  
  - LWIP_STATS=0 ：关闭统计，少量减负。
  - LWIP_NETIF_LINK_CALLBACK=0 ：未启用链路状态回调；按需可开。
RAM 消耗热点（结合你的工程）

- lwIP 堆 MEM_SIZE=10KB （User/arch/lwipopts.h:31）
  
  - 接收路径在 Bsp/w5500/ethernetif.c:218-227 使用 pbuf_alloc(PBUF_RAW, framelen, PBUF_RAM) ，即直接从 lwIP 堆申请整帧缓冲。每帧约 1.6KB，加上 pbuf 元数据开销，堆容易成为瓶颈。
  - 建议：若保持 PBUF_RAM ，可酌情提升 MEM_SIZE 至 16–24KB；或将驱动改为 PBUF_POOL 并增大 PBUF_POOL_SIZE （例如 8–16），更利于并发稳定。
- FreeRTOS 堆 configTOTAL_HEAP_SIZE=20KB （FreeRTOS/include/FreeRTOSConfig.h）
  
  - 任务栈： ETHIN 输入线程 2048（Bsp/w5500/ethernetif.h:8）、TCP/IP 线程 1024（User/arch/lwipopts.h:174）、应用任务若干（User/main.c:84-96）。
  - 内核对象：信号量、消息队列（默认大小 10）、各线程消息箱等。
  - 建议：按实际栈使用水位适度压缩栈；邮箱容量在不丢消息前提下收敛。
- lwIP 静态池（ .bss ）
  
  - MEMP_NUM_TCP_SEG=32 （User/arch/lwipopts.h:47）、 MEMP_NUM_TCP_PCB/PCB_LISTEN 等，固定占用 RAM。增大能提升吞吐但会增加静态内存开销。
  - 你的驱动不使用 PBUF_POOL ，相关配置影响较小。
- TCP 缓冲与窗口
  
  - TCP_SND_BUF 、 TCP_WND 直接决定可能持有的数据量上限，叠加 tcp_seg 、pbuf 链表也会消耗堆与静态池。