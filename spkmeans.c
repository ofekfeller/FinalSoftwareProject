#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


struct linked_list
{
    double data;
    struct linked_list *next;

};

typedef struct linked_list ELEMENT;
typedef ELEMENT* LINK;

struct points_linked_list
{
    double *vec;
    struct points_linked_list *next;

};

typedef struct points_linked_list MAIN_ELEMENT;
typedef MAIN_ELEMENT* MAIN_LINK;


typedef struct cen_info
{
double *sum;
int cnt;
} CEN_INFO;

typedef CEN_INFO* CEN_LINK;

CEN_LINK init_clusters(int k, int dim){
    CEN_LINK clusters;
    int i;

    clusters = (CEN_INFO*)calloc(k, sizeof(CEN_INFO));
    for ( i = 0; i < k; i++)
    {
        clusters[i].cnt = 0;
        clusters[i].sum = calloc(dim, sizeof(double));
        assert(clusters[i].sum != NULL);
    }

    return clusters;
}

void add_array(double *source, double *new, int dim){
    int i;
    double tmp;

    for (i = 0; i < dim; i++)
    {
        tmp = source[i] + new[i];
        source[i] = tmp;
    }

}

void sub_array(double *source, double *new, int dim){
    int i;
    double tmp;

    for (i = 0; i < dim; i++)
    {
        tmp = source[i] - new[i];
        source[i] = tmp;
    }

}

void update_center(double* origin, CEN_INFO cluster, int dim){
    int i;

    for (i = 0; i < dim; i++)
    {
        origin[i] = (cluster.sum[i] / cluster.cnt);
    }

}

double sec_norm(double *array1,double *array2, int size){
    double sum;
    double res;
    int i;
    sum=0;

    for (i = 0; i < size; i++)
    {
        res = array1[i]-array2[i];
        sum += (res*res);
    }

    return sum;
}


int classify(double *vec,int num_cent, int size_vec, double **centers){
    int min_ind=0;
    double tmp_norm=0;
    double min_norm;
    int i;

    min_norm = sec_norm(vec, centers[0], size_vec);


    for (i=1; i<num_cent; i++){

        tmp_norm = sec_norm(vec,centers[i],size_vec);

        if (tmp_norm<min_norm){
            min_norm=tmp_norm;
            min_ind=i;
        }
    }
    return min_ind;
}

int update_row(double * to, double * from, int k){
    int i;
    int cnt=0;
    for (i=0; i<k;i++){
        if (from[i]!=to[i]){
            to[i]=from[i];
            cnt++;
        }
    }
    if (cnt>0){
        return 1;
    }
    else{
        return 0;
    }
}

double* sub_vectors(double *vec1,double *vec2, int n){
    int i;
    double* vec=calloc(n,sizeof(double));
    assert(vec!=NULL);
    for (i=0; i<n;i++){
        vec[i]=vec1[i]-vec2[i];
    }
    return vec;
}

void print_vec(double* vec, int len){
    int i;
    char sign;
    for(i=0;i<len;i++){
            if (i==len-1) sign='\n';
            else sign = ',';
            printf("%.4f%c", vec[i], sign);
    }
}

void print_mat(double** mat, int n, int m){
    char sign;
    int i, j;
    double prt;

    for (i=0; i<n; i++){
    
        for (j=0; j < m; j++)
        {
            if (j==m-1) sign='\n';
            else sign = ',';

            if(mat[i][j]<0 && mat[i][j]>-0.00005){
                prt = 0;
            }
            else{
                prt = mat[i][j];
            }
            
            printf("%.4f%c", prt, sign);
        }
    }

}

/*
compute norm of a vector
*/
double norm(double *vec, int size){
    double sum;
    int i;
    sum=0;

    for (i = 0; i < size; i++)
    {
        sum += (vec[i]*vec[i]);
    }
    
    return sqrt(sum);
}

/*
given the norm of two vectors, will computer the weight in the matrix
*/
double p_exp(double *vec1,double *vec2, int size){
    double my_norm;
    double *vec=sub_vectors(vec1,vec2,size);
    my_norm=norm(vec, size);
    my_norm=-my_norm/2;
    free(vec);
    return exp(my_norm);
}
/*
divide each cordinate of the vector by the norm of the vector
*/
void normalize(double* vec, int size_){
    int i;
    double vec_norm;
    vec_norm=norm(vec,size_);

    for (i=0;i<size_;i++){
        vec[i]=vec[i]/vec_norm;
    }
}

