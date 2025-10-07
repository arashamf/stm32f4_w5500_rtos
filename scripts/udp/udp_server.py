
import socket
port_str = input("Port: ") # Пользователь вводит "25"
server_port = int (port_str);
server_address = '192.168.111.11'
#server_address = 'localhost'
server = (server_address, server_port)
udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp_socket.bind(server)

print ('connected:', server_address)

while 1:
 #   question = input('Do you want to quit? y\\n: ')
 #   if question == 'y': break
    print('wait data...')   
    
    data, addr = udp_socket.recvfrom(1024)  # 1024 is the buffer size
    print('client addr: ', addr)
    print(f"Received packet from {server_address}: {data.decode('utf-8')}")
    
udp_socket.close()