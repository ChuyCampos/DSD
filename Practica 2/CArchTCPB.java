import javax.swing.JFileChooser;
import java.net.*;
import java.io.*;

public class CArchTCPB {
    public static void main(String[] args){
        try{
            BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
            System.out.printf("Escriba la dirección del servidor:");
            String host = br.readLine();
            System.out.printf("\n\nEscriba el puerto:");
            int pto = Integer.parseInt(br.readLine());
            Socket cl = new Socket(host, pto);
            JFileChooser jf = new JFileChooser(".");
            jf.setMultiSelectionEnabled(true);
            int r = jf.showOpenDialog(null);

            if (r==JFileChooser.APPROVE_OPTION){
                File [] files = jf.getSelectedFiles();  //Manejador
                DataOutputStream dos = new DataOutputStream(cl.getOutputStream());
                dos.writeInt(files.length);
                dos.flush();
                for(File f : files){
                    String archivo = f.getAbsolutePath(); //Dirección
                    String nombre = f.getName(); //Nombre
                    long tam = f.length();  //Tamaño
                    DataInputStream dis = new DataInputStream(new FileInputStream(archivo));          
                    dos.writeUTF(nombre);
                    dos.flush();               
                    dos.writeLong(tam);
                    dos.flush();
                    byte[] b = new byte[1024];
                    long enviados = 0;
                    int porcentaje, n;
                    while (enviados < tam){
                        n = dis.read(b);
                        dos.write(b,0,n);
                        dos.flush();
                        enviados = enviados+n;
                        porcentaje = (int)(enviados*100/tam);
                        System.out.print("Enviado: "+porcentaje+"%\r");
                    }//While
                    dos.flush();
                    System.out.print("\n\nArchivo enviado");
                    
                    dis.close();
                }
                dos.close();
                
               
                cl.close();
            }//if
        }catch(Exception e){
            e.printStackTrace();
        }
    }
}



