import java.util.Arrays;
import java.lang.String;

public class CheckPermutation{

	public String sort(String s){

		char[] content = s.toCharArray();
		java.util.Arrays.sort(content);
		return new String(content);
	}
	public boolean permutation(String s, String t) {

		if(s.length() != t.length())
		{
			return false;
		}

		return sort(s).equals(sort(t));
	}
	public static void main(String[] args){

		
		if(args.length != 2)
		{
			System.out.println("Usage: CheckPermutation  <surce string <dest string> \n");
		}

		CheckPermutation chPer = new CheckPermutation(); 
		if(chPer.permutation(args[0], args[1]))
		{
			System.out.println(args[1] + " is a permutation of " + args[0] + "\n");
		}
		else
		{
			System.out.println(args[1] + " isn't a permutation of " + args[0] + "\n");
		}
	}

}
