import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Scanner;
import java.io.*;
import java.util.*;


public final class PrepData
{
  public static void main(String[] args)
  {
  	  File file = new File("Data.txt");
        StringBuffer contents = new StringBuffer();
        BufferedReader reader = null;

        try {
            reader = new BufferedReader(new FileReader(file));
            String text = null;

            // repeat until all lines is read
            while ((text = reader.readLine()) != null) {
                
                contents.append(text)
                    .append(System.getProperty(
                        "line.separator"));
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (reader != null) {
                    reader.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        
        // show file contents here
        String data = contents.toString();
        Scanner scan;
        scan = new Scanner(data);

        try
        {
          FileWriter writer = new FileWriter("newData.txt");
          while(scan.hasNext())
          {
              writer.append(scan.next());
              writer.append(" ");
          }
        }
        catch(Exception e)
        {
        }


    
  }
  
  public double stof(String inStr)
  {
  	
  	return 0.0;
  		
  }

}