/*
 normalize matrix by its rows
 */
void normalize_mat(double** mat, int rows, int cols){
    int i;
    for(i=0;i<rows;i++){
        normalize(mat[i], cols);
    }
}

/*
return new matrix that is a substraction of two matrixes
*/
double** matrix_sub(double** mat1, double** mat2, int n){
    double *a;
    double **mat;
    int i;
    int j;
    a=calloc(n*n,sizeof(double));
    assert(a!=NULL);
    mat=calloc(n,sizeof(double*));
    assert(mat!=NULL);
    for (i=0;i<n;i++){
        mat[i]= a + i*n;
    }
    for (i=0;i<n;i++){
        for (j=0;j<n;j++){
            mat[i][j]= mat1[i][j] - mat2[i][j];
        }
    }
    return mat;
}

/*
return new matrix that is a multiplication of two matrixes
*/
double** matrix_mul(double** mat1, double** mat2, int x, int y, int z){
    double *a;
    double **mat;
    int i,j,k;
    a=calloc(x*z , sizeof(double));
    assert(a!=NULL);
    mat=calloc(x,sizeof(double*));
    assert(mat!=NULL);
    for (i=0;i<x;i++){
        mat[i]= a + i*z;
    }

    for (i = 0; i < x; i++) {
        for (j = 0; j < z; j++) {
            mat[i][j] = 0;
            for (k = 0; k < y; k++)
                mat[i][j] += mat1[i][k] * mat2[k][j];
        }
    }
    return mat;
}
/*
return new matrix that is a multiplication of two square matrixes
*/
double** sq_matrix_mul(double** mat1, double** mat2, int n){
    return matrix_mul(mat1, mat2, n, n, n);
}

/*
given a vector and matrix- multiply each matrix row with the corresponding vector value
*/
void mul_lines(double** mat, double* vec, int dim){
    int j,i;
    for(i=0; i<dim; i++){
          for(j=0; j<dim; j++){
              mat[i][j] *= vec[i];
    }
}
}

/*
given a vector and matrix- multiply each matrix column with the corresponding vector value
*/
void mul_columns(double** mat, double* vec, int dim){
    int i,j;
    for(i=0; i<dim; i++){
          for(j=0; j<dim; j++){
              mat[i][j] *= vec[j];
    }
}
}
/*
given set of points compute the weight between each and store them in wheightened adjacency matrix
*/
double** weighted_matrix(double** points, int dim, int vec_num){
    double *a;
    double **mat;
    int i;
    int j;
    a=calloc(vec_num*vec_num,sizeof(double));
    assert(a!=NULL);
    mat=calloc(vec_num,sizeof(double*));
    assert(mat!=NULL);
    for (i=0;i<vec_num;i++){
        mat[i]= a + i*vec_num;
    }
    for (i=0; i<vec_num;i++){
        for (j=i; j<vec_num; j++){
            if(i==j){
                mat[i][j]=0;
            }
            else{
            mat[i][j]=p_exp(points[i],points[j],dim);
            mat[j][i]=mat[i][j];
            }
        }
    }
    return mat;
}
/*
given a matrix and an index, return the sum of the elements in the row of the index in the matrix
*/
double sum_line(double **mat, int n, int index){
    double sum=0;
    int i;
    for (i=0; i<n; i++){
        sum+=mat[index][i];
    }
    return sum;
}
/*
given a matrix, return a vector where each cordinate i, is the sum of row i in the matrix
*/
double* vector_sum(double** mat, int n){
    double* vec;
    int i;
    vec=calloc(n,sizeof(double));
    assert(vec!=NULL);
    for (i=0;i<n;i++){
        vec[i]=sum_line(mat,n,i);
    }
    return vec;
}


/*
given vector, calculate for each val - 1/sqrt(val)
*/
void div_square_vec(double* vec, int dim){
    int i;
    for(i=0; i<dim; i++){
        vec[i] = 1/sqrt(vec[i]);
    }
}


