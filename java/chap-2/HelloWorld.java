public class HelloWorld
{
/**
    byte   a = 8;
    short  b = 8;
    int    c = 8;
    long   d = 8;

    char    e = 8;
    double  f = 8;
    float   g = 8;
    boolean h = True;
    boolean j = TRUE;
    boolean k = true;
*/
    public native void print();
    public static void main(String args[])
    {
        new HelloWorld().print();
    }

    static {
        //System.out.println("hello World");
        System.loadLibrary("HelloWorld");
    }
}
