# import socket


data_port=4098
config_port=4096

static_ip='192.168.33.30'
adc_ip='192.168.33.180'

# cfg_dest = (adc_ip, config_port)
# cfg_recv = (static_ip, config_port)
# data_recv = (static_ip, data_port)

# # Create sockets
# config_socket = socket.socket(socket.AF_INET,
#                                     socket.SOCK_DGRAM,
#                                     socket.IPPROTO_UDP)
# data_socket = socket.socket(socket.AF_INET,
#                                          socket.SOCK_DGRAM,
#                                          socket.IPPROTO_UDP)

# # Bind data socket to fpga
# data_socket.bind(data_recv)
# # Bind config socket to fpga
# config_socket.bind(cfg_recv)

MAX_PACKET_SIZE = 4096

# data_socket.settimeout(1)

# data, addr = data_socket.recvfrom(MAX_PACKET_SIZE)

# print(data)
# print

import socket
BUFFER_LEN = 4096 #in bytes

def initUDP(IP, port):
    #Create a datagram socket
    sock = socket.socket(socket.AF_INET, # Internet
                         socket.SOCK_DGRAM) # UDP
    #Enable immediate reuse of IP address
    # sock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
    #Bind the socket to the port
    sock.bind((IP, port))
    #Set a timeout so the socket does not block indefinitely when trying to receive data
    sock.settimeout(0.5)

    return sock

def readUDP(sock):
    try:
        data, addr = sock.recvfrom(BUFFER_LEN)
        print("addr: ", addr)
    except socket.timeout as e:
        return b'Error'
    except Exception as e:
        return b'Error'
    else:
        return data

a = initUDP(static_ip, data_port)
print(readUDP(a))

