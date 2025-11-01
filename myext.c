#include <tcl.h>
#include <string.h>
#include <math.h>   // for floor()
#include <limits.h> // for LLONG_MIN, LLONG_MAX
extern int evaluate_d_expression(const char* expr, double* result);


// INT_64 definition for cross-platform compatibility
#ifdef _WIN32
typedef __int64 INT_64;
#else
typedef long long INT_64;
#endif

extern int evaluate_ll_expression(char* stringIn, INT_64* result);


// Command implementation for "="
// Concatenates all arguments with spaces and evaluates as expression
static int Equal_Cmd(ClientData clientData, Tcl_Interp* interp,
    int objc, Tcl_Obj* const objv[]) {
    Tcl_Obj* exprObj;
    char* exprStr;
    double result;
    int status;
    char errorMsg[256];
    int i;
    int errorPos;

    if (objc < 2) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("No expression provided", -1));
        return TCL_ERROR;
    }

    // Concatenate all arguments with spaces
    exprObj = Tcl_NewStringObj(Tcl_GetString(objv[1]), -1);
    for (i = 2; i < objc; i++) {
        Tcl_AppendToObj(exprObj, " ", 1);
        Tcl_AppendToObj(exprObj, Tcl_GetString(objv[i]), -1);
    }

    // Get the concatenated string
    exprStr = Tcl_GetString(exprObj);

    // Evaluate the expression
    status = evaluate_d_expression(exprStr, &result);

    // Free the temporary object
    Tcl_DecrRefCount(exprObj);

    if (status == 0) {
        // Success - return as integer if it's a whole number
        if (result == floor(result) &&
            result >= LLONG_MIN && result <= LLONG_MAX) {
            // It's a whole number - return as integer
            Tcl_SetObjResult(interp, Tcl_NewWideIntObj((Tcl_WideInt)result));
        }
        else {
            // Has fractional part - return as double
            Tcl_SetObjResult(interp, Tcl_NewDoubleObj(result));
        }
        return TCL_OK;
    }

    // Handle errors
    if (status == 2) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("Unbalanced parentheses", -1));
    }
    else if (status == 3) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("Exceeded recursion depth maximum", -1));
    }
    else if (status >= 0x4000) {
        // Overflow error
        errorPos = status - 0x4000;
        sprintf(errorMsg, "Overflow at character position %d", errorPos);
        Tcl_SetObjResult(interp, Tcl_NewStringObj(errorMsg, -1));
    }
    else if (status >= 0x3000) {
        // Bad function name
        errorPos = status - 0x3000;
        sprintf(errorMsg, "Bad function name at character position %d", errorPos);
        Tcl_SetObjResult(interp, Tcl_NewStringObj(errorMsg, -1));
    }
    else if (status >= 0x2000) {
        // Bad character
        errorPos = status - 0x2000;
        sprintf(errorMsg, "Invalid character at position %d", errorPos);
        Tcl_SetObjResult(interp, Tcl_NewStringObj(errorMsg, -1));
    }
    else if (status >= 0x1000) {
        // General syntax error
        errorPos = status - 0x1000;
        sprintf(errorMsg, "Syntax error at character position %d", errorPos);
        Tcl_SetObjResult(interp, Tcl_NewStringObj(errorMsg, -1));
    }
    else {
        sprintf(errorMsg, "Unknown error (code 0x%X)", status);
        Tcl_SetObjResult(interp, Tcl_NewStringObj(errorMsg, -1));
    }

    return TCL_ERROR;
}
// Command implementation for "=l"
// Concatenates all arguments with spaces and evaluates as long long expression
static int EqualLong_Cmd(ClientData clientData, Tcl_Interp* interp,
    int objc, Tcl_Obj* const objv[]) {
    Tcl_Obj* exprObj;
    char* exprStr;
    INT_64 result;
    int status;
    char errorMsg[256];
    int i;
    int errorPos;

    if (objc < 2) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("No expression provided", -1));
        return TCL_ERROR;
    }

    // Concatenate all arguments with spaces
    exprObj = Tcl_NewStringObj(Tcl_GetString(objv[1]), -1);
    for (i = 2; i < objc; i++) {
        Tcl_AppendToObj(exprObj, " ", 1);
        Tcl_AppendToObj(exprObj, Tcl_GetString(objv[i]), -1);
    }

    // Get the concatenated string
    exprStr = Tcl_GetString(exprObj);

    // Evaluate the expression
    status = evaluate_ll_expression(exprStr, &result);

    // Free the temporary object
    Tcl_DecrRefCount(exprObj);

    if (status == 0) {
        // Success - return the numeric result as wide integer
        Tcl_SetObjResult(interp, Tcl_NewWideIntObj((Tcl_WideInt)result));
        return TCL_OK;
    }

    // Handle errors - same as Equal_Cmd
    if (status == 2) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("Unbalanced parentheses", -1));
    }
    else if (status == 3) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("Exceeded recursion depth maximum", -1));
    }
    else if (status >= 0x4000) {
        errorPos = status - 0x4000;
        sprintf(errorMsg, "Overflow at character position %d", errorPos);
        Tcl_SetObjResult(interp, Tcl_NewStringObj(errorMsg, -1));
    }
    else if (status >= 0x3000) {
        errorPos = status - 0x3000;
        sprintf(errorMsg, "Bad function name at character position %d", errorPos);
        Tcl_SetObjResult(interp, Tcl_NewStringObj(errorMsg, -1));
    }
    else if (status >= 0x2000) {
        errorPos = status - 0x2000;
        sprintf(errorMsg, "Invalid character at position %d", errorPos);
        Tcl_SetObjResult(interp, Tcl_NewStringObj(errorMsg, -1));
    }
    else if (status >= 0x1000) {
        errorPos = status - 0x1000;
        sprintf(errorMsg, "Syntax error at character position %d", errorPos);
        Tcl_SetObjResult(interp, Tcl_NewStringObj(errorMsg, -1));
    }
    else {
        sprintf(errorMsg, "Unknown error (code 0x%X)", status);
        Tcl_SetObjResult(interp, Tcl_NewStringObj(errorMsg, -1));
    }

    return TCL_ERROR;
}
// Package initialization function
// Must be named Myext_Init to match the DLL name
#ifdef _WIN32
__declspec(dllexport)
#endif
int Myext_Init(Tcl_Interp *interp) {
    // Initialize Tcl stubs
#ifdef TCL9
    if (Tcl_InitStubs(interp, "9.0", 0) == NULL) {
#else
    if (Tcl_InitStubs(interp, "8.6", 0) == NULL) {
#endif
        return TCL_ERROR;
    }
    
    // Create the "=" command
    Tcl_CreateObjCommand(interp, "=", Equal_Cmd, NULL, NULL);

    // Create the "=l" command (long long integer)
    Tcl_CreateObjCommand(interp, "=l", EqualLong_Cmd, NULL, NULL);

    // Provide the package
    if (Tcl_PkgProvide(interp, "myext", "1.0") != TCL_OK) {
        return TCL_ERROR;
    }
    
    return TCL_OK;
}