/*
given a vector of sums, return the diagonal degree matrix
*/
double** get_diag_mat(double* vec, int n){
    double** mat;
    double* a;
    int i;
    int j;
    a = calloc(n*n,sizeof(double));
    assert(a!=NULL);
    mat=calloc(n,sizeof(double*));
    assert(mat!=NULL);
    for (i=0;i<n;i++){
        mat[i]= a + i*n;
    }
    for (i=0; i<n; i++){
        for (j=0; j<n;j++){
            if (i==j){
                mat[i][i]=vec[i];
            }
            else{
                mat[i][j]=0;
            }
        }
    }
    return mat;
}
/*
given a matrix, return it's transpose
*/
double** transpose(double **mat, int n, int m){
    double *a;
    double **mat_t;
    int i;
    int j;
    a=calloc(n*m,sizeof(double));
    assert(a!=NULL);
    mat_t=calloc(m,sizeof(double*));
    assert(mat_t!=NULL);
    for (i=0;i<m;i++){
        mat_t[i]= a + i*n;
    }
    for(i=0;i<m;i++){
        for(j=0;j<n;j++){
            mat_t[i][j]=mat[j][i];
        }
    }
    return mat_t;
}

/*
given a matrix, return it's diagonal
*/
double* get_diag(double** mat, int n){
    double* vec;
    int i;
    vec=calloc(n,sizeof(double));
    assert(vec!=NULL);
    for (i=0; i<n; i++){
        vec[i]=mat[i][i];
    }
    return vec;
}

double get_abs(double num){
    if(num<0){
        return -1*num;
    }
    return num;
}

/*
given a matrix return the the cordinates of the biggest element in the matrix that isn't in the diagonal
*/
int* get_max_indexes(double** mat, int n){
    int i,j,x,y;
    double max;
    int* cor;
    cor=calloc(2,sizeof(int));
    assert(cor!=NULL);
    max=mat[0][1];
    x=0;
    y=1;
    for (i=0;i<n;i++){
        for(j=0;j<n;j++){
            if(i!=j){
                if(get_abs(mat[i][j])>max){
                    max=get_abs(mat[i][j]);
                    x=i;
                    y=j;
                }
            }
        }
    }
    cor[0]=x;
    cor[1]=y;
    return cor;
}

double compute_theta(double x, double y, double z){
    double ret;
    ret = x-y;
    ret = ret / (2*z);
    return ret;
}

double get_sign(double x){
    if (x>=-0.000000000001){
        return 1;
    }
    else{
        return -1;
    }

}

double compute_t(double x){
    double si;
    double sq;

    si=get_sign(x);
    sq=sqrt(pow(x,2)+1);
    return (si)/(get_abs(x)+sq);
}

double compute_c(double x){
    return 1/sqrt(pow(x,2)+1);
}

double square_sum(double** mat, int n){
    double sum;
    int i,j;
    sum=0;
    for (i=0;i<n;i++){
        for (j=0;j<n;j++){
            sum+=pow(mat[i][j],2);
        }
    }
    return sum;
}

double compute_off(double** mat, int n){
    double x;
    double* diag;
    diag=get_diag(mat,n);
    x=pow(norm(diag,n),2);
    free(diag);
    return square_sum(mat,n) - x;
}

double** many_mul(double*** mat, int n, int m){
    double **a;
    int i;
    a=mat[0];
    for (i=1;i<m;i++){
        a=sq_matrix_mul(a,mat[i],n);
    }
    return a;
}

double** get_eye_mat(int dim){
    double** mat;
    int i;
    double* arr = calloc(dim, sizeof(double)); 
    for(i=0;i<dim;i++){
        arr[i] = 1;
    }
    mat= get_diag_mat(arr, dim);
    free(arr);
    return mat;
}

double** create_p_matrix(double s, double c, int x, int y, int n){
    double **mat;
    mat = get_eye_mat(n);
    mat[x][y] = s;
    mat[y][x] = -s;
    mat[x][x] = c;
    mat[y][y] = c;
    return mat;
}

double* get_deltas(double* lst, int n){
    double* n_list;
    int i;
    double x;
    n_list=calloc(n-1,sizeof(double));
    assert(n_list!=NULL);
    for (i=0; i<n-1; i++){
        x=lst[i]-lst[i+1];
        n_list[i]=x*get_sign(x);
    }
    return n_list;
}

int determine_k(double* lst, int n){
    int x,i,y;
    double max;
    x=0;
    y=(n+1)/2;
    max=lst[0];
    for (i=1; i<y;i++){
        if (lst[i]>max){
            max=lst[i];
            x=i;
        }
    }
    return x;
}

