void bar ( int a ) {
  int i;
  for ( i = 0; i < a; i += 1 ) {
    if( i == 5 ) {
      a = 9;
      break;
    }
    a = a - 1;
  }
}
int main ( ) {
  bar ( 10 );
}
