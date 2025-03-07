import java.net.*;
import java.io.*;

public class SArchTCPB {
    public static void main(String[] args){
        try{
            ServerSocket s = new ServerSocket(7000);
            System.out.println("Servidor preparado.");
            for(;;){
                Socket cl = s.accept();
                System.out.println("Conexión establecida desde"+cl.getInetAddress()+":"+cl.getPort());
                DataInputStream dis = new DataInputStream(cl.getInputStream());
                int nAr= dis.readInt();
                for(int i=0;i<nAr;i++){
                    System.out.println(nAr);
                    byte[] b = new byte[1028];
                    String nombre = dis.readUTF();
                    System.out.println("Recibimos el archivo:"+nombre);
                    long tam = dis.readLong();
                    DataOutputStream dos = new DataOutputStream(new FileOutputStream(nombre));
                    long recibidos=0;
                    int n, porcentaje;
                    while(recibidos < tam){
                        n = dis.read(b);
                        dos.write(b,0,n);
                        dos.flush();
                        recibidos = recibidos + n;
                        porcentaje = (int)(recibidos*100/tam);
                        System.out.print("Recibido: "+porcentaje+"%\r");
                    }//While
                    System.out.print("\n\nArchivo recibido.\n");
                    dos.close();
                }
                dis.close();

                cl.close();
            }
        }catch(Exception e){
                e.printStackTrace();
        }//catch
    }
}


