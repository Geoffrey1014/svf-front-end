int main() {
    int x, y;
    x = 1;
    y = 0;
    switch (nd())
    {
        case 0:
            x += 1;
            break;
        default:
            x++;
            y++;
            break;
        }
        return 0;
}