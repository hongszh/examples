
import java.text.NumberFormat;


public class Test {

    public static void main(String[] args) {
        int a =  16;  //00010000
        int c = -16;  //11101111

        int b1 = 0b00111111;
        int b2 = 0b11110000;

        System.out.println("二进制输出"+Integer.toBinaryString(a));
	System.out.println("八进制输出"+Integer.toOctalString(a));

        System.out.println("b1:00111111 的移位结果, 二进制输出：" + Integer.toBinaryString(b1>>2) );
        System.out.println("b2:11110000 的移位结果, 二进制输出：" + Integer.toBinaryString(b2>>2) );


	System.out.println("-------------------------------");

        int argb = 0xf0000000;
        int a0 = (argb & 0xff000000) >>> 24;
        int a2 = (argb & 0xff000000) >> 24;
        int a1 = argb & 0xff000000;

	System.out.println("a0:(argb,                      二进制输出：" + Integer.toBinaryString(argb) );
	System.out.println("ff:(0xff000000,                二进制输出：" + Integer.toBinaryString(0xff000000) );
	System.out.println("a1:argb & 0xff000000           二进制输出：" + Integer.toBinaryString(a1) );
        System.out.println("a0:(argb & 0xff000000) >>> 24, 二进制输出：" + Integer.toBinaryString(a0) );
	System.out.println("a2:(argb & 0xff000000) >> 24,  二进制输出：" + Integer.toBinaryString(a2) );
        System.out.println("a1:argb & 0xff000000          十六进制输出：" + Integer.toHexString(a1) );
        System.out.println("a2:(argb & 0xff000000) >> 24, 十六进制输出：" + Integer.toHexString(a2) );
        System.out.println("a0:(argb & 0xff000000) >>> 24,  十进制输出：" + a0 );
        System.out.println("a2:(argb & 0xff000000) >> 24,   十进制输出：" + a2);


	
	System.out.println("-------------------------------");

        int a00 = (argb >> 24) & 0xFF;
        int a01 = (argb >> 24);

        System.out.println("a00:(argb >> 24) & 0xFF, 二进制输出：" + Integer.toBinaryString(a00) );
	System.out.println("a01:(argb >> 24)         二进制输出：" + Integer.toBinaryString(a01) );


	System.out.println("-------------------------------");

        int b3 = 0b011111110000;
        int b4 = 0b111111110000;

        System.out.println("b3:011111110000, b3>>10 二进制输出：" + Integer.toBinaryString(b3>>10) );
	System.out.println("b4:111111110000  b4>>10 二进制输出：" + Integer.toBinaryString(b4>>10) );

	System.out.println("-------------------------------");


        long a000 = ((argb & 0x0FF000000l) >> 24) & 0xFF;
        long a001 = (argb >> 24);
        long a002 = argb & 0x0FF000000l;
        long a003 = (argb & 0x0FF000000l) >> 24;

	System.out.println("000:(argb & 0x0FF000000l) 　                二进制输出：" + a002);
	System.out.println("000:(argb & 0x0FF000000l) >> 24             二进制输出：" + a003);
        System.out.println("000:((argb & 0x0FF000000l) >> 24) & 0xFF 　 十进制输出：" + a000);

	System.out.println("-------------------------------");
        
        String s1 = String.format("%1.2f", 1.1256);
        String s2 = String.format("%1.2f", 12.1256);
        System.out.println("s1 : " + s1);
        System.out.println("s2 : " + s2);

	System.out.println("-------------------------------");

	String s3 = new java.text.DecimalFormat("#.00").format(12.1256);
	String s4 = new java.text.DecimalFormat("#.000").format(12.1256);
	String s5 = new java.text.DecimalFormat("#.00").format(1.1256);
        System.out.println("s3 : " + s3);
        System.out.println("s4 : " + s4);
        System.out.println("s5 : " + s5);

        NumberFormat numFormat = NumberFormat.getInstance();
        numFormat.setMaximumFractionDigits(2);

	System.out.println("numFormat(12.1256) : " + numFormat.format(12.1256));
	System.out.println("numFormat(10000) : " + numFormat.format(12.10000));
	System.out.println("numFormat(12.125) : " + numFormat.format(12.125));

    }
}

