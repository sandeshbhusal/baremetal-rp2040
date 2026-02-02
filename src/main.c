static volatile int foo = 10;

void main() {
    foo = 20;

    (void)foo;
    while(1);
    while(1);
}
