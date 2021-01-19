im_server : main.o msg.pb.o Login.o Global.o Contact.o Text.o User.o Server.o
	g++ -o server main.o msg.pb.o Global.o Login.o Contact.o User.o Server.o Text.o -I -L -std=c++11 -ludt -lstdc++ -lpthread -lprotobuf -lpqxx -lpq
main.o : main.cpp
	g++ -c main.cpp
msg.pb.o : msg.pb.cc
	g++ -c msg.pb.cc
Login.o : Login.cpp
	g++ -c Login.cpp
Text.o : Text.cpp
	g++ -c Text.cpp
User.o : User.cpp
	g++ -c User.cpp
Server.o : Server.cpp
	g++ -c Server.cpp
Contact.o : Contact.cpp
	g++ -c Contact.cpp
Global.o : Global.cpp
	g++ -c Global.cpp

clean:
	rm *.o


