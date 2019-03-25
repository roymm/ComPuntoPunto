from socket import *
from collections import deque
from threading import *
import re
import time
import sys

global open_socket
global strings_finished
lines_received = deque()
sent_lines = deque()


# Function that receives messages from the client, and stops when receives a -1 value. Also enqueues the received messages in a global
# double-ended queue named 'lines_received'.
def receive_message():
	message = ""
	while(True):
		message = (open_socket.recv(1024)).decode()
		lines_received.append(message)
		print('Received from client: ' + message)
		
		if message == "-1":
			strings_finished = True
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
		
# Function that takes the received messages queued, counts the words of each one and then stores the results in a dictionary.
def operate_sentences():
	while '-1' not in lines_received or not strings_finished:
		if lines_received:
			current_sentence = lines_received.pop()
			if current_sentence != '-1':
				sentence_length = count_words(current_sentence)
				sent_lines.append((current_sentence, sentence_length))
			else:
				sent_lines.append(('-1', '-1'))
				break
			#print(current_sentence + ' -> ' + str(sentence_length))
			
# Function executed by a specific thread to send answers to the client within a given delay time.
def answer_to_client(delay, none):
	while True:
		if sent_lines:
			sending_message = sent_lines.pop()
			time.sleep(delay)
			if '-1' in sending_message:
				open_socket.send(str(sending_message).encode())
				break
			else:
				open_socket.send(str(sending_message).encode())	

# Code for testing purposes

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
	print('Received from the client: ' + open_socket.recv(1024).decode())
	open_socket.send(str('Connection established with ' + str(client_address)).encode())
	thread_0 = Thread(target=receive_message, args=())
	thread_0.start()
	thread_1 = Thread(target = operate_sentences, args = ())
	thread_1.start()
	thread_2 = Thread(target = answer_to_client, args = (delay, 0))
	thread_2.start()
	
	thread_0.join()
	thread_1.join()
	thread_2.join()
	open_socket.close()
	print('Connection with ' + str(client_address) + ' has been closed successfully!')
else:
	print('Wrong port!')
	
