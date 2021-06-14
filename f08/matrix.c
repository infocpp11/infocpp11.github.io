double **read_matrix(char const *filename) {
    FILE *fp;
    fp = fopen(filename, "rt");
    if (fp == NULL)
        return NULL;
    int w, h;
    if (fscanf(fp, "%d %d", &w, &h) != 2) {
        fclose(fp);
        return NULL;
    }
    double ** ret;
    ret = (double **) malloc(sizeof(double*) * h);
    if (ret == NULL) {
        fclose(fp);
        return NULL;
    }
    for (int y = 0; y < h; ++y) {
        ret[y] = (double *) malloc(sizeof(double) * w);
        if (ret[y] == NULL) {
            for (int yy = 0; yy < y; ++yy)
                free(ret[yy]);
            free(ret);
            fclose(fp);
            return NULL;
        }
    }
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (fscanf(fp, "%lf", &ret[y][x]) != 1) {
                for (int yy = 0; yy < h; ++yy)
                    free(ret[yy]);
                free(ret);
                fclose(fp);
                return NULL;
            }
        }
    }
    fclose(fp);
    return ret;
}
