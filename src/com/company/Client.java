package com.company;

import java.io.*;
import java.net.*;
public class Client {
    public static void main(String[] args) {
        try{
            // host IP is the IPv4 address of your computer
            Socket s=new Socket("192.168.0.104",8080);
            DataOutputStream dout=new DataOutputStream(s.getOutputStream());
            dout.writeUTF("Hello Server");
            dout.flush();
            dout.close();
            s.close();
        }catch(Exception e){System.out.println(e);}
    }
}