/*
function to double arrays elements
*/
void d_swap(double **a, double **b) {
  double* t = *a;
  *a = *b;
  *b = t;
}


/*
function to swap double elements
*/ 
void swap(double *a, double *b) {
  double t = *a;
  *a = *b;
  *b = t;
}

/* function to find the partition position
*/
int partition(double *array, double** vects, int low, int high) {
  
  double pivot = array[high];
  int i = (low - 1);
  int j;
  for (j = low; j < high; j++) {
    if (array[j] <= pivot) {
      i++;
      swap(&array[i], &array[j]);
      d_swap(&vects[i], &vects[j]);
    }
  }
  swap(&array[i + 1], &array[high]);
  d_swap(&vects[i+1], &vects[high]);
  return (i + 1);
}

void quickSort(double *array,double** vects, int low, int high) {
  if (low < high) {
    int pi = partition(array, vects, low, high);
    quickSort(array, vects, low, pi - 1);
    quickSort(array, vects, pi + 1, high);
  }
}

int* read_file_dimensions(char* filename){
    int* arr;
    double value;
    char c;
    int vec_len;
    int vec_num;
    int flag;
    int read;
    FILE* f = fopen(filename, "r");

    flag=0;
    vec_len=0;
    vec_num=0;
    arr = calloc(2, sizeof(int));

    while (!feof(f)) {
        read=fscanf(f,"%lf%c", &value, &c);
        if(read == 2){
            if(c == '\n'){
                if (flag==0){
                    vec_len++;
                    flag=1;
                }
                vec_num++;
            } 
            if(c ==',' && !flag){
             vec_len++;
            }
        }
        else if(read ==1){
            if(flag==0){
                vec_len++;
            }
              vec_num++;
        }
     }
     fclose(f);
     arr[0] = vec_len;
     arr[1] = vec_num;
     return arr;
}


double** init_2d_array(int n, int m)
{
   double *a;
    double **mat;
    int i;
    a=calloc(n*m,sizeof(double));
    assert(a!=NULL);
    mat=calloc(n,sizeof(double*));  
    assert(mat!=NULL);
    for (i=0;i<n;i++){
        mat[i]= a + i*m;
    }
    return mat;
}

double** get_points_from_file(char* filename, int vec_len, int vec_num){
    double** points;
    FILE* f;
    int i,j;
    char c;
    double value;
    int read;

    points = init_2d_array(vec_num, vec_len);

    i=0;
    j=0;
    f = fopen(filename, "r");
    while (!feof(f)) {
        read=fscanf(f, "%lf%c", &value, &c);
        if(read == 2){
            points[i][j] = value;
            if(c == '\n'){
                i++;
                j=0;
            }
            if(c ==',') j++;
        }
        else if (read == 1){
            points[i][j] = value;
        }
     }
    fclose(f);

     return points;
}

double* get_diag_vec(double** weighted, int dim){
    double* diag;
    int i;

    diag = calloc(dim, sizeof(double));
    for(i=0; i<dim; i++){
        diag[i] = sum_line(weighted, dim, i);
    }
    return diag;
}


double** get_normalized_matrix(double** weighted, double* diag, int dim){ 
    double** normalized;
    double** eye;

    mul_lines(weighted, diag, dim); 
    mul_columns(weighted, diag, dim);
    eye=get_eye_mat(dim);
    normalized = matrix_sub(eye, weighted, dim);
    free(eye);
    return normalized;
}

double** deep_copy(double** mat, int dim){
    double** ret;
    int j,i;

    ret = init_2d_array(dim,dim);

    for(i=0;i<dim;i++){
        for(j=0;j<dim;j++){
            ret[i][j] = mat[i][j];
        }
    }
    return ret;

}

void compute_normalized(double** mat, int dim, double c, double s, int i, int j){
    int t,k;

    double temp;
    double** copy;

    copy = deep_copy(mat,dim);
    for(k=0;k<dim;k++){
        if(k == i || k == j){
        continue;}
        temp = c*copy[k][i]-s*copy[k][j];
        mat[k][i] = temp;
        mat[i][k] = temp;
    }

    for(t=0;t<dim;t++){
        if(t == i || t == j){
        continue;}
        temp = c*copy[t][j]+s*copy[t][i];
        mat[t][j] = temp;
        mat[j][t] = temp;
    }


    mat[i][i] = (pow(c,2)*copy[i][i])+(pow(s,2)*copy[j][j])-(2*s*c*copy[i][j]);  
    mat[j][j] = (pow(c,2)*copy[j][j])+(pow(s,2)*copy[i][i])+(2*s*c*copy[i][j]); 

    mat[i][j] = 0;
    mat[j][i] = 0;
    free(copy);
}

