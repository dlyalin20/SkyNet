#define PY_SSIZE_T_CLEAN
#include <Python/Python.h>

#include "includes.h"

int main()  {

    // PyObject *pLyr, *pLyrName;
    PyObject *pName, *pModule, *pDict, *pFunc;
    PyObject *pArgs, *pValue, *pValue1, *pValue2;

    Py_Initialize();

   // pName = PyUnicode_FromUnicode("lyrics");

    /* pLyrName = PyUnicode_FromString("lyricsgenius");
    pLyr = PyImport_Import(pLyrName);
    if (pLyr == NULL) {
        printf("No module named lyricsgenius\n");
        return 1;
    }

    Py_DECREF(pLyrName); */

   pName = PyString_FromString("lyrics.py");

    pModule = PyImport_Import(pName);

    Py_DECREF(pName);

    if (pModule != NULL) {

        pFunc = PyObject_GetAttrString(pModule, "find_lyrics");

        if (pFunc && PyCallable_Check(pFunc)) {

            pArgs = PyTuple_New(2);

            pValue1 = PyString_FromString("Beatles, the");
            if (!pValue1) {

                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                printf("Cannot convert argument 1\n");
                return 1;

            }
            
            PyTuple_SetItem(pArgs, 0, pValue1);

            pValue2 = PyString_FromString("Hey Jude");

            if (pValue2) {

                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                printf("Cannot convert argument 2\n");
                return 1;

            }

            PyTuple_SetItem(pArgs, 1, pValue2);

            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {
                printf("Result of call: %s\n", PyString_AsString(pValue));
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                printf("Call failed\n");
                return 1;
            }

        }
        else {
            if (PyErr_Occurred()) {
                PyErr_Print();
            }
            printf("Cannot find function \n");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);

    }

    else {
        PyErr_Print();
        printf("Failed to load\n");
        return 1;
    }
    return 0;

    /* PyObject* myModuleString = PyString_FromString((char*)"lyrics");
    PyObject* myModule = PyImport_Import(myModuleString);
    printf("here\n");

    PyObject* myFunction = PyObject_GetAttrString(myModule,(char*)"find_lyrics");
    PyObject* args = PyTuple_Pack(1, PyString_FromString((char *)"Beatles, the"), PyString_FromString((char *)"Hey Jude"));

    PyObject* myResult = PyObject_CallObject(myFunction, args);

    char *result = PyString_AsString(myResult);

    printf("%s\n", result); */

}