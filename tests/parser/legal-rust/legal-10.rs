fn main() {
    bar(10);
}

fn bar(mut a: i32) {
    let i: i32 = 0;
    while a != 0 {
        println!("{}", a);
        a += a + a / a + String::from(a).chars.count();
        println!("{}", i);
        if a == 4 {
            break;
        }
    }
}