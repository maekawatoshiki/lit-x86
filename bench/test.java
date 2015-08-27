class fibo {
	public static int rec_fibo(int n) {
		if(n < 2) return 1;
		else return rec_fibo(n - 1) + rec_fibo(n - 2);
	}

	public static void main(String args[]) {
		System.out.println(rec_fibo(35));
	}
}
