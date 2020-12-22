package com.company;

import java.io.*;
import java.net.*;
public class Server {

    static ServerSocket ss;
    static Socket s;
    static InputStreamReader isr;
    static BufferedReader br;
    static String message;

    public static void main(String[] args){
        try{
            ss=new ServerSocket(8080);
            while (true){
                s=ss.accept();//establishes connection
                isr = new InputStreamReader(s.getInputStream());
                br = new BufferedReader(isr);
                message= br.readLine();

                System.out.println(message);
            }


        }catch(Exception e){System.out.println(e);}
    }
}
