
# define lib dir
W5500_IO_DIR		=	$(ROOT_DIR)/Lib/ioLibrary_Driver
W5500_ETH_DIR  		= 	$(W5500_IO_DIR)/Ethernet
W5500_INTERNET_DIR  = 	$(W5500_IO_DIR)/Internet

# source director

# include director

# use libraries, please add or remove when you use or remove it.
C_SOURCES  += $(W5500_ETH_DIR)/socket.c 		
C_SOURCES  += $(W5500_ETH_DIR)/wizchip_conf.c 	
C_SOURCES  += $(W5500_ETH_DIR)/w5500.c 	
C_SOURCES  += $(W5500_INTERNET_DIR)/DHCP/dhcp.c 
C_SOURCES  += $(W5500_INTERNET_DIR)/DNS/dns.c 

# include directories
C_INCLUDES += 	$(W5500_ETH_DIR)			
C_INCLUDES +=	$(W5500_INTERNET_DIR)/DHCP
C_INCLUDES +=	$(W5500_INTERNET_DIR)/DNS
