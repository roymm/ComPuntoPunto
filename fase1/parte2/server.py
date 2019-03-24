from socket import *
from collections import deque
from threading import *

lines_recived = deque()

def recive_message():
	message = ""
	while(True):
		message = (open_socket.recv(1024)).decode()
		lines_recived.append(message)
		print('Received from the client: ' + message)
		

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
		
# Function that returns a 3-tuple containing an open socket, the client's IP address and the port number.
def connect_to_client(port_number):
	try:
		server_socket = socket(AF_INET, SOCK_STREAM)
		server_socket.bind(('', port_number))
		server_socket.listen(1)
		# accept() returns a 3-tuple containing the socket given by the server, the client's IP address and the port number.
		connection_socket, client_address = server_socket.accept()
	except Exception as error:
		print(error)
		return 0
	return (connection_socket, client_address)
	
# Code for testing purposes
user_port = read_port()
if validate_port(user_port):
	print('Server can start communication!')
	client_information = connect_to_client(user_port)
	global open_socket 
	open_socket = client_information[0]
	client_address = client_information[1]
	print('Received from the client: ' + open_socket.recv(1024).decode())
	open_socket.send(str('Connection established with ' + str(client_address)).encode())
	t = Thread(target=recive_message, args=())
	t.start()
	
