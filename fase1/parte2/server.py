from socket import *
from collections import deque
from threading import *
import re
import time
import sys

# Communication socket between client and server, used concurrently by various execution threads.
global open_socket

# Boolean variable to let other threads know if the client stopped sending messages.
global strings_finished

# Queue to store the incoming messages from the client.
lines_received = deque()

# Queue to store the outgoing messages to the client.
sent_lines = deque()

# Dictionary of the form {[IP, port_num -> sentence : sentence_words_count]} to store all the data associated with IP and port_num.
dictionary = []


# Function that receives messages from the client, and stops when receives a -1 value. Also enqueues the received messages in a global
# double-ended queue named 'lines_received'.
def receive_message():
	message = ""
	while(True):
		message = (open_socket.recv(1024)).decode()
		lines_received.append(message)
		
		if message == "-1":
			strings_finished = True
			open_socket.send(str("Please wait until server processes all the queries!").encode())
			break;

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
def read_ip_address(option = 'client'):
	ip_address = str(input('Enter the ' + option + ' IP adress: '))
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
	
# Temporal function to count words in a sentence, doesn't work as it is supposed to.	
def count_words(sentence, maxsplit = 0):
	delimiters = [' ', '.', ',', ':', '!', '?', ';', '\t']
	if sentence:
		regexPattern = '|'.join(map(re.escape, delimiters))
		split_sentence = re.split(regexPattern, sentence, maxsplit)
		return len(split_sentence)
		
# Function that takes the queued received messages, counts the words of each one and then stores the results in an 2-tuple queue, to be
# sent to the client by another thread.
def operate_sentences(IP_address, port_number):
	while '-1' not in lines_received or not strings_finished:
		if lines_received:
			current_sentence = lines_received.pop()
			if current_sentence != '-1':
				sentence_length = count_words(current_sentence)
				sent_lines.append((current_sentence, sentence_length))
				dictionary.append([IP_address, port_number, current_sentence, sentence_length])
			else:
				sent_lines.append(('-1', '-1'))
				break
			
# Function executed by a specific thread to send answers to the client within a given delay time.
def answer_to_client(delay, none):
	while True:
		if sent_lines:
			sending_message = sent_lines.popleft()
			time.sleep(delay)
			if '-1' in sending_message:
				open_socket.send(str(sending_message).encode())
				break
			else:
				open_socket.send(str(sending_message).encode())	
				
# Function that prompts the user to enter an IP address and a port number, whenever he wants to print all the stored data associated to 
# these addresses.
def wait_input():
	while True:
		if dictionary:
			IP_address = read_ip_address()
			if validate_ip_address(IP_address):
				port_number = read_port()
				if validate_port(port_number):
					index = 0
					for index in range(0,len(dictionary)):
						if IP_address == dictionary[index][0]:
							if port_number == dictionary[index][1]:
								print (IP_address + ', ' + str(port_number) + ' : ' + dictionary[index][2] + ' -> ' + str(dictionary[index][3]))
							else:
								print('Error! Unknown port number!')
						else:
							print('Error! Unknown IP address!')
					verification = str(input('Continue? (y/n)'))
					if verification == 'n':
						break
					elif verification == 'y':
						continue
					else:
						print('Unknown option!')
				else:
					print('Invalid port number!')
			else:
				print('Invalid IP address!')
			

# Code for testing purposes (main)

user_port = read_port()
if validate_port(user_port):
	delay = 0
	try:
		delay = int(input('Enter the delay time in seconds: '))
	except Exception as error:
		print('You entered a wrong value!')
		sys.exit()
	strings_finished = False
	print('Server can start communication!')
	try:
		client_information = connect_to_client(user_port)
	except Exception as error:
		print(error)
		sys.exit()
	open_socket = client_information[0]
	client_address = client_information[1]
	print(str('Connection established with ' + str(client_address)))
	thread_0 = Thread(target = receive_message, args = ())
	thread_0.start()
	thread_1 = Thread(target = operate_sentences, args = (client_address[0], client_address[1]))
	thread_1.start()
	thread_2 = Thread(target = answer_to_client, args = (delay, 0))
	thread_2.start()
	
	# Main thread waits for user input, whenever the user wants to print the list of sentences and lengths stored so far.
	wait_input()
	
	thread_0.join()
	thread_1.join()
	thread_2.join()
	open_socket.close()
	
	print('Connection with ' + str(client_address) + ' has been closed successfully!')
else:
	print('Wrong port!')
	
