// https://github.com/svf-tools/SVF/wiki/Analyze-a-Simple-C-Program
void swap(char **p, char **q){
    char* t = *p;
    *p = *q;
    *q = t;
}
int main(){
    char a1, b1;
    char *a = &a1;
    char *b = &b1;
    swap(&a,&b);
}
//
// Created by 何为刚 on 6/1/2025.
//
