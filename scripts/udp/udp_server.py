
import socket
port_str = input("Port: ") # user port input
SERVER_PORT = int (port_str);
SERVER_IP = '192.168.111.11'
server = (SERVER_IP, SERVER_PORT)
# AF_INET specifies the address family (IPv4)
# SOCK_DGRAM specifies the socket type (UDP)
udp_server_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp_server_sock.bind(server)

print ('connected:', SERVER_IP)

try:
    while True:
       #question = input('Do you want to quit? y\\n: ')
       #if question == 'y': break
       print('wait data...')   
       data, addr = udp_server_sock.recvfrom(1024)  # 1024 is the buffer size
       print('client addr: ', addr)
       print(f"Received packet from {SERVER_IP}: {data.decode('utf-8')}")

finally:    
    udp_server_sock.close() # Close the socket
    print("UDP server closed.")