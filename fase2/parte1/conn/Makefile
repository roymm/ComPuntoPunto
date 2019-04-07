make: message.cpp reader.cpp semaphore.cpp sender.cpp error_handler.c
	g++ -g -Wall -Wextra -o reader reader.cpp message.cpp semaphore.cpp error_handler.c shared.cpp 
	g++ -g -Wall -Wextra -o sender sender.cpp message.cpp error_handler.c shared.cpp semaphore.cpp socket.cpp
	g++ -g -Wall -Wextra -o receiver receiver.cpp message.cpp error_handler.c shared.cpp semaphore.cpp socket.cpp
release: message.cpp reader.cpp semaphore.cpp sender.cpp error_handler.c
	g++ -Wall -Wextra -O3 -o reader reader.cpp message.cpp semaphore.cpp error_handler.c shared.cpp
	g++ -Wall -Wextra -O3 -o sender sender.cpp message.cpp error_handler.c shared.cpp semaphore.cpp socket.cpp
	g++ -Wall -Wextra -O3 -o receiver receiver.cpp message.cpp error_handler.c shared.cpp semaphore.cpp socket.cpp
.PHONY : clean
clean :
	-rm sender
	-rm reader
	-rm receiver
