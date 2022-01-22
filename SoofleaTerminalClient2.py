import socket                   

port = 8999              
s = socket.socket()        
 
s.connect(('s00flea.go.ro', port))      
 
while True:
    shellStatus = "[ss]"
    s.send(shellStatus.encode())
    response = s.recv(1024).decode()
    print(response)
    command = input("command:")

    if(command == "(close)"):
        s.close()
        break
    else:
        s.send(command.encode())
        response = s.recv(1024).decode()
        print(response)
 

