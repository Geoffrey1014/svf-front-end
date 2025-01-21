fn main() {
    bar(10);
}

fn bar(mut a: i32) {
    for i in 0..a {
        a = a - 1;
    }
}