from queue import Queue
from threading import Thread
from socket import *
import time
import re

# Communication socket between client and server, used concurrently by various execution threads.
global open_socket

# Queue to store the incoming messages from the client.
lines_received = Queue()

# Queue to store the outgoing messages to the client.
answers = Queue()

# Dictionary of the form {[IP, port_num -> sentence : sentence_words_count]} to store all the data associated with IP and port_num.
query_dict = dict()


# Function that reads in the port entered by the user, in command prompt.
def read_port():
	try:
		port = int(input('Enter the port number: '))
	except ValueError:
		print("That's not a number")
		port = -1
	return port


# Function that returns true if port_number is valid, false otherwise.
# A valid port must be an integer in range [1024, 65535].	
def validate_port(port_number):
	is_valid = False
	if port_number > 1023 and port_number < 65536:
		is_valid = True
	return is_valid

# Function that reads in the IP address entered by the user, in command prompt.
def read_ip_address(option = 'client'):
	ip_address = str(input('Enter the ' + option + ' IP adress: '))
	return ip_address

# Function that returns true if ip_address is a valid IPv4 address, false otherwise.
# A valid IPv4 address must contain four number blocks separed by points, where each number is in range [0, 255]. Ex: 192.168.0.1
def validate_ip_address(ip_address):
	is_valid = True
	if not ip_address:
		is_valid = False
		
	address_sections = ip_address.split('.')
	
	if len(address_sections) != 4:
		is_valid = False
		
	for byte in address_sections:
		number = 0
		try:
			number = int(byte)
		except ValueError:
			is_valid = False
		
		if number < 0 or number > 255:
			is_valid = False

	return is_valid

def create_socket(port_number):
	try:
		server_socket = socket(AF_INET, SOCK_STREAM)
	except Exception as error:
		print(error)
		return (-1, -1)
	try:
		server_socket.bind(('', port_number))
	except Exception as error:
		print(error)
		return (-1, -1)
	try:
		server_socket.listen(1)
	except Exception as error:
		print(error)
		return (-1, -1)
	return server_socket

# Function that returns a 3-tuple containing an open socket, the client's IP address and the port number.
def connect_to_client(server_socket):
	# accept() returns a 3-tuple containing the socket given by the server, the client's IP address and the port number.
	try:
		connection_socket, client_address = server_socket.accept()
	except Exception as error:
		print(error)
		return (-1, -1)
	return (connection_socket, client_address)

# Function to count words in a sentence, with the separators lined in the documentation.	
def count_words(sentence):
	count = 0
	in_word = False

	for current_char in sentence:
		if current_char.isdigit() or current_char.isalpha():
			if not in_word:
				in_word = True
				count += 1

		else:
			in_word = False

	return count

# Function that takes the queued received messages, counts the words of each one and then stores the results in an 2-tuple queue, to be
# sent to the client by another thread.
def operate_sentences(IP_address, port_number):
	while True:
		current_sentence = lines_received.get()
		if current_sentence != "1":
			sentence_length = count_words(current_sentence)
			answers.put((current_sentence, sentence_length))
			if (IP_address, port_number) in query_dict.keys():
				query_dict[(IP_address, port_number)].append((current_sentence, sentence_length))
			else:
				query_dict[(IP_address, port_number)] = [(current_sentence, sentence_length)]
		else:
			answers.put("1")
			break

# Function that prompts the user to enter an IP address and a port number, whenever he wants to print all the stored data associated to 
# these addresses.
def wait_input():
	print("Welcome to the counter server console")
	while True:
		print("What do you want to do?")
		print("1. Check the queries from a particular client")
		print("2. Close the server")
		option = input("Choose wisely: ")
		if option == "1":
			IP_address = read_ip_address()
			if validate_ip_address(IP_address):
				port_number = read_port()
				if validate_port(port_number):
					if (IP_address, port_number) in query_dict.keys():
						for pairs in query_dict[(IP_address, port_number)]:
							print(pairs[0] + ' -> ' + str(pairs[1]))
					else:
						print("That IP:port pair hasn't made any requests yet")
				else:
					print("That's not a valid port")
			else:
				print("That's not a valid IP address")
		elif option == "2":
			print("Please wait while we finish up")
			break
		else:
			print("That's not a valid option")

# Function that receives messages from the client, and stops when receives a -1 value. Also enqueues the received messages in a global
# double-ended queue named 'lines_received'.
def receive_message():
	while(True):
		if open_socket:
			message = open_socket.recv(1024)
			if len(message) == 0:
				lines_received.put("1")
				#open_socket.close()
				break
			else:
				lines_received.put(message.decode())
		else:
			break

# Function executed by a specific thread to send answers to the client within a given delay time.
def answer_to_client(delay):
	while True:
		sending_message = answers.get()
		time.sleep(delay)
		if len(sending_message) != 1:
			try:
				open_socket.send(str(sending_message).encode())
			except:
				break
		else:
			break
		
# Main

keep_server_alive = True

user_port = read_port()
delay = 5 #default value
if validate_port(user_port):
	try:
		delay = int(input('Enter the delay time in seconds: '))
	except ValueError:
		print("That's not a number!")
	print('I am ready to answer queries')
server_socket = create_socket(user_port)
while keep_server_alive:
	#user_thread = Thread(target = wait_input, args = ())
	#user_thread.start()
	threads = []

	client_information = connect_to_client(server_socket)
	if client_information[0] == -1:
		print(error)
		sys.exit()
	open_socket = client_information[0]
	client_address = client_information[1]
	print(str('\nConnection established with ' + str(client_address)))
	threads.append(Thread(target = receive_message, args = ()))
	threads.append(Thread(target = operate_sentences, args = (client_address[0], client_address[1])))
	threads.append(Thread(target = answer_to_client, args = [delay]))
	for thr in threads:
		thr.start()

	wait_input()

	for thr in threads:
		thr.join()
		print("Thread closed")
	threads.clear()
	print("Connection closed!")
	#user_thread.join()
	user_response = ''
	try:
		user_response = str(input("Press 'y' to keep server going, or any other key to exit: "))
	except Exception as error:
		print(error)
	if user_response == 'y':
		keep_server_alive = True
	else:
		keep_server_alive = False

else:
	print('Wrong port!')
