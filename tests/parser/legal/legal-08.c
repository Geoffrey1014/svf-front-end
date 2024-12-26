void bar ( int a ) {
  int i;
  for ( i = 0; i < a; i += 1 ) {
    a = a - 1;
  }
}
int main ( ) {
  bar ( 10 );
}