typedef struct eigen_ret{
    double * eigen_values;
    double** eigen_vectors;
    int k;
} EIGEN;

typedef EIGEN* EIGEN_LINK;

EIGEN_LINK get_eigens_and_k(double** normalized, int dim, int k){  
    int* indexes;
    double temp;
    double** V;
    double** t_V;
    double** P;
    double c, s, t, theta;
    double* deltas;
    double* eigen_vals;
    EIGEN_LINK ret;
    int i;
    int j;
    double epsilon;

    epsilon = pow(10, -15);

    ret = malloc(sizeof(EIGEN));
        
    V = get_eye_mat(dim);

    do{
        temp = compute_off(normalized, dim);
        indexes = get_max_indexes(normalized, dim);
        i = indexes[0];
        j = indexes[1];

        theta = compute_theta(normalized[j][j], normalized[i][i], normalized[i][j]);
        t = compute_t(theta);
        c = compute_c(t);
        s = t*c;

        P = create_p_matrix(s,c,i,j,dim);

        V = sq_matrix_mul(V, P, dim);

        compute_normalized(normalized,dim,c,s,i,j);

    } while((temp-compute_off(normalized, dim)) > epsilon);

    t_V = transpose(V,dim, dim);

    eigen_vals = get_diag(normalized, dim);

    quickSort(eigen_vals, t_V,0, dim-1);

    if(!k){
        deltas = get_deltas(eigen_vals, dim);
        k = determine_k(deltas,dim-1) + 1;
        free(deltas);
    }

    ret->k = k;
    ret->eigen_vectors = transpose(t_V, dim, dim);
    ret->eigen_values = eigen_vals;
    
    free(indexes);
    free(t_V);
    free(V[0]);
    free(V);
    free(P[0]);
    free(P);
    return ret;
}

int kmeans_goal(double** points, char* goal, int vec_num, int dim){
    EIGEN_LINK eigens;
    double** weighted;
    double** normalized;
    double* diag;
    double** ddg;

    if(!strcmp(goal,"jacobi")){
        eigens = get_eigens_and_k(points, vec_num, vec_num);
        print_vec(eigens->eigen_values, vec_num);
        print_mat(transpose(eigens->eigen_vectors, vec_num, vec_num), vec_num, vec_num);
        free(eigens->eigen_values);
        free(eigens->eigen_vectors[0]);
        free(eigens->eigen_vectors);
        free(eigens);
        return 0;

    }

    weighted = weighted_matrix(points, dim, vec_num);

    if(!strcmp(goal,"wam")){
        print_mat(weighted, vec_num, vec_num);
        free(weighted[0]);
        free(weighted);
        return 0;
    }

    diag = get_diag_vec(weighted, vec_num);


    if(!strcmp(goal,"ddg")){
        ddg = get_diag_mat(diag, vec_num);
        print_mat(ddg, vec_num, vec_num);
        free(diag);
        free(weighted[0]);
        free(weighted);
        free(ddg[0]);
        free(ddg);
        return 0;
    }

    div_square_vec(diag, vec_num);

    normalized = get_normalized_matrix(weighted,diag, vec_num);

    if(!strcmp(goal,"lnorm")){
        print_mat(normalized, vec_num, vec_num);
        free(diag);
        free(weighted[0]);
        free(weighted);
        free(normalized[0]);
        free(normalized);
        return 0;
    }

  
    return 0;
}

EIGEN_LINK get_spk_points(double** points, int dim, int vec_num, int k){
    EIGEN_LINK eigens;
    double** weighted;
    double** normalized;
    double* diag;

    weighted = weighted_matrix(points, dim, vec_num);

    diag = get_diag_vec(weighted, vec_num);

    div_square_vec(diag, vec_num);

    normalized = get_normalized_matrix(weighted,diag, vec_num);

    eigens = get_eigens_and_k(normalized, vec_num, k);

    normalize_mat(eigens->eigen_vectors, vec_num, eigens->k);

    free(points);
    free(weighted[0]);
    free(weighted);
    free(normalized[0]);
    free(normalized);
    free(diag);

    return eigens;
}

