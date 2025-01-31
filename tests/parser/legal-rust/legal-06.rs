static mut A: i32 = 0;

fn add(a: i32, b: i32) -> i32 {
    a + b * a
}

fn main() {
    unsafe {
        A = add(2, 3);
    }
}
