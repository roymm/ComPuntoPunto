from socket import *

# Function that reads in the port entered by the user, in command prompt.
def read_port():
	port = int(input('Enter the port number: '))
	return port

# Function that returns true if port_number is valid, false otherwise.
# A valid port must be an integer in range [1024, 65535].
def validate_port(port_number):
	try:
		int(port_number)
	except Exception as error:
		print(error)
		return False

	if port_number > 1023 and port_number < 65536:
		return True
	else:
		return False
		
# Function that reads in the IP address entered by the user, in command prompt.
def read_ip_address():
	ip_address = str(input('Enter the server IP adress: '))
	return ip_address

# Function that returns true if ip_address is a valid IPv4 address, false otherwise.
# A valid IPv4 address must contain four number blocks separed by points, where each number is in range [0, 255]. Ex: 192.168.0.1
def validate_ip_address(ip_address):
	if not ip_address:
		return False
		
	address_sections = ip_address.split('.')
	
	if len(address_sections) != 4:
		return False
		
	for byte in address_sections:
		number = 0
		try:
			number = int(byte)
		except Exception:
			return False
		
		if number < 0 or number > 255:
			return False
	
	return True

# Function that provides an open socket, given the server ip address and the port number to communicate.	
def connect_to_server(ip_address, port_number):
	try:
		client_socket = socket(AF_INET, SOCK_STREAM)
		client_socket.connect((ip_address, port_number))
	except Exception as error:
		print(error)
		return 0
	return client_socket
	
	
# Code for testing the above functions

user_ip_address = read_ip_address()
user_port = read_port()

if validate_ip_address(user_ip_address) and validate_port(user_port):
	print('Client can start communication!')
	open_socket = connect_to_server(user_ip_address, user_port)
	open_socket.send(str('Connection established with ' + str(user_ip_address) + ' in port ' + str(user_port)).encode())
	received_message = open_socket.recv(1024)
	print('Received from the server: ' + received_message.decode())
	open_socket.close()
else:
	print('We can NOT start communication!')