void kmeans(double** points, double** centers, int vec_cnt, int k, int max_iter){

    int i;
    int j;
    int center;
    int *vec_to_cen;
    CEN_LINK clusters;
    int bool;
    int old_center;

    vec_to_cen= calloc(vec_cnt,sizeof(int));
    assert(vec_to_cen != NULL);

    clusters = init_clusters(k, k);

    for (i = 0; i < vec_cnt; i++)
    {
       center = classify(points[i],  k, k, centers);

       vec_to_cen[i] = center;

       add_array(clusters[center].sum , points[i], k);

       clusters[center].cnt++;

    }

    bool = 1;
    for (i=0; i<max_iter-1;i++){

        for(j=0;j<vec_cnt;j++){

            center = classify(points[j] ,k, k, centers);

            old_center = vec_to_cen[j];

            if (old_center!=center){

                sub_array(clusters[old_center].sum, points[j], k);

                add_array(clusters[center].sum , points[j], k);

                clusters[center].cnt++;
                clusters[old_center].cnt--;

                vec_to_cen[j] = center;

                bool = 1;
            }

        }

        if (bool == 0){

            break;

        }

        for (j = 0; j < k; j++)
        {

            update_center(centers[j], clusters[j], k);

        }

        bool = 0;
    }

    free(vec_to_cen);
    free(clusters);

    print_mat(centers,k,k);

}

int main(int argv, char** args){
    int k;
    double** points;
    double** weighted;
    double** normalized;
    double** centers;
    double** transp;
    double** diag_mat;
    int* dims;
    int vec_len, vec_num;
    EIGEN_LINK eigens;
    char* goal;
    char* file_name;
    double* diag;

    assert(argv==4);
    k = atoi(args[1]);
    goal = args[2];
    file_name = args[3];

    dims = read_file_dimensions(file_name);
    vec_len = dims[0];
    vec_num = dims[1];

    points = get_points_from_file(file_name, vec_len, vec_num);

    if(!strcmp(goal,"jacobi")){
        eigens = get_eigens_and_k(points, vec_num, vec_num);
        print_vec(eigens->eigen_values, vec_num);
        transp=transpose(eigens->eigen_vectors, vec_num, vec_num);
        print_mat(transp, vec_num, vec_num);
        free(transp[0]);
        free(transp);
        free(eigens->eigen_values);
        free(eigens->eigen_vectors[0]);
        free(eigens->eigen_vectors);
        free(points[0]);
        free(points);
        free(dims);
        return 0;

    }

    weighted = weighted_matrix(points, vec_len, vec_num);

    if(!strcmp(goal,"wam")){
        print_mat(weighted, vec_num, vec_num);
        free(points[0]);
        free(points);
        free(weighted[0]);
        free(weighted);
        free(dims);
        return 0;
    }

    diag = get_diag_vec(weighted, vec_num);

    if(!strcmp(goal,"ddg")){
        diag_mat=get_diag_mat(diag, vec_num);
        print_mat(diag_mat, vec_num, vec_num);
        free(diag_mat[0]);
        free(diag_mat);
        free(points[0]);
        free(points);
        free(dims);
        free(diag);
        free(weighted[0]);
        free(weighted);
        return 0;
    }

    div_square_vec(diag, vec_num);
    normalized = get_normalized_matrix(weighted, diag, vec_num);

    if(!strcmp(goal,"lnorm")){
        print_mat(normalized, vec_num, vec_num);
        free(points[0]);
        free(points);
        free(dims);
        free(normalized[0]);
        free(normalized);
        free(diag);
        free(weighted[0]);
        free(weighted);
        return 0;
    }

    eigens = get_eigens_and_k(normalized, vec_num, 0);

    k = eigens->k;  
  
    normalize_mat(eigens->eigen_vectors, vec_num, k);

    centers = deep_copy(eigens->eigen_vectors, k);


    if(!strcmp(goal,"spk")){
    kmeans(eigens->eigen_vectors, centers,vec_num, k, 300);
    }
    
    free(dims);
    free(weighted[0]);
    free(weighted);
    free(normalized[0]);
    free(normalized);
    free(points[0]);
    free(points);
    free(centers[0]);
    free(centers);
    free(eigens->eigen_values);
    free(eigens->eigen_vectors[0]);
    free(eigens->eigen_vectors);
    free(diag);

    return 0;

    }
