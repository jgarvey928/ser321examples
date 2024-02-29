import java.net.*;
import java.io.*;

public class Server2 {
public static void main (String args[]) {
try{
    int port = Integer.parseInt(args[0]);
    Socket clientSock;
    ServerSocket sock = new ServerSocket(port);

    int bufLen = 1024;
    byte clientInput[] = new byte[bufLen]; // up to 1024 bytes in a message.
    while(true) {
            System.out.println("Server waiting for a connection");
            clientSock = sock.accept(); // blocking wait
            System.out.println("Server connected to client");

            PrintWriter out = new PrintWriter(clientSock.getOutputStream(), true);
            InputStream input = clientSock.getInputStream();
            
            int numr = input.read(clientInput, 0, bufLen);
            while (numr != -1) {
              String received = new String(clientInput, 0, numr);
              System.out.println("read from client: " + received);
              out.println(received);
              numr = input.read(clientInput, 0, bufLen);
            }
            input.close();
            clientSock.close();
            System.out.println("Socket Closed.");
    }     
} catch(Exception e) {
    e.printStackTrace();
}
}
}