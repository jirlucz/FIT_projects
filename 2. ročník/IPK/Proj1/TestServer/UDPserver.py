# TODO: Implement dynamic port allocation for subsequent communication after initial AUTH message

import socket
import sys

def setResponse(inputData):
    messID = inputData[1:3]
    print("{messID}")
    if inputData == 'a':        #REPLY TO AUTH
        return '\x00\r\n'
    elif inputData == "b":      #REPLY TO BYE
        return 'ByE\r\n'
    elif inputData == "c":      #REPLY TO MGS
        return 'Msg FROM Jirluc IS Toto je Odpoved\r\n'
    elif inputData == "d":      # ERR MSG
        return 'ERR frOM ServerName IS CvicnaChybaServeru\r\n'
    elif inputData == "e":      # !REPLY
        return 'reply NOK IS MessageContent\r\n'
    elif inputData == "f":      # !REPLY
        return 'JOIn Room1 AS Prezdivka\r\n'
    else:
        return "Korektni odpoved neni vytvorena"
    
def sendReply(input):
    messID = input[1:3]
    messID = messID.decode('utf-8')
    processedInput = input
    if input[0] == 0:
        print("Dosel CONFIRM")
        
    elif input[0] == 1:
        print("Dosel REPLY")
        
    elif input[0] == 2:
        print("Dosel AUTH")
        stringToReturn = "\x00" + messID
        return stringToReturn
        
    elif input[0] == 3:
        print("Dosel JOIN")
        
    elif input[0] == 4:
        print("Dosel MSG")
        
    elif input[0] == 254:
        print("Dosel ERR")
        
    elif input[0] == 255:
        print("Dosel BYE")
    else:
        print("Chyba sendReply input[0] je |",input[0],"|" )
        return 'ERROR'
        

def main():
    # Create a UDP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    response = "Default"
    firstLogin = 1
    dynamic_port = None

    try:
        # Bind the socket to the port
        server_socket.bind(('0.0.0.0', 4567))  # Use '0.0.0.0' to listen on all available interfaces

        print("UDP server is listening on port 4567...")

        while True:
            # Receive data from the client
            data, address = server_socket.recvfrom(2000)  # Buffer size is 2000 bytes
            print("Received data from {}: {}".format(address, data.decode()))
            
            if firstLogin == 1:
                firstLogin = 0
                # Allocate a dynamic port for subsequent communication
                try:
                    server_socket.close()
                    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                    server_socket.bind(('0.0.0.0', 5678))  # Use '0.0.0.0' to listen on all available interfaces
                    print("UDP server is listening on port 5678...")
                except:
                    print("Chyba dyn portu")
                
            
            response = sendReply(data)
            
            # Sending the user input response back to the client
            
            print("Sending response")
            server_socket.sendto(response.encode(), address)

    except KeyboardInterrupt:
        print("\nCtrl+C pressed. Closing the server...")
        server_socket.close()
        sys.exit()

if __name__ == "__main__":
    main()
