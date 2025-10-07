
import socket

# Server address and port
SERVER_IP = '192.168.111.197'  # Localhost
SERVER_PORT = 1111

# Create a UDP socket
# AF_INET specifies the address family (IPv4)
# SOCK_DGRAM specifies the socket type (UDP)
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    while True:
        message = input("Enter message to send (or 'exit' to quit): ")

        if message.lower() == 'exit':
            break

        # Encode the message to bytes and send it to the server
        client_socket.sendto(message.encode(), (SERVER_IP, SERVER_PORT))

        # Receive a response from the server
        # The buffer size (1024) is the maximum amount of data to be received at once
        #response, server_address = client_socket.recvfrom(1024)
        #print(f"Received response from {server_address}: {response.decode()}")

except Exception as e:
    print(f"An error occurred: {e}")

finally:
    # Close the socket
    client_socket.close()
    print("UDP client closed.")