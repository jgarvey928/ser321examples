import java.net.*;
import java.io.*;

public class Client2 {
public static void main (String args[]) {
    try{

        String host = args[0];
        int port = Integer.parseInt(args[1]);
        Socket server = new Socket(host, port);

        InputStream input = server.getInputStream();
        OutputStream output = server.getOutputStream();
        BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
        
        int bufLen = 1024;
        byte bytesReceived[] = new byte[1024];
        while(true) {

            /* read from the command line */ 
            System.out.print("String to send> ");
            String strToSend = stdin.readLine();
            if (strToSend.equals("quit")) break;

            /* send to server */
            byte bytesToSend[] = strToSend.getBytes();
            output.write(bytesToSend,0,bytesToSend.length);
            
            /* read from server */
            int numBytesReceived = input.read(bytesReceived, 0, bufLen);
            String strReceived = new String(bytesReceived,0,numBytesReceived);
            System.out.println("Received from server: " + strReceived);
        }
        server.close();
    } catch(Exception e) {
        e.printStackTrace();
    }
}
}