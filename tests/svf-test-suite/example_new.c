void swap(char *p){
    *p = 'b';
    char c = *p;
}
int main(){
    char a;
    swap(&a);
}
