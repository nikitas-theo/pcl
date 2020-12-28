
int f(int &x){
	x = 10; 
	return 5;
}

int main ()
{
  int x;
  x = 9; 
  int y ;
  y = f(x); 
}

