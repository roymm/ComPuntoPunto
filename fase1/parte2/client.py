from socket import *
from threading import *
import sys

global open_socket
global ans_list

# Function that reads in the port entered by the user, in command prompt.
def read_port():
	try:
		port = int(input('Enter the port number: '))
	except Exception as error:
		print(error)
		return 0
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
		
def print_dict():
	for pair in word_counts.items():
		print(pair[0] + " -> " + pair[1])

def get_sentence():
	sentence = input("> ")
	if validate_sentence(sentence):
		word_counts[sentence] = -1 #word_counts is a dictionary defined elsewhere
	else:
		print("The sentence contains an illegal character!")

def validate_sentence(sentence):
	valid_seps= [' ', '.', ',', ':', '!', '?', ';', '\t']
	valid = True
	for c in sentence:
		if not c.isalnum():
			if c not in valid_seps:
				valid = False
	return valid

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
		return -1
	return client_socket

# Function that often blocks waiting for server's responses, until server tells it there are no more answers, with a -1 value.	
def receive_answers():
	while True:
		answer = open_socket.recv(1024)
		if len(answer) > 0:
			ans_list.append(answer.decode())
		else:
			break
	
	
# Code for testing the above functions (main)

user_ip_address = read_ip_address()
user_port = read_port()
ans_list = []
if validate_ip_address(user_ip_address) and validate_port(user_port):
	open_socket = connect_to_server(user_ip_address, user_port)
	if open_socket == -1:
		sys.exit(-1)
	print('Connection established with ' + str(user_ip_address) + ' through port ' + str(user_port))
	string = ""
	thread_0 = Thread(target = receive_answers, args = ())
	thread_0.start()
	# The main execution thread reads in the user input.
	while(True):
		try:
			string = input('> ')
		except EOFError:
			print("exiting...")
			open_socket.shutdown(SHUT_RDWR)
			open_socket.close()
			print("socket closed")
			break
		if string != "1":
			if validate_sentence(string):
				open_socket.send(str(string).encode())
			else:
				print("The sentence contains an invalid character!")
		else:
			for ans in ans_list:
				print(ans)
	print("waiting on thread")
	thread_0.join()
else:
	print('IP address or port are wrong!')
