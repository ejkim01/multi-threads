#include <stdio.h>

int main() {
    FILE* out;
    out = fopen("number.dat", "w");
    if (out==NULL)
        return 1;
    fprintf(out, "20");
    fseek(out, 0, SEEK_SET);
    fprintf(out, "19");

    fclose(out);
    return 0;
}
