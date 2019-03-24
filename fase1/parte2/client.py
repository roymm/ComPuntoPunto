from server import validate_port
from server import read_port

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
