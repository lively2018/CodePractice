public class LinkedListNode {
	public int data;
	public LinkedListNode next;

	public LinkedListNode(int x){
		data = x;
		next = null;
	}

	public LinkedListNode(int x) {
		data = x;
		next = null;
	}


	public LinkedListNode buildList(int[] a){

		if( a == null || a.length == 0) return null;
		LinkedListNode n = new LinkedListNode(a[0]);
		LinkedListNode head = n;
		for(int i = 1; i < a.length; ++i)
		{
			n.next = new LinkedListNode(a[i]);
			n = n.next;
		}
		return head;
	}

	public String toString() {
		return String.valueOf(data);
	}
}
