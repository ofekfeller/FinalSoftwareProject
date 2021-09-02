#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "spkmeans.h"
#include <Python.h>

static PyObject* execute_goal(PyObject *self, PyObject *args){  //fix memory issues
    int dim;
    int vec_num;
    char* goal;
    int i,j;

    PyObject *_points, *val;

    if(!PyArg_ParseTuple(args, "Os", &_points, &goal)) {   //make sure we can parse goal as char* and not pythonic object
        return NULL;
    }

    if(!PyList_Check(_points)){
        return NULL;
    }

 
    /* Get the size of it and build the output list*/
    
    vec_num = PyList_Size(_points);
    dim = PyList_Size(PyList_GetItem(_points, 0));
    
    double** points = calloc(vec_num, sizeof(double*));
    assert(points!=NULL);
    for(i=0; i<vec_num; i++){
            points[i] = calloc(dim, sizeof(double));
            assert(points[i]!=NULL);
        for(j=0; j<dim; j++){
            val = PyList_GetItem(PyList_GetItem(_points,i), j);
            points[i][j] = PyFloat_AsDouble(val);
        }
    }

    kmeans_goal(points, goal, vec_num, dim);

    Py_RETURN_NONE;
}

static PyObject* spk_points(PyObject *self, PyObject *args)
{
    int dim, i, j;
    int vec_num;
    Py_ssize_t _K;
    EIGEN_LINK res;

    PyObject *_points, *_k_ls, *val;


    if(!PyArg_ParseTuple(args, "OO", &_points, &_k_ls)) {
        return NULL;
    }
    if(!PyList_Check(_points)){
        return NULL;
    }

    _K = PyList_Size(_k_ls);

    /* Get the size of it and build the output list*/
    
    vec_num = PyList_Size(_points);
    dim = PyList_Size(PyList_GetItem(_points, 0));

    double** points = calloc(vec_num, sizeof(double*));
    assert(points!=NULL);
    for(i=0; i<vec_num; i++){
            points[i] = calloc(dim, sizeof(double));
            assert(points[i]!=NULL);
        for(j=0; j<dim; j++){
            val = PyList_GetItem(PyList_GetItem(_points,i), j);
            points[i][j] = PyFloat_AsDouble(val);
        }
    }

    res = get_spk_points(points, dim, vec_num, _K);

    PyObject * all_points = PyList_New(vec_num);
    PyObject * vector;
    for(i=0; i<vec_num; i++){
        vector = PyList_New(res->k);
        for(j=0; j<res->k; j++){

            PyList_SetItem(vector, j, PyFloat_FromDouble(res->eigen_vectors[i][j]));
        }
        PyList_SetItem(all_points, i, vector);
    }

    free(res);
   
    return all_points;
    
}

static PyObject* fit(PyObject *self, PyObject *args)
{

    PyObject *_points, *_centers, *val;
    Py_ssize_t dim, vec_num, i, j, _K;

    if(!PyArg_ParseTuple(args, "OO", &_points,&_centers)) {
        return NULL;
    }
    if(!PyList_Check(_points) || !PyList_Check(_centers)){
        return NULL;
    }

    /* Get the size of it and build the output list*/
    vec_num = PyList_Size(_points);
    dim = PyList_Size(PyList_GetItem(_points, 0));


    double** points = calloc(vec_num, sizeof(double*));
    assert(points!=NULL);
    for(i=0; i<vec_num; i++){
            points[i] = calloc(dim, sizeof(double));
            assert(points[i]!=NULL);
        for(j=0; j<dim; j++){
            val = PyList_GetItem(PyList_GetItem(_points,i), j);
            points[i][j] = PyFloat_AsDouble(val);
        }
    }

    _K = PyList_Size(_centers);
    double** centers = calloc(_K, sizeof(double*));
    assert(centers!=NULL);
    for(i=0; i<_K; i++){
        centers[i] = calloc(dim, sizeof(double));
        assert(centers[i]!=NULL);
        for(j=0; j<dim; j++){
            val = PyList_GetItem(PyList_GetItem(_centers,i), j);
            centers[i][j] = PyFloat_AsDouble(val);
        }
    }
    kmeans(points, centers, vec_num, _K, 300);

    free(points);
    free(centers);

    Py_RETURN_NONE;

}

/*
 * A macro to help us with defining the methods
 * Compare with: {"f1", (PyCFunction)f1, METH_NOARGS, PyDoc_STR("No input parameters")}
*/
#define FUNC(_flag, _name, _docstring) { #_name, (PyCFunction)_name, _flag, PyDoc_STR(_docstring) }

static PyMethodDef _methods[] = {
    FUNC(METH_VARARGS, fit, "Find k means clusters given points, early centers and some shitty integers"),
    FUNC(METH_VARARGS, execute_goal, "apply goal on the input given"),
    FUNC(METH_VARARGS, spk_points, "return new points of spk process"),
    {NULL, NULL, 0, NULL}   /* sentinel */
};

static struct PyModuleDef _moduledef = {
    PyModuleDef_HEAD_INIT,
    "spkmeansmodule",
    NULL,
    -1,
    _methods
};

PyMODINIT_FUNC
PyInit_spkmeansmodule(void)
{
    return PyModule_Create(&_moduledef);
}
