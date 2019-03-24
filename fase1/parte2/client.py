from server import validate_port
from server import read_port

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

def read_ip_address():
	ip_address = str(input('Enter the server IP adress: '))
	return ip_address
	
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
	
#Code for testing the above functions

if validate_ip_address(read_ip_address()) and validate_port(read_port()):
	print('We can start communication!')
else:
	print('We can NOT start communication!')

while(True):
	string = input('> ')
	if validate_sentence(string):
		print("yeah")
	else:
		print("nah")
