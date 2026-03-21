#include <stdio.h>
#include <stdlib.h>

void solve(){
    int a, b;
    if(scanf("%d %d", &a, &b) != 2) return;

    char *names[] = {"", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

    for(int i = a; i <= b; i++){
        if(i >= 1 && i <= 9){
            printf("%s\n", names[i]);
        }else if(i > 9){
            if(i % 2 == 0) printf("even\n");
            else printf("odd\n");
        }
    }
}

int main(){
    solve();
    return 0;
}
