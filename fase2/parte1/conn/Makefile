make: message.cpp reader.cpp semaphore.cpp sender.cpp error_handler.c
	g++ -g -Wall -Wextra -o reader reader.cpp message.cpp semaphore.cpp error_handler.c shared.cpp 
	g++ -lpthread -g -Wall -Wextra -o sender sender.cpp message.cpp error_handler.c shared.cpp semaphore.cpp socket.cpp
	g++ -lpthread -g -Wall -Wextra -o  receiver receiver.cpp error_handler.c socket.cpp message.cpp
release: message.cpp reader.cpp semaphore.cpp sender.cpp error_handler.c
	g++ -Wall -Wextra -O3 -o reader reader.cpp message.cpp semaphore.cpp error_handler.c shared.cpp
	g++ -lpthread -Wall -Wextra -O3 -o sender sender.cpp message.cpp error_handler.c shared.cpp semaphore.cpp socket.cpp
	g++ -lpthread -Wall -Wextra -O3 -o receiver receiver.cpp  error_handler.c socket.cpp message.cpp
.PHONY : clean
clean :
	-rm sender
	-rm reader
	-rm receiver
