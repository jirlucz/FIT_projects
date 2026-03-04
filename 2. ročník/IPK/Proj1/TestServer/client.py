import socket

def main():
    # Define the address and port of the server
    server_address = ('localhost', 4567)
    
    # Create a TCP socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        # Connect to the server
        client_socket.connect(server_address)
        
        while True:
            # Get user input for the message
            message = input("Enter your message (type 'quit' to exit): ")
            
            if message.lower() == 'quit':
                break
            
            # Send the message to the server
            client_socket.sendall(message.encode())
            
            # Receive the response from the server
            response = client_socket.recv(1024)
            print("Server response:", response.decode())
    
    finally:
        # Close the connection
        client_socket.close()

if __name__ == "__main__":
    main()
