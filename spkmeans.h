#ifndef SPK_H_
#define SPK_H_

typedef struct eigen_ret{
    double * eigen_values;
    double** eigen_vectors;
    int k;
} EIGEN;

typedef EIGEN* EIGEN_LINK;

int kmeans_goal(double** points, char* goal, int vec_num, int dim);

EIGEN_LINK get_spk_points(double** points, int dim, int vec_num, int k);

void kmeans(double** points, double** centers, int vec_cnt, int k, int max_iter);

#endif // SPK_H_