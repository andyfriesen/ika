
using System;

public class RLE
{
	private RLE() {}	// don't instantiate this class ;P
	
	public static void Read(byte[] dest,int count,byte[] src)
	{
		int sidx=0,didx=0;
		int n=0;
			
		do
		{
			byte w=src[sidx++];
			if (w==0xFF)
			{
				byte run=src[sidx++];
				w=src[sidx++];
				for (int i=0; i<run; i++)
					dest[didx++]=w;
				n+=run;
			}
			else
			{
				dest[didx++]=w;
				n++;
			}
		} while (n<count);
	}
	
	public static void Read(int[] dest,int count,UInt16[] src)
	{
		int sidx=0,didx=0;
		int n=0;
		
		do
		{
			UInt16 w=src[sidx++];
			if ((w&0xFF00)==0xFF00)
			{
				int run=w&0xFF;
				w=src[sidx++];
				for (int i=0; i<run; i++)
					dest[didx++]=w;
				n+=run;
			}
			else
			{
				dest[didx++]=w;
				n++;
			}
		} while (n<count);
	}
	
	// TODO: Write if I feel like it. ;P
}
