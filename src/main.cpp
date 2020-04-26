#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include<cstdio>
#include<windows.h>
#include<winuser.h>
#include "jcalg1.h"

/* Documentation */
PyDoc_STRVAR(
        compress_doc,
        "compress(data, level=-1, skip_checksum=False /)\n"
        "--\n"
        "\n"
        "Compresses data using the JCALG1 compression algorithm.\n\n"
        ":param data: The data to compress.\n"
        ":type data: bytes\n\n"
        ":param level: Compression level (between 1 and 9).\n"
        ":type level: int\n\n"
        ":param skip_checksum: If true, checksum will not be calculated.\n"
        ":type skip_checksum: bool\n\n"
        ":returns: a compressed bytes object.\n"
        ":rtype: bytes\n"
);
PyDoc_STRVAR(
        decompress_doc,
        "decompress(data /)\n"
        "--\n"
        "\n"
        "Decompresses a JCALG1 compressed bytes object.\n\n"
        ":param data: The data to decompress.\n"
        ":type data: bytes\n\n"
        ":returns: an uncompressed bytes object.\n"
        ":rtype: bytes\n"
);
PyDoc_STRVAR(
        module_doc,
        "Interface to the JCALG1 compression library.\n\n"
        "This module provides a method for compressing, and a method for decompressing data.\n\n"
        "Algorithm author: (c)1999-2003 Jeremy Collake jeremy@bitsum.com."
);

/*Functions for jcalg1 */
void *_stdcall AllocFunc(DWORD nMemSize) { return (void *) GlobalAlloc(GMEM_FIXED, nMemSize); }

bool _stdcall DeallocFunc(void *pBuffer) {
    GlobalFree((HGLOBAL) pBuffer);
    return true;
}

bool _stdcall CallbackFunc(DWORD pSourcePos, DWORD pDestinationPos) { return true; }

/* Internal function to get the window size */
static int _get_window_size(int level) {
    switch (level) {
        case 1:
            return 4 * 1024;
        case 2:
            return 8 * 1024;
        case 3:
            return 16 * 1024;
        case 4:
            return 32 * 1024;
        case 5:
            return 64 * 1024;
        case -1: // default
        case 6:
            return 128 * 1024;
        case 7:
            return 256 * 1024;
        case 8:
            return 512 * 1024;
        case 9:
            return 1024 * 1024;
        default:
            return -1; // error
    }
}

int jcalg_compress_impl(char *in_buff, int size, char *out_buff, int window_size, int checksum) {
    return JCALG1_Compress(
            (void *) in_buff,
            size,
            (void *) out_buff,
            window_size,
            &AllocFunc, &DeallocFunc, &CallbackFunc, checksum);
}


static PyObject *jcalg_compress(PyObject *self, PyObject *args, PyObject *kw) {

    char *in_buff;          // bytes-like input
    int in_size;            // size of uncompressed input
    char *out_buff;         // bytes-like output
    int level = -1;         // compression level
    int checksum = 0;       // if true, output will not contain checksum
    static char *keywords[] = {"", "level", "skip_checksum", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kw, "s#|ib", keywords, &in_buff, &in_size, &level, &checksum))
        return nullptr;

    int window_size = _get_window_size(level);
    if (window_size < 0) {
        PyErr_SetString(PyExc_ValueError, "Bad compression level");
        return nullptr;
    }

    if (in_size == 0) {
        PyErr_SetString(PyExc_ValueError, "No data");
        return nullptr;
    }

    out_buff = (char *) malloc(in_size + 4);
    int compressed_size = jcalg_compress_impl(in_buff, in_size, out_buff, window_size, checksum);

    return PyBytes_FromStringAndSize(out_buff, compressed_size);
}

static PyObject *jcalg_decompress(PyObject *self, PyObject *args) {

    char *in_buff;  // bytes-like input (compressed)
    int in_size;    // size of compressed input
    char *out_buff; // bytes-like output (uncompressed)
    int out_size;   // size of uncompressed output

    if (!PyArg_ParseTuple(args, "s#", &in_buff, &in_size))
        return nullptr;

    out_size = JCALG1_GetUncompressedSizeOfCompressedBlock((void *) in_buff);
    if (!out_size) {
        PyErr_SetString(PyExc_ValueError, "Invalid compressed data");
        return nullptr;
    }

    out_buff = (char *) malloc(out_size);
    JCALG1_Decompress_Fast((void *) in_buff, out_buff);

    return PyBytes_FromStringAndSize(out_buff, out_size);
}

static PyMethodDef Jcalg1Modules[] = {

        {"compress",   (PyCFunction) jcalg_compress, METH_VARARGS | METH_KEYWORDS, compress_doc},
        {"decompress", jcalg_decompress, METH_VARARGS, decompress_doc},

        {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef jcalg1module = {
        PyModuleDef_HEAD_INIT,
        "jcalg1",   /* name of module */
        module_doc, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        Jcalg1Modules
};

PyMODINIT_FUNC PyInit_jcalg1(void) {
    return PyModule_Create(&jcalg1module);
}

int main(int argc, char *argv[]) {

    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    PyImport_AppendInittab("jcalg1", PyInit_jcalg1);

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyImport_ImportModule("jcalg1");

    PyMem_RawFree(program);
    return 0;
}