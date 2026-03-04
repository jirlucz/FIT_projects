import socket

def setResponse(inputData):
    if inputData == 'a':        #REPLY TO AUTH
        return 'ReplY OK is MessageContent\r\n'
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

def main():
    host = '127.0.0.1'
    port = 4567

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        server_socket.bind((host, port))
        server_socket.listen(1)
        print(f"Server is listening on {host}:{port}")

        client_socket, client_address = server_socket.accept()
        print(f"Connection from {client_address} has been established.")

        while True:
            response = 4
            # Read input from stdin
            user_input = input("Enter message to send (or 'quit' to exit): ")
            if user_input == 'quit':
                client_socket.close()
                server_socket.close()
                break
            elif user_input == "1":
                response = "a"                    
            elif user_input == "2":
                response = "b"
            elif user_input == "3":
                response = "c"
            elif user_input == "4":
                response = "d"
            elif user_input == "5":
                response = "e" 
            elif user_input == "6":
                response = "f"            
            else:
                # Receive data from the client
                data = client_socket.recv(1024)
                if not data:
                    break
                print(f"Received message: {data.decode()}|")
                if data.decode() == "BYE":
                    print("Klient zaslal BYE message")
                    break
                continue
            
            print(f"Response is: {setResponse(response)}")
            client_socket.send(setResponse(response).encode())


                

    except Exception as e:
        print(f"An error occurred: {e}")

    finally:
        server_socket.close()


if __name__ == "__main__":
    main()
