def read_port():
	port = int(input('Enter the port number: '))
	return port

def validate_port(port_number):
	if port_number > 1023 and port_number < 65536:
		return True
	else:
		return False
	
